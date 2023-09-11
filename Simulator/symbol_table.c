#include "symbol_table.h"
#include <string.h>
#include <assert.h>

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS

#define MAX_ELEMENTS 1000

typedef struct SymbolTableEntry
{
	const char* symbol;
	long long address;
}
SymbolTableEntry;

static const int INVALID_KEY = -1;
static const long long INVALID_ADDRESS = -1;

static SymbolTableEntry elements[MAX_ELEMENTS];
static int next_element = 0;
static int sorted = 0;

static int DoLinearSearch(const char* symbol)
{
	int e;
	for (e = 0; e < next_element; ++e)
		if (strcmp(elements[e].symbol, symbol) == 0)
			return e;
	return INVALID_KEY;
}

static int AddSymbolToArray(const char* symbol)
{
	int key = DoLinearSearch(symbol);
	if (key != INVALID_KEY)
		return key;
	assert(next_element < MAX_ELEMENTS);
	elements[next_element].symbol = strdup(symbol);
	elements[next_element].address = INVALID_ADDRESS;
	return next_element++;
}

// Public interface ----------------------------------------------

void AddToSymbolTable(const char* symbol, long long address)
{
	int key = AddSymbolToArray(symbol);
	assert(elements[key].address == INVALID_ADDRESS);
	elements[key].address = address;
}

int SymbolTableHasSymbol(const char* symbol)
{
	return DoLinearSearch(symbol) != INVALID_KEY;
}

long long LookupInSymbolTable(const char* symbol)
{
	int key = DoLinearSearch(symbol);
	assert(key != INVALID_KEY);
	return elements[key].address;
}
