/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_MIPS_PARSER_MIPS_PARSER_TOKENS_H_INCLUDED
# define YY_MIPS_PARSER_MIPS_PARSER_TOKENS_H_INCLUDED
/* Debug traces.  */
#ifndef MIPS_PARSER_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define MIPS_PARSER_DEBUG 1
#  else
#   define MIPS_PARSER_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define MIPS_PARSER_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined MIPS_PARSER_DEBUG */
#if MIPS_PARSER_DEBUG
extern int mips_parser_debug;
#endif

/* Token kinds.  */
#ifndef MIPS_PARSER_TOKENTYPE
# define MIPS_PARSER_TOKENTYPE
  enum mips_parser_tokentype
  {
    MIPS_PARSER_EMPTY = -2,
    MIPS_PARSER_EOF = 0,           /* "end of file"  */
    MIPS_PARSER_error = 256,       /* error  */
    MIPS_PARSER_UNDEF = 257,       /* "invalid token"  */
    NEWLINE = 258,                 /* NEWLINE  */
    TEXT_DIR = 259,                /* TEXT_DIR  */
    DATA_DIR = 260,                /* DATA_DIR  */
    GLOBL_DIR = 261,               /* GLOBL_DIR  */
    ENT_DIR = 262,                 /* ENT_DIR  */
    END_DIR = 263,                 /* END_DIR  */
    BYTE_DIR = 264,                /* BYTE_DIR  */
    HALF_DIR = 265,                /* HALF_DIR  */
    WORD_DIR = 266,                /* WORD_DIR  */
    RRR_INSTR = 267,               /* RRR_INSTR  */
    RR_INSTR = 268,                /* RR_INSTR  */
    RRI_INSTR = 269,               /* RRI_INSTR  */
    RIR_INSTR = 270,               /* RIR_INSTR  */
    RI_INSTR = 271,                /* RI_INSTR  */
    I_INSTR = 272,                 /* I_INSTR  */
    EMPTY_INSTR = 273,             /* EMPTY_INSTR  */
    REGISTER = 274,                /* REGISTER  */
    INTEGER = 275,                 /* INTEGER  */
    LABEL = 276                    /* LABEL  */
  };
  typedef enum mips_parser_tokentype mips_parser_token_kind_t;
#endif

/* Value type.  */
#if ! defined MIPS_PARSER_STYPE && ! defined MIPS_PARSER_STYPE_IS_DECLARED
union MIPS_PARSER_STYPE
{
#line 103 "mips_parser.y"

	InstrType instr_type;
	int integer;
	char string[100];
	Instr instr;

#line 100 "mips_parser_tokens.h"

};
typedef union MIPS_PARSER_STYPE MIPS_PARSER_STYPE;
# define MIPS_PARSER_STYPE_IS_TRIVIAL 1
# define MIPS_PARSER_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined MIPS_PARSER_LTYPE && ! defined MIPS_PARSER_LTYPE_IS_DECLARED
typedef struct MIPS_PARSER_LTYPE MIPS_PARSER_LTYPE;
struct MIPS_PARSER_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define MIPS_PARSER_LTYPE_IS_DECLARED 1
# define MIPS_PARSER_LTYPE_IS_TRIVIAL 1
#endif






#endif /* !YY_MIPS_PARSER_MIPS_PARSER_TOKENS_H_INCLUDED  */
