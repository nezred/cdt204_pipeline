#include "data_memory.h"
#include "error_handling.h"
#include "utils.h"
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef enum Endianness { LITTLE, BIG } Endianness;

//const Endianness endianness = LITTLE;
const Endianness endianness = BIG;

// The starting address of the static data segment
const long long DATA_SEG_START = 0x10000000LL;

// The end address of (the lowest address not part of) the stack segment
const long long STACK_SEG_END = 0x7FFFFFFFLL;

static uint8_t* data_seg, * stack_seg;
static long long ds_start, ss_end, ss_start;
static long ds_buf_size, ss_buf_size;
static long long ds_alloc[2], ss_alloc[2];

static void ValidateAddress(long long address, int nr_bytes)
{
	if (address < DATA_SEG_START)
		MIPS_RUNTIME_ERROR("The address 0x%llx points below the data segment", address)

	if (address + nr_bytes >= STACK_SEG_END)
		MIPS_RUNTIME_ERROR("The address 0x%llx points above the stack segment", address)

	if (address % nr_bytes != 0)
		MIPS_RUNTIME_ERROR("Address 0x%llx is not %d-byte aligned", address, nr_bytes)
}

static void AdjustBuffers(long long address, int nr_bytes)
{
	long ds_buf_ind, ss_buf_ind;
	long ds_incr, ss_incr;

	// Translate the address to indices into the data_seg and stack_seg buffers
	ds_buf_ind = (long) (address - ds_start);
	ss_buf_ind = (long) (address - ss_start);

	// If a write of nr_bytes fits inside some of the two buffers,
	// they don't need to be enlarged
	if (ds_buf_ind + nr_bytes <= ds_buf_size || ss_buf_ind >= 0)
		return;

	// One of the buffers must be enlarged - compute the amount
	ds_incr = ds_buf_ind + nr_bytes - ds_buf_size;
	ss_incr = -ss_buf_ind;

	// Enlarge the buffer that needs to be enlarged by the least amount
	if (ds_incr <= ss_incr)
	{
		// Allocate a new data_seg buffer that is twice the old size plus
		// the extra elements
		uint8_t* new_data_seg;
		int prev_dsbs = ds_buf_size;
		ds_buf_size = 2*(ds_buf_size + ds_incr);
		new_data_seg = (uint8_t*) calloc(ds_buf_size, sizeof(uint8_t));
		memcpy(new_data_seg, data_seg, prev_dsbs * sizeof(uint8_t));
		free(data_seg);
		data_seg = new_data_seg;
		return;
	}
	else
	{
		// Allocate a new stack_seg buffer that is twice the old size plus
		// the extra elements
		uint8_t* new_stack_seg;
		int prev_ssbs = ss_buf_size;
		ss_buf_size = 2*(ss_buf_size + ss_incr);
		new_stack_seg = (uint8_t*) calloc(ss_buf_size, sizeof(uint8_t));
		memcpy(new_stack_seg + ss_incr, stack_seg, prev_ssbs * sizeof(uint8_t));
		free(stack_seg);
		stack_seg = new_stack_seg;
		ss_start = STACK_SEG_END - ss_buf_size;
		return;
	}
}

static long long ReadFromBuffer(uint8_t* buffer, long buf_ind, int nr_bytes)
{
	long long value = 0;
	int b;
	if (endianness == LITTLE)
		for (b = nr_bytes - 1; b >= 0; --b)
		{
			value <<= 8;
			value |= buffer[buf_ind + b];
		}
	else
		for (b = 0; b < nr_bytes; ++b)
		{
			value <<= 8;
			value |= buffer[buf_ind + b];
		}
	return value;
}

static void WriteToBuffer(uint8_t* buffer, long buf_ind, long long value, int nr_bytes)
{
	long long mask = CreateLLMask(0, 8*nr_bytes);
	long long inv_mask = ~mask;
	long long most_sig_bits = value & inv_mask;
	assert(most_sig_bits == 0ll || most_sig_bits == inv_mask);
	if (endianness == LITTLE)
	{
		int b;
		for (b = 0; b < nr_bytes; ++b)
		{
			buffer[buf_ind + b] = (uint8_t) value & mask;
			value >>= 8;
		}
	}
	else
	{
		int b;
		for (b = nr_bytes - 1; b >= 0; --b)
		{
			buffer[buf_ind + b] = (uint8_t) value & mask;
			value >>= 8;
		}
	}
}

// Public functions - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void InitDataMemory()
{
	ds_buf_size = 10;
	ss_buf_size = 10;

	data_seg = (uint8_t*) calloc(ds_buf_size, sizeof(uint8_t));
	stack_seg = (uint8_t*) calloc(ss_buf_size, sizeof(uint8_t));

	ds_start = DATA_SEG_START;
	ss_start = STACK_SEG_END - ss_buf_size;
}

long long GetDataSegmentStartingAddress()
{
	return DATA_SEG_START;
}

long long ReadFromDataMemory(long long address, int nr_bytes, int read_unsigned)
{
	long long ds_buf_ind, ss_buf_ind, read_value;

	ValidateAddress(address, nr_bytes);
	if (nr_bytes < 0 || nr_bytes > 4)
		FAILED_ASSERTION("Cannot read %d bytes from the data memory", nr_bytes);

	// Translate the address to indices into the data_seg and stack_seg buffers
	ds_buf_ind = address - ds_start;
	ss_buf_ind = address - ss_start;

	// Read from the appropriate buffer
	if (ds_buf_ind + nr_bytes <= ds_buf_size)
		read_value = ReadFromBuffer(data_seg, (long) ds_buf_ind, nr_bytes);
	else
		read_value = ReadFromBuffer(stack_seg, (long) ss_buf_ind, nr_bytes);

	// Sign extend if requested
	if (!read_unsigned)
	{
		// Check if the most significant bit is a one, in which case
		// the sign-extended value should be negative
		long long bit_mask = 1LL << (8*nr_bytes - 1);
		long long masked = read_value & bit_mask;
		if (masked != 0)
		{
			long long ext = CreateLLMask(8*nr_bytes, 8*sizeof(long long));
			return read_value | ext;
		}
	}

	return read_value;
}

void WriteToDataMemory(long long address, long long value, int nr_bytes)
{
	long long ds_buf_ind, ss_buf_ind;

	ValidateAddress(address, nr_bytes);
	if (nr_bytes < 0 || nr_bytes > 4)
		FAILED_ASSERTION("Cannot write %d bytes to the data memory", nr_bytes);

	AdjustBuffers(address, nr_bytes);
	
	ds_buf_ind = address - ds_start;
	ss_buf_ind = address - ss_start;

	if (ds_buf_ind + nr_bytes <= ds_buf_size)
		WriteToBuffer(data_seg, (long) ds_buf_ind, value, nr_bytes);
	else
		WriteToBuffer(stack_seg, (long) ss_buf_ind, value, nr_bytes);
}

void PrintDataMemory(FILE* file)
{
	long long i, a;
	for (i = ss_buf_size - 1, a = ss_start + ss_buf_size - 1;
		i >= 0; --i, --a)
	{
		fprintf(file, "0x%08lx:\t%u\n", (long) a, (unsigned) stack_seg[i]);
	}
	fprintf(file, "...\n");
	for (i = ds_buf_size - 1, a = ds_start + ds_buf_size - 1;
		i >= 0; --i, --a)
	{
		fprintf(file, "0x%08lx:\t%u\n", (long) a, (unsigned) data_seg[i]);
	}
}
