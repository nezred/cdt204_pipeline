%option nounistd
%option noyywrap
%option bison-locations
%option outfile="mips_lexer.c"
%option never-interactive
%option prefix="mips_lexer_"

%{

#include "instructions.h"
#include "register_file.h"
#include "mips_parser_tokens.h"

#define YY_USER_ACTION UpdateLocation(yylloc_param);

#define YYLTYPE MIPS_PARSER_LTYPE
#define YYSTYPE MIPS_PARSER_STYPE

#define RETURN_INSTRUCTION(INSTR_TYPE, TOKEN) \
	yylval->instr_type = INSTR_TYPE; \
	return TOKEN;

#define RETURN_REGISTER(REG_NR) \
	yylval->integer = REG_NR; \
	return REGISTER;

static void UpdateLocation(YYLTYPE* location)
{
	// First make the previous last_line and _column the new first_ dittos
	location->first_line = location->last_line;
	location->first_column = location->last_column;
	
	// Update the new first_line and _column
	{
		const char* start, * end;
		for (start = yytext, end = strchr(yytext, '\n'); end != 0; 
			start = end + 1, end = strchr(start, '\n'))
		{
			++location->last_line;
			location->last_column = 1;
		}
		location->last_column += yytext + yyleng - start;
	}
}

%}

%x READ_EOF_ONCE

%%

[[:blank:]] { /* Throw away spaces and tabs */ }

#.*$ { /* Comment */ }

\.text { return TEXT_DIR; }
\.data { return DATA_DIR; }
\.globl { return GLOBL_DIR; }
\.ent { return ENT_DIR; }
\.end { return END_DIR; }

\.byte { return BYTE_DIR; }
\.half { return HALF_DIR; }
\.word { return WORD_DIR; }

nop { RETURN_INSTRUCTION(NOP, EMPTY_INSTR) }

add { RETURN_INSTRUCTION(ADD, RRR_INSTR) }
sub { RETURN_INSTRUCTION(SUB, RRR_INSTR) }
addi { RETURN_INSTRUCTION(ADDI, RRI_INSTR) }

lw { RETURN_INSTRUCTION(LW, RIR_INSTR) }
sw { RETURN_INSTRUCTION(SW, RIR_INSTR) }
lh { RETURN_INSTRUCTION(LH, RIR_INSTR) }
lhu { RETURN_INSTRUCTION(LHU, RIR_INSTR) }
sh { RETURN_INSTRUCTION(SH, RIR_INSTR) }
lb { RETURN_INSTRUCTION(LB, RIR_INSTR) }
lbu { RETURN_INSTRUCTION(LBU, RIR_INSTR) }
sb { RETURN_INSTRUCTION(SB, RIR_INSTR) }
lui { RETURN_INSTRUCTION(LUI, RI_INSTR) }

and { RETURN_INSTRUCTION(AND, RRR_INSTR) }
or { RETURN_INSTRUCTION(OR, RRR_INSTR) }
nor { RETURN_INSTRUCTION(NOR, RRR_INSTR) }
andi { RETURN_INSTRUCTION(ANDI, RRI_INSTR) }
ori { RETURN_INSTRUCTION(ORI, RRI_INSTR) }
sll { RETURN_INSTRUCTION(SLL, RRI_INSTR) }
srl { RETURN_INSTRUCTION(SRL, RRI_INSTR) }

beq { RETURN_INSTRUCTION(BEQ, RRI_INSTR) }
bne { RETURN_INSTRUCTION(BNE, RRI_INSTR) }
slt { RETURN_INSTRUCTION(SLT, RRR_INSTR) }
sltu { RETURN_INSTRUCTION(SLTU, RRR_INSTR) }
slti { RETURN_INSTRUCTION(SLTI, RRI_INSTR) }
sltiu { RETURN_INSTRUCTION(SLTIU, RRI_INSTR) }

j { RETURN_INSTRUCTION(J, I_INSTR) }

syscall { RETURN_INSTRUCTION(SYSCALL, EMPTY_INSTR) }

move { RETURN_INSTRUCTION(MOV, RR_INSTR) }
li { RETURN_INSTRUCTION(LI, RI_INSTR) }
incr { RETURN_INSTRUCTION(INCR, RI_INSTR) }
decr { RETURN_INSTRUCTION(DECR, RI_INSTR) }

[[:alpha:]_][[:alnum:]_]* {
	strncpy(yylval->string, yytext, yyleng);
	yylval->string[yyleng] = '\0';
	return LABEL;
}

\$[[:digit:]]{1,2} { sscanf(yytext, "$%d", &yylval->integer); return REGISTER; }
\$r[[:digit:]]{1,2} { sscanf(yytext, "$r%d", &yylval->integer); return REGISTER; }

\$zero { RETURN_REGISTER(ZERO) }
\$at { RETURN_REGISTER(AT) }
\$v0 { RETURN_REGISTER(V0) }
\$v1 { RETURN_REGISTER(V1) }
\$a0 { RETURN_REGISTER(A0) }
\$a1 { RETURN_REGISTER(A1) }
\$a2 { RETURN_REGISTER(A2) }
\$a3 { RETURN_REGISTER(A3) }
\$t0 { RETURN_REGISTER(T0) }
\$t1 { RETURN_REGISTER(T1) }
\$t2 { RETURN_REGISTER(T2) }
\$t3 { RETURN_REGISTER(T3) }
\$t4 { RETURN_REGISTER(T4) }
\$t5 { RETURN_REGISTER(T5) }
\$t6 { RETURN_REGISTER(T6) }
\$t7 { RETURN_REGISTER(T7) }
\$t8 { RETURN_REGISTER(T8) }
\$t9 { RETURN_REGISTER(T9) }
\$s0 { RETURN_REGISTER(S0) }
\$s1 { RETURN_REGISTER(S1) }
\$s2 { RETURN_REGISTER(S2) }
\$s3 { RETURN_REGISTER(S3) }
\$s4 { RETURN_REGISTER(S4) }
\$s5 { RETURN_REGISTER(S5) }
\$s6 { RETURN_REGISTER(S6) }
\$s7 { RETURN_REGISTER(S7) }
\$k0 { RETURN_REGISTER(K0) }
\$k1 { RETURN_REGISTER(K1) }
\$gp { RETURN_REGISTER(GP) }
\$sp { RETURN_REGISTER(SP) }
\$fp { RETURN_REGISTER(FP) }
\$ra { RETURN_REGISTER(RA) }

-?[[:digit:]]+ { sscanf(yytext, "%d", &yylval->integer); return INTEGER; }

-0x[[:digit:]]+ { sscanf(yytext, "-0x%x", &yylval->integer); yylval->integer *= -1; return INTEGER; }

0x[[:digit:]]+ { sscanf(yytext, "0x%x", &yylval->integer); return INTEGER; }

\n { return NEWLINE; }

. { return *yytext; }

<INITIAL><<EOF>> { BEGIN(READ_EOF_ONCE); return NEWLINE; }
<READ_EOF_ONCE><<EOF>> { BEGIN(INITIAL); yyterminate(); }

%%