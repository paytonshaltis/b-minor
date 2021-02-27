bminor: main.o scanner.o
	gcc main.o scanner.o -o bminor

main.o: main.c token.h
	gcc main.c -c -o main.o

scanner.o: scanner.c token.h
	gcc scanner.c -c -o scanner.o

scanner.c: scanner.flex
	flex -oscanner.c scanner.flex

clean:
	rm -f scanner.c scanner.o main.o bminor
