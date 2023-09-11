/** @file instructions.h 
	MIPS instructions */

#ifndef INSTRUCTIONS_H_INCL
#define INSTRUCTIONS_H_INCL

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

/** The different category of instructions */
typedef enum
{
	ARITHMETIC_LOGICAL,
	LOAD,
	STORE,
	PROGRAM_CONTROL,
	OTHER
}
InstrCategory;

/** The different types of instructions */
typedef enum
{
	NOP,

	//Pseudo
	MOV,
	LI,
	INCR,
	DECR,
	// Arithmetic
	ADD,
	SUB,
	ADDI,

	// Data transfer (load)
	LW,
	LH,
	LHU,
	LB,
	LBU,
	LUI,

	// Data transfer (store)
	SW,
	SH,
	SB,

	// Logical
	AND,
	OR,
	NOR,
	ANDI,
	ORI,
	SLL,
	SRL,

	// Conditional jumps
	BEQ,
	BNE,
	SLT,
	SLTU,
	SLTI,
	SLTIU,

	// Unconditional jumps
	J,

	// System call. Only used to terminate the simulated program.
	SYSCALL
}
InstrType;

/** A MIPS instruction */
typedef struct Instr
{
	/** The instruction's type */
	InstrType type;

	/** The instruction's line number in the assembly code. Is set to DONT_CARE if
		the instruction is generated, i.e., does not come from an assembly listing. */
	int line_nr;

	/** The rd register number. Is set to DONT_CARE for instructions that do not specify
		this register operand. */
	int rd;

	/** The rs register number. Is set to DONT_CARE for instructions that do not specify
		this register operand. */
	int rs;

	/** The rt register number. Is set to DONT_CARE for instructions that do not specify
		this register operand. */
	int rt;

	/** The immediate field. Is set to DONT_CARE for instructions that do not specify
		an immediate operand.
		@note This might for some instruction types hold values larger (or smaller) than
			what fits in 16 bits, although this would not be possible in a real MIPS CPU, where
			all instructions must fit in 32 bits */
	long long imm;
}
Instr;

/** A symbolic value to use for instruction fields when
	their actual value is unimportant */
#define DONT_CARE (-1)

/** Create an instruction that uses the rd, rs, and rt fields of the Instr struct (apart from type and line_nr).
	The rest of the fields are set to DONT_CARE. */
Instr CreateInstr_RRR(InstrType type, int rd, int rs, int rt, int line_nr);

/** Create an instruction that uses the rd, and rs fields of the Instr struct (apart from type and line_nr).
	The rest of the fields are set to DONT_CARE. */
Instr CreateInstr_RR(InstrType type, int rd, int rs, int line_nr);

/** Create an instruction that uses the rs, rt, and imm fields of the Instr struct (apart from type and line_nr).
	The rest of the fields are set to DONT_CARE. */
Instr CreateInstr_RRI(InstrType type, int rs, int rt, long long imm, int line_nr);

/** Create an instruction that uses the rt and imm fields of the Instr struct (apart from type and line_nr).
	The rest of the fields are set to DONT_CARE. */
Instr CreateInstr_RI(InstrType type, int rt, long long imm, int line_nr);

/** Create an instruction that uses the imm field of the Instr struct (apart from type and line_nr).
	The rest of the fields are set to DONT_CARE. */
Instr CreateInstr_I(InstrType type, long long imm, int line_nr);

/** Create an instruction that don't have any operands */
Instr CreateInstr_Empty(InstrType type, int line_nr);

/** Return the category to which an istruction type belongs to */
InstrCategory GetInstrCategory(InstrType type);

/** Compare the instructions @a instr1 and @a instr2
	@return 1 if they are exactly equal (same type and same operands), 0 otherwise */
int CompareInstrs(const Instr* instr1, const Instr* instr2);

/** Get the register numbers that the instruction @a instr reads,
	which may be one, two, or no registers
	@param instr The instruction
	@param rs Used to return the number of the rs register, in case
		@a instr has an rs register that it reads
	@param rt Used to return the number of the rt register, in case
		@a instr has an rt register that it reads */
void GetRegsReadByInstr(const Instr* instr, int* rs, int* rt);

/** Get the register number written to by @a instr, if any
	@return The register number, if @a instr updates a register, DONT_CARE otherwise */
int GetRegWrittenByInstr(const Instr* instr);

/** Get a string representation of the instruction type @a type
	@note The returned pointer should @e not be deleted by the caller */
const char* StringRepOfInstrType(InstrType type);

/** Print a textual representation of @a instr to @a file similar to how
	the instruction would appear in an assembly listing */
void PrintInstruction(const Instr* instr, FILE* file);

/** Print the line number of @a instr in the assembly listing to @a file.
	For generated instructions, "generated", is printed instead. */
void PrintLineNr(const Instr* instr, FILE* file);

/** Does the same as @ref PrintInstruction, but also prints the line number
	of the instruction in the assembly listing to @a file. If the instruction
	is generated, "generated" is printed instead of a line number. */
void PrintInstruction_LineNr(const Instr* instr, FILE* file);

#endif // ifndef INSTRUCTIONS_H_INCL
