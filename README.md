# CSC-425 Compilers Project: Scanner

## FINAL: Scanner Project Submission
All 30 instructor test cases and all 20 student test cases work as expected, and test a wide variety of what I believe to be the more important edge-cases and scanning tests. `token.h` contains an enumeration of type `token_t`. `main.c` contains two functions `main()` and `modifyText()`; the latter used to modify char and string literals that are scanned so that quotes are removed and escape characters are translated. `scanner.flex` contains the regular expressions that I have put together in order to recognize the tokens within a `.bminor` source file. The scanner is used by executing: `./bminor -scan <sourcefile>`. Each token is printed to `stdout` with the value of the literal or identifier (if applicable). Unrecognized tokens cause the scanning process to end prematurely, and exit the program with code 1.

## Scanner Progress Check #2
All 20 student source files have been created, tested, and work as expected. I duplicated the same shell script from the `tests/instructor/scanner` directory into the `tests/student/scanner` directory for easy testing. I believe that my 20 tests highlight the more important test cases for the scanner, including strings, chars, escape sequences, and comments. I recommend checking some of them out for detailed comments regarding what each file is testing for.

## Scanner Progress Check #1
All 30 teacher source files work, as well as 5 of my own. Feel free to try some out, and browse the code. Comments should help understand what each code region is responsible for. Any feedback for improvements would be appreciated!
