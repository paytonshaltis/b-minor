# CSC-425 Compilers Project: Scanner

## UPDATE: Scanner Progress Check #1
All 30 teacher source files work, as well as 5 of my own. Feel free to try some out, and browse the code. Comments should help understand what each code region is responsible for. Any feedback for improvements would be appreciated!

The first part of the bminor compiler needed is the scanner. Through the `-scan` command line option, a `.bminor` source file is scanned for tokens. These tokens are written into `stdout` for the user to check and see which tokens the scanner recognizes. An unrecognized token will return an error and exit with exit status 1.
