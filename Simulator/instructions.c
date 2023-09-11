#include "instructions.h"
#include "register_file.h"
#include "error_handling.h"
#include "utils.h"
#include <string.h>
#include <assert.h>

static void ValidateRegisterOperand(int register_nr)
{
	assert(register_nr == DONT_CARE || IsValidRegisterNr(register_nr));
}

Instr CreateInstr_Empty(InstrType type, int line_nr)
{
	Instr instr;
	instr.type = type;
	instr.line_nr = line_nr;
	instr.rd = instr.rs = instr.rt = DONT_CARE;
	instr.imm = DONT_CARE;
	return instr;
}

Instr CreateInstr_RRR(InstrType type, int rd, int rs, int rt, int line_nr)
{
	Instr instr = CreateInstr_Empty(type, line_nr);
	ValidateRegisterOperand(rd);
	ValidateRegisterOperand(rs);
	ValidateRegisterOperand(rt);
	instr.rd = rd;
	instr.rs = rs;
	instr.rt = rt;
	return instr;
}

Instr CreateInstr_RR(InstrType type, int rd, int rs, int line_nr)
{
	Instr instr = CreateInstr_Empty(type, line_nr);
	ValidateRegisterOperand(rd);
	ValidateRegisterOperand(rs);
	instr.rd = rd;
	instr.rs = rs;
	return instr;
}

Instr CreateInstr_RRI(InstrType type, int rs, int rt, long long imm, int line_nr)
{
	Instr instr = CreateInstr_Empty(type, line_nr);
	ValidateRegisterOperand(rs);
	ValidateRegisterOperand(rt);
	instr.rs = rs;
	instr.rt = rt;
	instr.imm = imm;
	return instr;
}

Instr CreateInstr_RI(InstrType type, int rt, long long imm, int line_nr)
{
	Instr instr = CreateInstr_Empty(type, line_nr);
	ValidateRegisterOperand(rt);
	instr.rt = rt;
	instr.imm = imm;
	return instr;
}

Instr CreateInstr_I(InstrType type, long long imm, int line_nr)
{
	Instr instr = CreateInstr_Empty(type, line_nr);
	instr.imm = imm;
	return instr;
}

InstrCategory GetInstrCategory(InstrType type)
{
	switch (type)
	{
		// R-type instructions
	case ADD:
	case SUB:
	case ADDI:
	case ANDI:
	case ORI:	
	case AND:
	case OR:
	case NOR:
	case SLL:
	case SRL:
		return ARITHMETIC_LOGICAL;
	case LW:
	case LH:
	case LHU:
	case LB:
	case LBU:
	case LUI:
		return LOAD;
	case SW:
	case SH:
	case SB:
		return STORE;
	case BNE:
	case BEQ:
	case SLT:
	case SLTU:
	case SLTI:
	case SLTIU:
	case J:
		return PROGRAM_CONTROL;
	default:
		return OTHER;
	}
}


int CompareInstrs(const Instr* instr1, const Instr* instr2)
{
	if (instr1->type != instr2->type)
		return 0;

	return memcmp(instr1, instr2, sizeof(Instr)) == 0;
}

int GetRegWrittenByInstr(const Instr* instr)
{
	switch (instr->type)
	{
	// R-type instructions
	case ADD:
	case SUB:
	case AND:
	case OR:
	case NOR:
	case SLT:
	case SLTU:
	case MOV:
		return instr->rd;

	// I-type instructions
	case ADDI:
	case ANDI:
	case ORI:
	case SLL:
	case SRL:
	case LW:
	case LH:
	case LHU:
	case LB:
	case LBU:
	case LUI:
	case SLTI:
	case SLTIU:
	case LI:
	case INCR:
	case DECR:
		return instr->rt;
	
	default:
		return DONT_CARE;
	}
}

void GetRegsReadByInstr(const Instr* instr, int* rs, int* rt)
{
	*rs = *rt = DONT_CARE;

	switch (instr->type)
	{
	// Instructions that read registers rs and rt
	case ADD:
	case SUB:
	case AND:
	case OR:
	case NOR:
	case SW:
	case SH:
	case SB:
	case BEQ:
	case BNE:
	case SLT:
	case SLTU:
		*rs = instr->rs;
		*rt = instr->rt;
		return;

	// I-type instructions that read register rs
	case ADDI:
	case ANDI:
	case ORI:
	case SLL:
	case SRL:
	case LW:
	case LH:
	case LHU:
	case LB:
	case LBU:
	case SLTI:
	case SLTIU:
		*rs = instr->rs;
		return;

	case INCR:
	case DECR:
		*rs = instr->rt;
		return;
	default:
		/* Instruction does not read a register */
		return;
	}
}

const char* StringRepOfInstrType(InstrType type)
{
#define CASE(TYPE) case TYPE: return #TYPE;

	switch (type)
	{
	case NOP: return "nop";
	
	// Arithmetic
	case ADD: return "add";
	case SUB: return "sub";
	case ADDI: return "addi";
	

	// data transfer
	case LW: return "lw";
	case SW: return "sw";
	case LH: return "lh";
	case LHU: return "lhu";
	case SH: return "sh";
	case LB: return "lb";
	case LBU: return "lbu";
	case SB: return "sb";
	case LUI: return "lui";

	// Pseudo
	case MOV: return "move";
	case LI: return "li";
	case INCR: return "incr";
	case DECR: return "decr";

	// logical
	case AND: return "and";
	case OR: return "or";
	case NOR: return "nor";
	case ANDI: return "andi";
	case ORI: return "ori";
	case SLL: return "sll";
	case SRL: return "srl";

	// conditional jumps
	case BEQ: return "beq";
	case BNE: return "bne";
	case SLT: return "slt";
	case SLTU: return "sltu";
	case SLTI: return "slti";
	case SLTIU: return "sltiu";

	// unconditional jumps
	case J: return "j";

	case SYSCALL: return "syscall";
	}
	
	return "(Unknown instruction type)";

#undef CASE
}

void PrintInstruction(const Instr* instr, FILE* file)
{
	fprintf(file, "%s", StringRepOfInstrType(instr->type));
	switch (instr->type)
	{
	case NOP:
	case SYSCALL:
		break;

	// R-type instructions with format "opname rd, rs, rt"
	case ADD:
	case SUB:
	case AND:
	case OR:
	case NOR:
	case SLT:
	case SLTU:
		fprintf(file, " $%s, $%s, $%s",
			RegNameAsString(instr->rd),
			RegNameAsString(instr->rs),
			RegNameAsString(instr->rt));
		break;

	// R-type instructions with format "opname rd, rs"
	case MOV:
		fprintf(file, " $%s, $%s", 
			RegNameAsString(instr->rd),
			RegNameAsString(instr->rs));
		break;

	// I-type instructions with format "opname rt, imm(rs)"
	case LW:
	case SW:
	case LH:
	case LHU:
	case SH:
	case LB:
	case LBU:
	case SB:
		fprintf(file, " $%s, %lld($%s)",
			RegNameAsString(instr->rt),
			instr->imm,
			RegNameAsString(instr->rs));
		break;

	// I-type instructions with format "opname rt, imm"
	case LUI:
	case LI:
	case INCR:
	case DECR:
		fprintf(file, " $%s, %lld", RegNameAsString(instr->rt), instr->imm);
		break;

	// I-type instructions with format "opname rt, rs, imm"
	case ADDI:
	case ANDI:
	case ORI:
	case SLL:
	case SRL:
	case SLTI:
	case SLTIU:
		fprintf(file, " $%s, $%s, %lld",
			RegNameAsString(instr->rt),
			RegNameAsString(instr->rs),
			instr->imm);
		break;

	// I-type instructions with format "opname rs, rt, imm"
	case BEQ:
	case BNE:
		fprintf(file, " $%s, $%s, %llX",
			RegNameAsString(instr->rs),
			RegNameAsString(instr->rt),
			instr->imm);
		break;

	// J-type instructions with format "opname address"
	case J:
		fprintf(file, " %lld", instr->imm);
		break;
	}
}

void PrintLineNr(const Instr* instr, FILE* file)
{
	if (instr->line_nr == DONT_CARE)
		fprintf(file, "generated");
	else
		fprintf(file, "line %d", instr->line_nr);
}

void PrintInstruction_LineNr(const Instr* instr, FILE* file)
{
	PrintInstruction(instr, file);

	fprintf(file, " (");
	PrintLineNr(instr, file);
	fprintf(file, ")");
}
