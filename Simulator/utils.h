/** @file utils.h
	Utilities */

#ifndef UTILS_H_INCL
#define UTILS_H_INCL

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>

/** Macro to terminate the application and return the exit status
	@a RET_CODE to the OS. Mainly useful when using the Visual C++ IDE
	to prevent the console window from closing immediately. */
#if defined(_MSC_VER) && defined(_WIN32)
#	define EXIT_APPL(RET_CODE) \
		system("pause"); \
		exit(RET_CODE);
#else
#	define EXIT_APPL(RET_CODE) \
		exit(RET_CODE);
#endif

/** Create a bit mask of type long long with 1s in bit positions @a start_bit
	to, but not including, @a end_bit.
	Example: CreateLLMask(3, 8) = ...000000011111000 */
long long CreateLLMask(int start_bit, int end_bit);

/** Test whether @a value can be represented using @a precision bits,
	either as signed or unsigned */
int ValueFitsInPrecision(long long value, int precision);

#endif // ifndef UTILS_H_INCL
