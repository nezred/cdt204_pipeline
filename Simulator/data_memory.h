/** @file data_memory.h
	The data memory. For simplicity we treat the data and instruction memories as
	distinct entities. */

#ifndef DATA_MEMORY_H_INCL
#define DATA_MEMORY_H_INCL

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

/** Initialize the data memory. This should be done at program startup, before any
	other functions in data_memory.h are called. */
void InitDataMemory();

/** Get the starting address (the lowest address) of the data segment of
	the process's memory space */
long long GetDataSegmentStartingAddress();

/** Read a value from data memory
	@param address The memory address to read from
	@param nr_bytes The number of bytes to read
	@param read_unsigned Whether or not to interpret the read value as unsigned
	@return The read value */
long long ReadFromDataMemory(long long address, int nr_bytes, int read_unsigned);

/** Write a value to the data memory
	@param address Which byte address to start writing to
	@param value The value to write
	@param nr_bytes How much of the value to write */
void WriteToDataMemory(long long address, long long value, int nr_bytes);

/** Print a textual representation of the data memory to the file @a file */
void PrintDataMemory(FILE* file);

#endif // ifndef DATA_MEMORY_H_INCL
