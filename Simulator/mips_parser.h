/** @file mips_parser.h
	Interface to the MIPS assembly code parser */

#ifndef MIPS_PARSER_H_INCL
#define MIPS_PARSER_H_INCL

#include "instructions.h"
#include <stdio.h>

/** A parsed instruction, possibly with a label operand */
typedef struct ParsedInstr
{
	/**	Used only by instructions that have label operands, to be able to link the program after parsing */
	char* label;

	/** The parsed instruction */
	Instr instr;
}
ParsedInstr;

/** Parse the MIPS program in @a mips_file
	@post After the call, @a instr_list will point to a dynamically allocated array that
		holds all the instructions of the program, and @a nr_instr_out will hold the
		instruction count
	@return 0 if the parse went fine, 1 otherwise */
int mips_parser_parse(FILE* mips_file, ParsedInstr** instr_list, int* nr_instr_out);

#endif // ifndef MIPS_PARSER_H_INCL
