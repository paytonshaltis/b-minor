bminor: main.o scanner.o parser.o
	gcc main.o scanner.o parser.o -o bminor

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

clean:
	rm -f scanner.c scanner.o parser.c parser.o parser.output token.h main.o bminor
