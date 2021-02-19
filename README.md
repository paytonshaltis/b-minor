# CSC-425 Compilers Project: Scanner
The first part of the bminor compiler needed is the scanner. Through the `-scan` command line option, a `.bminor` source file is scanned for tokens. These tokens are written into `stdout` for the user to check and see which tokens the scanner recognizes. An unrecognized token will return an error and exit with exit status 1.
