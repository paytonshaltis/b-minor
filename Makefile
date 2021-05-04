#  	all code in this file is original, and was written by:
#  
#   PAYTON JAMES SHALTIS
#   COMPLETED MAY 4TH, 2021
#
#			for
#
#	B-MINOR COMPILER, v1.0
#
#
#   in CSC-425: "Compilers and Interpreters" taught by Professor John DeGood,
#   over the course of the Spring 2021 semester. I understand that keeping this
#   code in a public repository may allow other students to have access. In the
#   event that the course is taught again, with a similar project component, this 
#   code is NOT to be used in place of another student's work.
#
#
#
#                                   'Makefile'
#                                   ----------
#   This Makefile handles dependencies for compiling elements of the project. Most
#   source-code files are compiled into object files, which are later linked together   
#   to generate the executable file 'bminor'.
#
#

bminor: main.o scanner.o parser.o decl.o type.o stmt.o expr.o param_list.o hash_table.o symbol.o scope.o scratch.o label.o
	gcc main.o scanner.o parser.o decl.o type.o stmt.o expr.o param_list.o hash_table.o symbol.o scope.o scratch.o label.o -o bminor

main.o: main.c token.h parser.c
	gcc main.c -c -o main.o

scanner.o: scanner.c token.h
	gcc scanner.c -c -o scanner.o

scanner.c: scanner.flex
	flex -oscanner.c scanner.flex

parser.o: parser.c token.h
	gcc parser.c -c -o parser.o

parser.c token.h: parser.bison
	bison -v --defines=token.h --output=parser.c parser.bison 

decl.o: decl.c decl.h expr.h type.h scope.h
	gcc decl.c -c -o decl.o

type.o: type.c type.h
	gcc type.c -c -o type.o

stmt.o: stmt.c stmt.h scope.h
	gcc stmt.c -c -o stmt.o

expr.o: expr.c expr.h scope.h symbol.h
	gcc expr.c -c -o expr.o

param_list.o: param_list.c param_list.h scope.h expr.h
	gcc param_list.c -c -o param_list.o

hash_table.o: hash_table.c hash_table.h
	gcc hash_table.c -c -o hash_table.o

symbol.o: symbol.c symbol.h type.h
	gcc symbol.c -c -o symbol.o

scope.o: scope.c scope.h hash_table.h symbol.h
	gcc scope.c -c -o scope.o

scratch.o: scratch.c scratch.h
	gcc scratch.c -c -o scratch.o

label.o: label.c label.h
	gcc label.c -c -o label.o

clean:
	rm -f scanner.c scanner.o parser.c parser.o parser.output token.h main.o decl.o type.o stmt.o expr.o param_list.o hash_table.o symbol.o scope.o scratch.o label.o *.s bminor
