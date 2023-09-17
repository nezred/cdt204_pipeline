#pragma warning(disable : 6271)
#pragma warning(disable : 26453)

#include "update.h"
#include "utils.h"
#include "error_handling.h"
#include <assert.h>

#define ENABLE_TASK1_SOLUTION
#define ENABLE_TASK2_SOLUTION
#define ENABLE_TASK3_SOLUTION

 // Functions for computing the next states of the pipeline stages -------------------------

 // Based on the current state of the pipeline, compute a new state for the IF
 // stage to use in the next clock cycle. next_if is an output parameter for the computed state.
static void ComputeNextIFStage(IFStage* next_if)
{
	// If there is a jump instruction in the ID stage select the jump target as the next PC
	if(id_stage.instr.type == J)
	{
		next_if->pc = id_stage.instr.imm;

	}
	// If there is a branch instruction in the Mem stage, and the branch condition
	// is true, select the branch target as the next PC
	else if ((mem_stage.instr.type == BEQ && mem_stage.zero)
		|| (mem_stage.instr.type == BNE && !mem_stage.zero))
	{
		next_if->pc = mem_stage.branch_target;
	}
	// Otherwise just select the succeeding instruction to fetch
	else
		next_if->pc = if_stage.pc + 4;

	// Fetch the instruction from memory, although in a real CPU this would be done
	// in the next clock cycle. This is just to be able to show the instruction
	// in the trace output.
	next_if->instr = ReadFromInstrMemory(next_if->pc);
}

// Based on the current state of the pipeline, compute a new state for the ID
// stage to use in the next clock cycle. next_id is an output parameter for the computed state.
static void ComputeNextIDStage(IDStage* next_id)
{
	if(id_stage.instr.type == J) 
	{
		// we cancel the instruction in the next_id, because it is the one just after the jump
		ClearIDStage(next_id);
	}
	// Update the ID stage with the instruction fetched in the IF stage
	else 
		next_id->instr = ReadFromInstrMemory(if_stage.pc);
}

// Based on the current state of the pipeline, compute a new state for the Ex 
// stage to use in the next clock cycle. next_ex is an output parameter for the computed state.
static void ComputeNextExStage(ExStage* next_ex)
{
	// Since the j instruction has not been implemented yet (this is your job),
	// just terminate the program if one appears



	// Read the right registers
	next_ex->instr = id_stage.instr;
	if (id_stage.instr.rs != DONT_CARE)
		next_ex->rs_value = ReadFromRegisterFile(id_stage.instr.rs);
	else
		next_ex->rs_value = DONT_CARE;
	if (id_stage.instr.rt != DONT_CARE)
		next_ex->rt_value = ReadFromRegisterFile(id_stage.instr.rt);
	else
		next_ex->rt_value = DONT_CARE;
	next_ex->imm_value = id_stage.instr.imm;
}

// Based on the current state of the pipeline, compute a new state for the Mem
// stage to use in the next clock cycle. next_mem is an output parameter for the computed state.
static void ComputeNextMemStage(MemStage* next_mem)
{
	// A mask used to convert signed values to unsigned
	long long mask = CreateLLMask(0, 32);

	// Set some default values
	ClearMemStage(next_mem);
	next_mem->instr = ex_stage.instr;
	next_mem->rt_value = ex_stage.rt_value;

	// Note that since we store the complete target address for jumps and
	// branches, no address computation needs to be done in this stage (this
	// is what is done by the extra ALU described in the course book)

	// Select an operation for the ALU
	switch (ex_stage.instr.type)
	{
	case NOP:
	case SYSCALL:
	case J:
		break;
	case ADD:
		next_mem->alu_result = ex_stage.rs_value + ex_stage.rt_value;
		break;
	case ADDI:
	case LW:
	case SW:
	case LH:
	case LHU:
	case SH:
	case LB:
	case LBU:
	case SB:
		next_mem->alu_result = ex_stage.rs_value + ex_stage.imm_value;
		break;
	case SUB:
		next_mem->alu_result = ex_stage.rs_value - ex_stage.rt_value;
		break;
	case AND:
		next_mem->alu_result = ex_stage.rs_value & ex_stage.rt_value;
		break;
	case ANDI:
		next_mem->alu_result = ex_stage.rs_value & ex_stage.imm_value;
		break;
	case OR:
		next_mem->alu_result = ex_stage.rs_value | ex_stage.rt_value;
		break;
	case ORI:
		next_mem->alu_result = ex_stage.rs_value | ex_stage.instr.imm;
		break;
	case NOR:
		next_mem->alu_result = ~(ex_stage.rs_value | ex_stage.rt_value);
		break;
	case SLL:
	{
		long long sign_bit_mask = 1LL << 31;

		next_mem->alu_result = ex_stage.rs_value << ex_stage.imm_value;

		// Remove the shifted-out bits, while trying to preserve the sign in the result.
		// This means that if the left operand was negative, and the result from the shift
		// also has its sign bit set, we copy the sign into the result
		if (ex_stage.rs_value < 0 && (next_mem->alu_result & sign_bit_mask) != 0)
			next_mem->alu_result |= ~mask;
		else
			next_mem->alu_result &= mask;
		break;
	}
	case SRL:
		if (ex_stage.imm_value != 0)
			next_mem->alu_result = (ex_stage.rs_value & mask) >> ex_stage.imm_value;
		break;
	case BEQ:
	case BNE:
		next_mem->alu_result = ex_stage.rs_value - ex_stage.rt_value;
		next_mem->branch_target = ex_stage.imm_value;
		break;
	case SLT:
		next_mem->alu_result = ex_stage.rs_value < ex_stage.rt_value;
		break;
	case SLTI:
		next_mem->alu_result = ex_stage.rs_value < ex_stage.imm_value;
		break;
	case SLTU:
		next_mem->alu_result = (ex_stage.rs_value & mask) < (ex_stage.rt_value & mask);
		break;
	case SLTIU:
		next_mem->alu_result = (ex_stage.rs_value & mask) < (ex_stage.imm_value & mask);
		break;
	case LUI:
		next_mem->alu_result = ex_stage.imm_value << 16;
		break;
	default:
		STATIC_MIPS_ERROR("ComputeNextMemStage(): unhandled instruction in current EX stage from program line %d: %s", ex_stage.instr.line_nr, StringRepOfInstrType(ex_stage.instr.type));
	}

	// Do some error checking. In a real CPU, arithmetic underflow or overflow would cause an exception,
	// but we just terminate the simulator with an error message.
	if (next_mem->alu_result < MIN_32_BIT_REGVALUE)
		MIPS_RUNTIME_ERROR("Arithmetic underflow: %lld (value does not fit in 32 bits)", next_mem->alu_result);
	if (next_mem->alu_result >= (1LL << 32))
		MIPS_RUNTIME_ERROR("Arithmetic overflow: %lld (value does not fit in 32 bits)", next_mem->alu_result);

	next_mem->zero = next_mem->alu_result == 0 ? 1 : 0;

}

// Based on the current state of the pipeline, compute a new state for the WB 
// stage to use in the next clock cycle. next_wb is an output parameter for the computed register state.
static void ComputeNextWBStage(WBStage* next_wb)
{
	// Set some default values
	ClearWBStage(next_wb);
	next_wb->instr = mem_stage.instr;

	switch (mem_stage.instr.type)
	{
	case LW:
		next_wb->result = ReadFromDataMemory(mem_stage.alu_result, 4, 0);
		break;
	case SW:
		WriteToDataMemory(mem_stage.alu_result, mem_stage.rt_value, 4);
		break;
	case LH:
		next_wb->result = ReadFromDataMemory(mem_stage.alu_result, 2, 0);
		break;
	case LHU:
		next_wb->result = ReadFromDataMemory(mem_stage.alu_result, 2, 1);
		break;
	case SH:
		WriteToDataMemory(mem_stage.alu_result, mem_stage.rt_value, 2);
		break;
	case LB:
		next_wb->result = ReadFromDataMemory(mem_stage.alu_result, 1, 0);
		break;
	case LBU:
		next_wb->result = ReadFromDataMemory(mem_stage.alu_result, 1, 1);
		break;
	case SB:
		WriteToDataMemory(mem_stage.alu_result, mem_stage.rt_value, 1);
		break;
	default:
		next_wb->result = mem_stage.alu_result;
	}
}

static void HandleHazards(IFStage* next_if, IDStage* next_id, ExStage* next_ex,
	MemStage* next_mem, WBStage* next_wb)
{
	int rs, rt;
	int data_hazard = 0;
	
	// If there is taken branch in the mem stage, the instruction fetched in the
	// IF stage (which is from the non-taken branch) should be zeroed out
	if ((mem_stage.instr.type == BEQ && mem_stage.zero) ||
		(mem_stage.instr.type == BNE && !mem_stage.zero))
	{
		ClearIDStage(next_id);
		ClearExStage(next_ex);
		ClearMemStage(next_mem);
	}

	// Get the register(s) read by the instruction in ID
	GetRegsReadByInstr(&id_stage.instr, &rs, &rt);

	// Check if the instruction in Mem has any of the registers read by the
	// instruction in ID as destination register
	if (rs != DONT_CARE && GetRegWrittenByInstr(&mem_stage.instr) == rs)
		next_ex->rs_value = next_wb->result;
	else if (rt != DONT_CARE && GetRegWrittenByInstr(&mem_stage.instr) == rt)
		next_ex->rt_value = next_wb->result;

	// Check if the instruction in Ex has any of the registers read by the
	// instruction in ID as destination register
	if ((rs != DONT_CARE && GetRegWrittenByInstr(&ex_stage.instr) == rs) ||
		(rt != DONT_CARE && GetRegWrittenByInstr(&ex_stage.instr) == rt))
	{

		if(GetInstrCategory(ex_stage.instr.type)==LOAD)
		{
			// Reset the future IF and ID stages to the same states as in the current clock cycle,
			// and make the future Ex stage a bubble
			*next_if = if_stage;
			*next_id = id_stage;
			ClearExStage(next_ex);
		}
		else 
		{
			if(rs != DONT_CARE && GetRegWrittenByInstr(&ex_stage.instr) == rs) 
				next_ex->rs_value = next_mem->alu_result;
			else 
				next_ex->rt_value = next_mem->alu_result;
		}
	}
}

static void UpdateRegisterFile()
{
	int written_reg = GetRegWrittenByInstr(&wb_stage.instr);
	if (written_reg != DONT_CARE)
	{
		WriteToRegisterFile(written_reg, wb_stage.result);
	}
}

// Exported functions ----------------------------------------------------------

void UpdatePipelineState()
{
	// The next states of all state elements
	IFStage next_if;
	IDStage next_id;
	ExStage next_ex;
	MemStage next_mem;
	WBStage next_wb;

	// Start by updating the register file, since this is done
	// in the first half of the clock cycle (in particular, before
	// reading the registers in the ID stage)
	UpdateRegisterFile();

	// Compute the next state elements for the pipeline
	ComputeNextIFStage(&next_if);
	ComputeNextIDStage(&next_id);
	ComputeNextExStage(&next_ex);
	ComputeNextMemStage(&next_mem);
	ComputeNextWBStage(&next_wb);

	HandleHazards(&next_if, &next_id, &next_ex, &next_mem, &next_wb);

	// Update the pipeline's state
	if_stage = next_if;
	id_stage = next_id;
	ex_stage = next_ex;
	mem_stage = next_mem;
	wb_stage = next_wb;
} 
