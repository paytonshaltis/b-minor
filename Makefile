bminor: main.o scanner.o parser.o decl.o type.o stmt.o
	gcc main.o scanner.o parser.o decl.o type.o stmt.o -o bminor

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

decl.o: decl.c decl.h
	gcc decl.c -c -o decl.o

type.o: type.c type.h
	gcc type.c -c -o type.o

stmt.o: stmt.c stmt.h
	gcc stmt.c -c -o stmt.o

clean:
	rm -f scanner.c scanner.o parser.c parser.o parser.output token.h main.o decl.o type.o stmt.o bminor
