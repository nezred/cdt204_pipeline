#include "register_file.h"
#include "error_handling.h"

// The value to initialize the $gp register with
static const long long GP_INIT = 0x10008000LL;

// The value to initialize the $sp register with
static const long long SP_INIT = 0x7FFFFFFCLL;

// The actual contents of the register file
static long long contents[32] = { 0 };

static void ValidateRegisterNr(int register_nr)
{
	if (register_nr < 0 || 31 < register_nr)
		MIPS_RUNTIME_ERROR("%d is not a valid register number", register_nr);
}

static void ValidateWrittenValue(long long value)
{

	if (value < MIN_32_BIT_REGVALUE || (1LL << 32) <= value)
		MIPS_RUNTIME_ERROR("The value %lld does not fit in a 32-bit register", value);
}

// Public functions -----------------------------------------------------

void InitRegisterFile()
{
	contents[GP] = GP_INIT;
	contents[SP] = SP_INIT;
}

void WriteToRegisterFile(int register_nr, long long value)
{
	ValidateRegisterNr(register_nr);
	ValidateWrittenValue(value);
	if (register_nr != 0)
		contents[register_nr] = value;
}

long long ReadFromRegisterFile(int register_nr)
{
	ValidateRegisterNr(register_nr);
	if (register_nr == 0)
		return 0;
	return contents[register_nr];
}

int IsValidRegisterNr(int register_nr)
{
	return 0 <= register_nr && register_nr < 32;
}

const char* RegNameAsString(int register_nr)
{
	static const char* reg_names[32] =
	{
		"zero",	// ZERO
		"at",	// AT
		"v0",	// V0
		"v1",	// V1
		"a0",	// A0
		"a1",	// A1
		"a2",	// A2
		"a3",	// A3
		"t0",	// T0
		"t1",	// T1
		"t2",	// T2
		"t3",	// T3
		"t4",	// T4
		"t5",	// T5
		"t6",	// T6
		"t7",	// T7
		"s0",	// S0
		"s1",	// S1
		"s2",	// S2
		"s3",	// S3
		"s4",	// S4
		"s5",	// S5
		"s6",	// S6
		"s7",	// S7
		"t8",	// T8
		"t9",	// T9
		"k0",	// K0
		"k1",	// K1
		"gp",	// GP
		"sp",	// SP
		"fp",	// FP
		"ra"	// RA
	};

	if (register_nr < 0 || 32 <= register_nr) {
		FAILED_ASSERTION("RegNameAsString(): register_nr %d must be in the range [0, 31]", register_nr)
	}
	return reg_names[register_nr];
}

void PrintRegisterFile(FILE* file)
{
	int l;
	for (l = 0; l < 8; ++l)
	{
		int c;
		for (c = 0; c < 4; ++c)
		{
			int r = l + 8*c;
			char* after_r_pad, * before_eq_pad;
			if (r == 8 || r == 9)
				after_r_pad = " ";
			else
				after_r_pad = "";

			if (c == 0 && l != 0)
				before_eq_pad = "  ";
			else
				before_eq_pad = "";

			fprintf(file, "R%d%s (%s) %s= %-11lld ", r, after_r_pad, RegNameAsString(r), before_eq_pad, contents[r]);
		}
		fprintf(file, "\n");
	}
}