#include "pipeline.h"
#include "mips_parser.h"
#include "symbol_table.h"
#include "error_handling.h"
#include "utils.h"
#include "update.h"
#include <assert.h>
#include <stdlib.h>

// Private variables and functions -------------------------------------

static int nr_instr_retired;
static int nr_cycles;
static int trace;
static FILE* trace_out;

static void LinkMIPSProgram(ParsedInstr* instr_list, int nr_instr)
{
	int i;
	long long a;
	
	if (!SymbolTableHasSymbol("__start"))
		STATIC_MIPS_ERROR("Program misses an entry point (should be labeled \"__start\")")

	a = GetTextSegmentStartingAddress();
	for (i = 0; i < nr_instr; ++i)
	{
		switch (instr_list[i].instr.type)
		{
		// Data transfer instructions
		case LW:
		case SW:
		case LH:
		case LHU:
		case SH:
		case LB:
		case LBU:
		case SB:
			{
				long long addr = LookupInSymbolTable(instr_list[i].label);
				instr_list[i].instr.rs = ZERO;
				instr_list[i].instr.imm = addr;
			}
			break;
		// Branch instructions
		case BEQ:
		case BNE:
			{
				long long addr = LookupInSymbolTable(instr_list[i].label);
				instr_list[i].instr.imm = addr;
			}
			break;
		// Jump instructions
		case J:
			{
				long long addr = LookupInSymbolTable(instr_list[i].label);
				instr_list[i].instr.imm = addr;
			}
			break;
		default:
			/* no linking needed */;
		}
		WriteToInstrMemory(a, &instr_list[i].instr);
		a += 4;
	}
}

static void InitMemory(FILE* mips_file)
{
	int parsing_failed;
	ParsedInstr* instr_list = NULL;
	int nr_instr, i;

	InitInstrMemory(mips_file);
	InitDataMemory();

	parsing_failed = mips_parser_parse(mips_file, &instr_list, &nr_instr);
	if (parsing_failed)
	{
		fprintf(stderr, "Parsing failed.\n");
		EXIT_APPL(EXIT_FAILURE)
	}

	LinkMIPSProgram(instr_list, nr_instr);

	// Clean up the temporary list of instructions
	for (i = 0; i < nr_instr; ++i)
		free(instr_list[i].label);
	free(instr_list);
}

// Functions for printing HTML to the trace file --------------------------------------------

static void PrintIFStage_HTML(FILE* file)
{
	fprintf(file,
		"<tbody>\n"
		"<tr><td nowrap>IF stage:</td><td/><td/></tr>\n"
		"<tr><td/> <td nowrap>Instruction:&nbsp;</td> <td nowrap><code>");
	PrintInstruction(&if_stage.instr, file);
	fprintf(file,
		"</code> (");
	PrintLineNr(&if_stage.instr, file);
	fprintf(file, ")&nbsp;</td></tr>\n"
		"<tr><td/> <td nowrap>PC:&nbsp;</td> <td nowrap>0x%llX&nbsp;</td></tr>\n"
		"</tbody>\n", if_stage.pc);
}

static void PrintIDStage_HTML(FILE* file)
{
	fprintf(file,
		"<tbody>\n"
		"<tr><td nowrap>ID stage:</td><td/><td/></tr>\n"
		"<tr><td/> <td nowrap>Instruction:&nbsp;</td> <td nowrap><code>");
	PrintInstruction(&id_stage.instr, file);
	fprintf(file,
		"</code> (");
	PrintLineNr(&id_stage.instr, file);
	fprintf(file, ")&nbsp;</td></tr>\n"
		"</tbody>\n");
}

static void PrintExStage_HTML(FILE* file)
{
	fprintf(file,
		"<tbody>\n"
		"<tr><td nowrap>Ex stage:</td><td/><td/></tr>\n"
		"<tr><td/> <td nowrap>Instruction:&nbsp;</td> <td nowrap><code>");
	PrintInstruction(&ex_stage.instr, file);
	fprintf(file,
		"</code> (");
	PrintLineNr(&ex_stage.instr, file);
	fprintf(file, ")&nbsp;</td></tr>\n"
		"<tr><td/> <td nowrap><i>rs</i> value:&nbsp;</td> <td nowrap>%lld&nbsp;</td></tr>\n"
		"<tr><td/> <td nowrap><i>rt</i> value:&nbsp;</td> <td nowrap>%lld&nbsp;</td></tr>\n"
		"<tr><td/> <td nowrap>Immediate value:&nbsp;</td> <td nowrap>%lld&nbsp;</td></tr>\n"
		"</tbody>\n",
		ex_stage.rs_value,
		ex_stage.rt_value,
		ex_stage.imm_value);
}

static void PrintMemStage_HTML(FILE* file)
{
	fprintf(file,
		"<tbody>\n"
		"<tr><td nowrap>Mem stage:</td><td/><td/></tr>\n"
		"<tr><td/> <td nowrap>Instruction:&nbsp;</td> <td nowrap><code>");
	PrintInstruction(&mem_stage.instr, file);
	fprintf(file,
		"</code> (");
	PrintLineNr(&mem_stage.instr, file);
	fprintf(file, ")&nbsp;</td></tr>\n"
		"<tr><td/> <td nowrap><i>rt</i> value:&nbsp;</td> <td nowrap>%lld&nbsp;</td></tr>\n"
		"<tr><td/> <td nowrap>ALU result:&nbsp;</td> <td nowrap>%lld&nbsp;</td></tr>\n"
		"<tr><td/> <td nowrap>Zero:&nbsp;</td> <td nowrap>%d&nbsp;</td></tr>\n"
		"<tr><td/> <td nowrap>Branch target:&nbsp;</td> <td nowrap>%lld&nbsp;</td></tr>\n"
		"</tbody>\n",
		mem_stage.rt_value, 
		mem_stage.alu_result,
		mem_stage.zero,
		mem_stage.branch_target);
}

static void PrintWBStage_HTML(FILE* file)
{
	fprintf(file,
		"<tbody>\n"
		"<tr><td nowrap>WB stage:</td><td/><td/></tr>\n"
		"<tr><td/> <td nowrap>Instruction:&nbsp;</td> <td nowrap><code>");
	PrintInstruction(&wb_stage.instr, file);
	fprintf(file,
		"</code> (");
	PrintLineNr(&wb_stage.instr, file);
	fprintf(file, ")&nbsp;</td></tr>\n"
		"<tr><td/> <td nowrap>Result:&nbsp;</td> <td nowrap>%lld&nbsp;</td></tr>\n"
		"</tbody>\n",
		wb_stage.result);
}

static void PrintRegisterFile_HTML()
{
	int r;
	static const char* spacing = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	fprintf(trace_out,
		"<table rules=groups frame=box width=\"100%%\">\n"
		"<thead><tr><th colspan=8 align=left nowrap>Register File</th></tr></thead>\n"
		"<tbody>\n");
	for (r = 0; r < 8; ++r)
	{
		int c;
		fprintf(trace_out, "<tr>");
		for (c = 0; c < 4; ++c)
		{
			int reg = r + 8 * c;
			fprintf(trace_out, "<td nowrap>R%d (%s)&nbsp;</td> <td nowrap>= %lld%s</td>", reg,
				RegNameAsString(reg), ReadFromRegisterFile(reg), spacing);
		}
		fprintf(trace_out, "</tr>\n");
	}
	fprintf(trace_out,
		"</tbody>\n"
		"</table>\n");
}

static void PrintPipelineInfo_HTML()
{
	fprintf(trace_out,
		"<td>\n"
		"<table rules=groups frame=box width=\"100%%\">\n"
		"<thead><tr><th colspan=3 align=left>Cycle %d (%d retired)</th> </tr></thead>\n",
		nr_cycles, nr_instr_retired);

	PrintIFStage_HTML(trace_out);
	PrintIDStage_HTML(trace_out);
	PrintExStage_HTML(trace_out);
	PrintMemStage_HTML(trace_out);
	PrintWBStage_HTML(trace_out);

	fprintf(trace_out,
		"</table><p/>\n");

	PrintRegisterFile_HTML();

	fprintf(trace_out,
		"</td> <td>&nbsp;</td>\n");
}

static void InitTraceFile(const char* filename)
{
	trace_out = fopen(filename, "w");
	fprintf(trace_out,
		"<html>\n"
		"<head>\n"
		"<style>\n"
		"body { font-family: arial, sans; }\n"
		"</style>\n"
		"</head>\n"
		"<body>\n"
		"<table><tr valign=top>\n");
}

static void FinalizeTraceFile()
{
	fprintf(trace_out, 
		"</tr></table><p/>\n"
		"Time &rarr;\n"
		"</body>\n"
		"</html>\n");

	fclose(trace_out);
}

// Exported stuff ---------------------------------------------------

// The current states of all the state elements in the pipeline
IFStage if_stage;
IDStage id_stage;
ExStage ex_stage;
MemStage mem_stage;
WBStage wb_stage;

void ClearIFStage(IFStage* to_clear)
{
	to_clear->pc = 0;
	to_clear->instr = CreateInstr_Empty(NOP, DONT_CARE);
}

void ClearIDStage(IDStage* to_clear)
{
	to_clear->instr = CreateInstr_Empty(NOP, DONT_CARE);
}

void ClearExStage(ExStage* to_clear)
{
	to_clear->instr = CreateInstr_Empty(NOP, DONT_CARE);
	to_clear->rs_value = to_clear->rt_value
		= to_clear->imm_value = DONT_CARE;
}

void ClearMemStage(MemStage* to_clear)
{
	to_clear->instr = CreateInstr_Empty(NOP, DONT_CARE);
	to_clear->rt_value = to_clear->alu_result = to_clear->branch_target = DONT_CARE;
	to_clear->zero = 0;
}

void ClearWBStage(WBStage* to_clear)
{
	to_clear->instr = CreateInstr_Empty(NOP, DONT_CARE);
	to_clear->result = DONT_CARE;
}

void InitPipeline(FILE* mips_file)
{
	InitMemory(mips_file);

	if_stage.pc = LookupInSymbolTable("__start");
	if_stage.instr = ReadFromInstrMemory(if_stage.pc);
	ClearIDStage(&id_stage);
	ClearExStage(&ex_stage);
	ClearMemStage(&mem_stage);
	ClearWBStage(&wb_stage);
}

void SetTracing(int on_off)
{
	trace = on_off;
}

void RunProgram(const char* filename)
{
	int found_syscall = 0;

	if (trace)
		InitTraceFile(filename);

	for (nr_cycles = 0, nr_instr_retired = 0; !found_syscall; ++nr_cycles)
	{
		if (trace)
			PrintPipelineInfo_HTML();

		// Check if the instruction now leaving the WB stage is a syscall,
		// in which case the simulation should be terminated
		found_syscall = (wb_stage.instr.type == SYSCALL);
		
		// If the instruction now leaving the WB stage is not a nop,
		// we count this as a retired instruction
		if (wb_stage.instr.type != NOP)
			++nr_instr_retired;

		UpdatePipelineState();
	}

	if (trace)
		FinalizeTraceFile();
}

int GetElapsedCycles()
{
	return nr_cycles;
}

int GetNrOfRetiredInstructions()
{
	return nr_instr_retired;
}

float GetCPI()
{
	return ((float) nr_cycles) / ((float) nr_instr_retired);
}