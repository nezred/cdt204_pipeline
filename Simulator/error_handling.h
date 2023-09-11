/** @file error_handling.h
	Some macros for handling program errors */

#ifndef ERROR_HANDLING_H_INCL
#define ERROR_HANDLING_H_INCL

#include "utils.h"
#include <stdio.h>

/** If the symbol DEBUG is defined, make all error handling routines
	break into the debugger (provided that a debugger i attached).
	Otherwise, just exit the application with an error code. */
#ifdef DEBUG
	// If compiling under Visual C++, use __debugbreak()
#	ifdef _MSC_VER
#		define BREAK __debugbreak();
	// Otherwise, generate a SIGINT signal (this should work with gdb)
#	else
#		include <signal.h>
#		define BREAK raise(SIGINT);
#	endif
#else
#	include <stdlib.h>
#	define BREAK EXIT_APPL(EXIT_FAILURE);
#endif

/** Macro to handle static errors, i.e., parsing errors, in the MIPS program being run. */
#define STATIC_MIPS_ERROR(FORMAT, ...) \
{ \
	fprintf(stderr, "Static MIPS error: "); \
	fprintf(stderr, FORMAT, ## __VA_ARGS__); \
	fprintf(stderr, ".\n"); \
	BREAK \
}

/** Macro to handle runtime errors in the MIPS program being run. */
#define MIPS_RUNTIME_ERROR(FORMAT, ...) \
{ \
	fprintf(stderr, "MIPS runtime error: "); \
	fprintf(stderr, FORMAT , ## __VA_ARGS__); \
	fprintf(stderr, ".\n"); \
	BREAK \
}

/** Macro to handle failed assertions in the C source code. */
#define FAILED_ASSERTION(FORMAT, ...) \
{ \
	fprintf(stderr, "On line %d in %s: ", __LINE__, __FILE__); \
	fprintf(stderr, FORMAT, ## __VA_ARGS__); \
	fprintf(stderr, "\n."); \
	BREAK \
}

#endif // ifndef ERROR_HANDLING_H_INCL
