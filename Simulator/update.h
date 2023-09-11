/** @file update.h
	Compute new states for the state elements in the pipeline */

#ifndef UPDATE_H_INCL
#define UPDATE_H_INCL

#include "pipeline.h"

/** Based on the current state of the pipeline, compute a new state for each of the
	five pipeline stages to use in the next clock cycle */
void UpdatePipelineState();

#endif	// ifndef UPDATE_H_INCL
