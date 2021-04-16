bminor: main.o scanner.o parser.o decl.o type.o stmt.o expr.o param_list.o hash_table.o symbol.o scope.o
	gcc main.o scanner.o parser.o decl.o type.o stmt.o expr.o param_list.o hash_table.o symbol.o scope.o -o bminor

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

param_list.o: param_list.c param_list.h scope.h
	gcc param_list.c -c -o param_list.o

hash_table.o: hash_table.c hash_table.h
	gcc hash_table.c -c -o hash_table.o

symbol.o: symbol.c symbol.h type.h
	gcc symbol.c -c -o symbol.o

scope.o: scope.c scope.h hash_table.h symbol.h
	gcc scope.c -c -o scope.o

clean:
	rm -f scanner.c scanner.o parser.c parser.o parser.output token.h main.o decl.o type.o stmt.o expr.o param_list.o hash_table.o symbol.o scope.o bminor
