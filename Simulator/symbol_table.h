/** @file symbol_table.h
	A table mapping symbols (labels) to code and data addresses */

#ifndef SYMBOL_TABLE_H_INCL
#define SYMBOL_TABLE_H_INCL

/** Register @a symbol as naming the address @a address
	@pre @a symbol is not already present in the table */
void AddToSymbolTable(const char* symbol, long long address);

/** Check whether @a symbol exists in the symbol table
	@return 1 if it does, 0 otherwise */
int SymbolTableHasSymbol(const char* symbol);

/** Get the address named by the label @a symbol
	@pre @a symbol has previously been added with AddToSymbolTable() */
long long LookupInSymbolTable(const char* symbol);

#endif // ifndef SYMBOL_TABLE_H_INCL
