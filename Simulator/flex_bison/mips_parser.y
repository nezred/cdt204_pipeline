%define api.pure
%locations
%define parse.error verbose
%header "mips_parser_tokens.h"
%define api.prefix {mips_parser_}

%{

#include "instructions.h"
#include "data_memory.h"
#include "instr_memory.h"
#include "symbol_table.h"
#include "mips_parser.h"
#include "mips_parser_tokens.h"
#include "error_handling.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef enum Segment { TEXT, DATA } Segment;

// Used to keep track of the current segment. Is changed when encountering .text and .data directives.
static Segment cur_segment;

// Temporary buffer used to store the parsed instructions. Is returned to the caller upon exit.
static ParsedInstr* instr_buf;
static int nr_instr, buf_size;

static long long cur_data_addr, cur_instr_addr;

static void AddParsedInstrToBack(const Instr* instr, const char* label)
{
	assert(cur_segment == TEXT);

	if (nr_instr == buf_size)
	{
		buf_size *= 2;
		instr_buf = (ParsedInstr*)realloc(instr_buf, buf_size * sizeof(ParsedInstr));
	}

	if (label != NULL)
		instr_buf[nr_instr].label = strdup(label);
	else
		instr_buf[nr_instr].label = NULL;
	instr_buf[nr_instr].instr = *instr;
	++nr_instr;
	cur_instr_addr += 4;
}

static void AddParsedDataToBack(long long data, int size)
{
	WriteToDataMemory(cur_data_addr, data, size);
	cur_data_addr += size;
}

static int mips_parser_lex(YYSTYPE* semantic_value, YYLTYPE* location)
{
	extern int mips_lexer_lex(YYSTYPE* semantic_value, YYLTYPE* location);
	return mips_lexer_lex(semantic_value, location);
} 

static void mips_parser_error(YYLTYPE* location, FILE* unused1, ParsedInstr** unused2, int* unused3, const char* msg)
{
	extern char* mips_lexer_text;
	extern int mips_lexer_leng;
	
	// Extract the lexeme of the current token
	char* text = (char*)malloc(mips_lexer_leng * sizeof(char) + sizeof('\0'));
	text[mips_lexer_leng] = '\0';
	strncpy(text, mips_lexer_text, mips_lexer_leng);

	fprintf(stderr, "At %d:%d-%d:%d, \"%s\": %s",
		location->first_line, location->first_column,
		location->last_line, location->last_column, text, msg);

	// Add a period if the error message happens to not be terminated by a punctuation character
	if (ispunct(msg[strlen(msg) - 1]))
		fprintf(stderr, "\n");
	else
		fprintf(stderr, ".\n");
	free(text);
}

static void mips_parser_error2(YYLTYPE* location, const char* msg)
{
	fprintf(stderr, "At %d:%d-%d:%d: %s",
		location->first_line, location->first_column,
		location->last_line, location->last_column, msg);
	
	// Add a period if the error message happens to not be terminated by a punctuation character
	if (ispunct(msg[strlen(msg) -1]))
		fprintf(stderr, "\n");
	else
		fprintf(stderr, ".\n");
}

%}

%union
{
	InstrType instr_type;
	int integer;
	char string[100];
	Instr instr;
}

%token NEWLINE
%token TEXT_DIR DATA_DIR GLOBL_DIR ENT_DIR END_DIR
%token BYTE_DIR HALF_DIR WORD_DIR
%token <instr_type> RRR_INSTR RR_INSTR RRI_INSTR RIR_INSTR RI_INSTR I_INSTR EMPTY_INSTR
%token <integer> REGISTER INTEGER
%token <string> LABEL

%parse-param { FILE* mips_file }
%parse-param { ParsedInstr** instr_list }
%parse-param { int* nr_instr_out }

%initial-action
{
	extern FILE* mips_lexer_in;
	mips_lexer_in = mips_file;

	buf_size = 10;
	instr_buf = (ParsedInstr*) calloc(buf_size, sizeof(ParsedInstr));
	nr_instr = 0;

	cur_segment = TEXT;
	cur_data_addr = GetDataSegmentStartingAddress();
	cur_instr_addr = GetTextSegmentStartingAddress();
}

%%

start : prog
{
	*instr_list = instr_buf;
	*nr_instr_out = nr_instr;
	if (mips_parser_nerrs > 0) 
		YYABORT;
};

prog : /* empty */ | prog line NEWLINE ;

line
	: /* Empty */
	| directive
	| data_line
	| instr_line
	| error { ++mips_parser_nerrs; yyerrok; }
	;

data_line
	: LABEL ':' maybe_newlines { AddToSymbolTable($1, cur_data_addr); } data
	| data
	;

instr_line
	: LABEL ':' maybe_newlines { AddToSymbolTable($1, cur_instr_addr); } instr
	| instr
	;

maybe_newlines : /* Empty */ | maybe_newlines NEWLINE ;

directive
	: TEXT_DIR { cur_segment = TEXT; }
	| DATA_DIR { cur_segment = DATA; }
	| GLOBL_DIR LABEL
	| ENT_DIR LABEL
	| END_DIR LABEL
	;

data
	: BYTE_DIR byte_constants
	| HALF_DIR half_constants
	| WORD_DIR word_constants
	;

byte_constants
	: INTEGER { AddParsedDataToBack($1, 1); }
	| byte_constants INTEGER { AddParsedDataToBack($2, 1); }
	;

half_constants
	: INTEGER { AddParsedDataToBack($1, 2); }
	| half_constants INTEGER { AddParsedDataToBack($2, 2); } 
	;

word_constants
	: INTEGER { AddParsedDataToBack($1, 4); } 
	| word_constants INTEGER { AddParsedDataToBack($2, 4); } 
	;

instr
	: RRR_INSTR REGISTER ',' REGISTER ',' REGISTER
	{
		Instr instr = CreateInstr_RRR($1, $2, $4, $6, @$.first_line);
		AddParsedInstrToBack(&instr, NULL);
	}
	| RR_INSTR REGISTER ',' REGISTER
	{
		Instr instr = CreateInstr_RR($1, $2, $4, @$.first_line);
		AddParsedInstrToBack(&instr, NULL);
	}
	| RRI_INSTR REGISTER ',' REGISTER ',' INTEGER
	{
		Instr instr;
		if ($1 == BEQ || $1 == BNE)
			instr = CreateInstr_RRI($1, $2, $4, $6, @$.first_line);
		else
			instr = CreateInstr_RRI($1, $4, $2, $6, @$.first_line);
		AddParsedInstrToBack(&instr, NULL);
	}
	| RRI_INSTR REGISTER ',' REGISTER ',' LABEL
	{
		Instr instr;
		if (!($1 == BEQ || $1 == BNE))
		{
			mips_parser_error2(&@$, "Only beq and bne may use this operand syntax");
			YYERROR;
		}
		instr = CreateInstr_RRI($1, $2, $4, DONT_CARE, @$.first_line);
		AddParsedInstrToBack(&instr, $6);
	}
	| RIR_INSTR REGISTER ',' INTEGER '(' REGISTER ')' 
	{ 
		Instr instr = CreateInstr_RRI($1, $6, $2, $4, @$.first_line); 
		AddParsedInstrToBack(&instr, NULL);
	}
	| RIR_INSTR REGISTER ',' LABEL
	{
		Instr instr = CreateInstr_RRI($1, DONT_CARE, $2, DONT_CARE, @$.first_line); 
		AddParsedInstrToBack(&instr, $4);	
	}
	| RI_INSTR REGISTER ',' INTEGER
	{
		Instr instr = CreateInstr_RI($1, $2, $4, @$.first_line);
		if ($1 == LUI)
		{
			if (!ValueFitsInPrecision($4, 16))
			{
				mips_parser_error2(&@4, "Immediate operand to lui must fit in 16 bits");
				YYERROR;
			}
		}
		AddParsedInstrToBack(&instr, NULL);	
	}
	| I_INSTR INTEGER
	{
		Instr instr = CreateInstr_I($1, $2, @$.first_line); 
		AddParsedInstrToBack(&instr, NULL);
	}
	| I_INSTR LABEL
	{
		Instr instr = CreateInstr_I($1, DONT_CARE, @$.first_line); 
		AddParsedInstrToBack(&instr, $2);	
	}
	| EMPTY_INSTR
	{
		Instr instr = CreateInstr_Empty($1, @$.first_line);
		AddParsedInstrToBack(&instr, NULL);
	}
	;

%%