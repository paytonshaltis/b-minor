/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_ARRAY = 258,
     TOKEN_BOOLEAN = 259,
     TOKEN_CHAR = 260,
     TOKEN_ELSE = 261,
     TOKEN_FALSE = 262,
     TOKEN_FOR = 263,
     TOKEN_FUNCTION = 264,
     TOKEN_IF = 265,
     TOKEN_INTEGER = 266,
     TOKEN_PRINT = 267,
     TOKEN_RETURN = 268,
     TOKEN_STRING = 269,
     TOKEN_TRUE = 270,
     TOKEN_VOID = 271,
     TOKEN_COLON = 272,
     TOKEN_SEMICOLON = 273,
     TOKEN_COMMA = 274,
     TOKEN_LBRACKET = 275,
     TOKEN_RBRACKET = 276,
     TOKEN_LCURLY = 277,
     TOKEN_RCURLY = 278,
     TOKEN_LPAREN = 279,
     TOKEN_RPAREN = 280,
     TOKEN_ASSIGN = 281,
     TOKEN_CARET = 282,
     TOKEN_PLUS = 283,
     TOKEN_MINUS = 284,
     TOKEN_INCREMENT = 285,
     TOKEN_DECREMENT = 286,
     TOKEN_MULTIPLY = 287,
     TOKEN_DIVIDE = 288,
     TOKEN_MOD = 289,
     TOKEN_EQUAL = 290,
     TOKEN_GE = 291,
     TOKEN_LE = 292,
     TOKEN_LESS = 293,
     TOKEN_GREATER = 294,
     TOKEN_NEQUAL = 295,
     TOKEN_IDENT = 296,
     TOKEN_INTLIT = 297,
     TOKEN_CHARLIT = 298,
     TOKEN_STRINGLIT = 299,
     TOKEN_OR = 300,
     TOKEN_AND = 301,
     TOKEN_NOT = 302,
     TOKEN_ERROR = 303
   };
#endif
/* Tokens.  */
#define TOKEN_ARRAY 258
#define TOKEN_BOOLEAN 259
#define TOKEN_CHAR 260
#define TOKEN_ELSE 261
#define TOKEN_FALSE 262
#define TOKEN_FOR 263
#define TOKEN_FUNCTION 264
#define TOKEN_IF 265
#define TOKEN_INTEGER 266
#define TOKEN_PRINT 267
#define TOKEN_RETURN 268
#define TOKEN_STRING 269
#define TOKEN_TRUE 270
#define TOKEN_VOID 271
#define TOKEN_COLON 272
#define TOKEN_SEMICOLON 273
#define TOKEN_COMMA 274
#define TOKEN_LBRACKET 275
#define TOKEN_RBRACKET 276
#define TOKEN_LCURLY 277
#define TOKEN_RCURLY 278
#define TOKEN_LPAREN 279
#define TOKEN_RPAREN 280
#define TOKEN_ASSIGN 281
#define TOKEN_CARET 282
#define TOKEN_PLUS 283
#define TOKEN_MINUS 284
#define TOKEN_INCREMENT 285
#define TOKEN_DECREMENT 286
#define TOKEN_MULTIPLY 287
#define TOKEN_DIVIDE 288
#define TOKEN_MOD 289
#define TOKEN_EQUAL 290
#define TOKEN_GE 291
#define TOKEN_LE 292
#define TOKEN_LESS 293
#define TOKEN_GREATER 294
#define TOKEN_NEQUAL 295
#define TOKEN_IDENT 296
#define TOKEN_INTLIT 297
#define TOKEN_CHARLIT 298
#define TOKEN_STRINGLIT 299
#define TOKEN_OR 300
#define TOKEN_AND 301
#define TOKEN_NOT 302
#define TOKEN_ERROR 303




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

