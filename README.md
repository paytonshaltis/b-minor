# CSC-425 Compilers Project: Scanner

## UPDATE: Scanner Progress Check #2
All 20 student source files have been created, tested, and work as expected. I duplicated the same shell script from the `tests/instructor/scanner` directory into the `tests/student/scanner` directory for easy testing. I believe that my 20 tests highlight the more important test cases for the scanner, including strings, chars, escape sequences, and comments. I recommend checking some of them out for detailed comments regarding what each file is testing for.

## Scanner Progress Check #1
All 30 teacher source files work, as well as 5 of my own. Feel free to try some out, and browse the code. Comments should help understand what each code region is responsible for. Any feedback for improvements would be appreciated!

## Component Description
The first part of the bminor compiler needed is the scanner. Through the `-scan` command line option, a `.bminor` source file is scanned for tokens. These tokens are written into `stdout` for the user to check and see which tokens the scanner recognizes. An unrecognized token will return an error and exit with exit status 1.
