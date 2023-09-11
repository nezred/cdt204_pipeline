/** @file register_file.h
	The register file */

#ifndef REGISTER_FILE_H_INCL
#define REGISTER_FILE_H_INCL

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
static const long long MIN_32_BIT_REGVALUE = -2147483648LL;

/** Symbolic names for the different registers */
typedef enum RegName
{
	ZERO = 0,
	AT = 1,
	V0 = 2,
	V1 = 3,
	A0 = 4,
	A1 = 5,
	A2 = 6,
	A3 = 7,
	T0 = 8,
	T1 = 9,
	T2 = 10,
	T3 = 11,
	T4 = 12,
	T5 = 13,
	T6 = 14,
	T7 = 15,
	S0 = 16,
	S1 = 17,
	S2 = 18,
	S3 = 19,
	S4 = 20,
	S5 = 21,
	S6 = 22,
	S7 = 23,
	T8 = 24,
	T9 = 25,
	K0 = 26,
	K1 = 27,
	GP = 28,
	SP = 29,
	FP = 30,
	RA = 31
}
RegName;

/** Initialize the register file and set default values for some registers.
	Should be called on program startup. */
void InitRegisterFile();

/** Write a @a value to the register @a register_nr
	@pre @a register_nr is a value in the range [0, 31] */
void WriteToRegisterFile(int register_nr, long long value);

/** Read the value in register nr @a register_nr */
long long ReadFromRegisterFile(int register_nr);

/** Check if @a register_nr is a valid register number
	@return 1 if it is, 0 otherwise */
int IsValidRegisterNr(int register_nr);

/** Return a string representation of the register given by @a register_nr
	@pre @a register_nr is a value in the range [0, 31] */
const char* RegNameAsString(int register_nr);

/** Print a textual representation of the contents of the register file to @a file */
void PrintRegisterFile(FILE* file);

#endif // ifndef REGISTER_FILE_H_INCL
