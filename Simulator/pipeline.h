/** @file pipeline.h
	The 5-stage pipeline */

#ifndef PIPELINE_H_INCL
#define PIPELINE_H_INCL

#include "instr_memory.h"
#include "data_memory.h"
#include "register_file.h"
#include <stdio.h>

/** State of the IF stage */
typedef struct IFStage
{
	/** The current program counter */
	long long pc;

	/** The instruction being fetched in the IF stage */
	Instr instr;
}
IFStage;

/** State of the ID stage. Corresponds to the IF/ID pipeline register. */
typedef struct IDStage
{
	/** The instruction currently residing in the ID stage */
	Instr instr;	
}
IDStage;

/** State of the Ex stage. Corresponds to the ID/Ex pipeline register.
	@note Not all fields are used by all instructions */
typedef struct ExStage
{
	/** The instruction currently in the Ex stage */
	Instr instr;

	/** The value read from the register given by rs in the ID stage */
	long long rs_value;

	/** The value read from the register given by rt in the ID stage */
	long long rt_value;

	/** The value read from the register given by rt in the ID stage */
	long long imm_value;
}
ExStage;

/** State of the Mem stage. Corresponds to the Ex/Mem pipeline register.
	@note Not all fields are used by all instructions */
typedef struct MemStage
{
	/** The instruction currently in the Mem stage */
	Instr instr;

	/** The value previously read from the rt register in the
		ID stage */
	long long rt_value;

	/** The output from the ALU in the Ex stage */
	long long alu_result;

	/** Holds whether the ALU output is equal to 0 or not */
	int zero;

	/** The branch target computed by the extra ALU in the
		Ex stage */
	long long branch_target;
}
MemStage;

/** State of the WB stage. Corresponds to the Mem/WB pipeline register. */
typedef struct WBStage
{
	/** The instruction currently residing in the WB stage */
	Instr instr;

	/** The result being written back to the register file */
	long long result;
}
WBStage;

// The current pipeline state ----------------------------------------------------------

/** The current state of the IF stage */
extern IFStage if_stage;

/** The current state of the ID stage */
extern IDStage id_stage;

/** The current state of the Ex stage */
extern ExStage ex_stage;

/** The current state of the Mem stage */
extern MemStage mem_stage;

/** The current state of the WB stage */
extern WBStage wb_stage;

// --------------------------------------------------------------------------------------

// Functions for clearing the pipeline registers ----------------------------------------

/** Clear @a to_clear */
void ClearIFStage(IFStage* to_clear);

/** Clear @a to_clear */
void ClearIDStage(IDStage* to_clear);

/** Clear @a to_clear */
void ClearExStage(ExStage* to_clear);

/** Clear @a to_clear */
void ClearMemStage(MemStage* to_clear);

/** Clear @a to_clear */
void ClearWBStage(WBStage* to_clear);

// --------------------------------------------------------------------------------------

/** Initialize all components of the pipeline: the memory and all the pipeline
	stages. This should be called at program startup. */
void InitPipeline(FILE* mips_file);

/** Turn tracing on or off. When turned on, a text file named trace.txt will be
	created that describes that pipeline's status for all clock cycles during
	the run of a program.
	@param on_off Should be != 0 to turn tracing on, and == 0 to turn tracing off */
void SetTracing(int on_off);

/** Run the simulation until the program terminates */
void RunProgram(const char* filename);

/** Return the number of elapsed clock cycles. Can be called after a simulation has
	been completed. */
int GetElapsedCycles();

/** Return the number of retired (i.e., finished) instructions. Can be called after 
	a simulation has been completed. */
int GetNrOfRetiredInstructions();

/** Return the average CPI. Can be called after a simulation has been completed. */
float GetCPI();

#endif // ifndef PIPELINE_H_INCL
