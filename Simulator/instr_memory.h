/** @file instr_memory.h
	The instruction memory. For simplicity we treat the data and instruction memories as
	distinct entities. */

#ifndef INSTR_MEMORY_H_INCL
#define INSTR_MEMORY_H_INCL

#include "instructions.h"
#include <stdio.h>

/** Initialize the instruction memory. This should be called at program startup before
	any other functions from instr_memory.h are called. */
void InitInstrMemory(FILE*);

/** Get the starting address of the text segment in the program's memory space */
long long GetTextSegmentStartingAddress();

/** Read an instruction from the absolute address @a address.
	@pre The address is word-aligned */
Instr ReadFromInstrMemory(long long address);

/** Write the instruction @a instr to the address @a address in the instruction memory
	@pre The address is word-aligned */
void WriteToInstrMemory(long long address, const Instr* instr);

#endif // ifndef INSTR_MEMORY_H_INCL
