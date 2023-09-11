#include "instr_memory.h"
#include "error_handling.h"
#include "symbol_table.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

static const long long TEXT_SEG_START = 0x00400000LL;

static Instr* instrs;
static long buf_size;

static void ValidateAddress(long long address)
{
	long long buf_ind = (address - TEXT_SEG_START) / 4;

	if (buf_ind < 0)
		MIPS_RUNTIME_ERROR("The address 0x%llx points below the text segment", address)

	if (address % 4 != 0)
		MIPS_RUNTIME_ERROR("Instruction address 0x%llx is not 4-byte aligned", address)

	if (address >= (1LL << 32))
		MIPS_RUNTIME_ERROR("The address 0x%llx points outside the 4 GiB address space", address)
}

// Public stuff --------------------------------------------------------------

void InitInstrMemory(FILE* mips_file)
{
	int i;
	buf_size = 10;
	instrs = (Instr*) calloc(buf_size, sizeof(Instr));
	for (i = 0; i < buf_size; ++i)
		instrs[i] = CreateInstr_Empty(NOP, DONT_CARE);
}

long long GetTextSegmentStartingAddress()
{
	return TEXT_SEG_START;
}

Instr ReadFromInstrMemory(long long address)
{
	long long buf_ind = (address - TEXT_SEG_START) / 4;

	ValidateAddress(address);

	if (buf_ind >= buf_size)
		return CreateInstr_Empty(NOP, DONT_CARE);

	return instrs[buf_ind];
}

void WriteToInstrMemory(long long address, const Instr* instr)
{
	long buf_ind = (long) (address - GetTextSegmentStartingAddress()) / 4;
	
	ValidateAddress(address);

	// Check if the instruction buffer needs to be enlarged
	if (buf_ind >= buf_size)
	{
		long prev_buf_size = buf_size;
		long i;
		buf_size = 2*(buf_ind + 1);
		instrs = (Instr*) realloc(instrs, buf_size * sizeof(Instr));
		for (i = prev_buf_size; i < buf_size; ++i)
			instrs[i] = CreateInstr_Empty(NOP, DONT_CARE);
	}

	instrs[buf_ind] = *instr;
}