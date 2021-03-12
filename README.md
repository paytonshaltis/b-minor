# CSC-425 Compilers Project: Parser

## Parser Progress Check #1
All of the production rules that I could think of up until this point have been implemented. This includes balanced if/else and for statements. As of now, all 30 instructor test cases are working with my current implementation. The `main.c` file was updated to accept the `-parse` command line option, and my program still works fine with the `-scan` option. The most interesting file to look at would be `parser.bison`, which holds all of the production rules organized for readability and commented to enhance understanding of their purposes. I still need to write my own test cases, which means testing corner cases that may cause my production rules to change between now and the final submission.
