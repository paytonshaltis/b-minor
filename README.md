<div align="center">

# 🅱 B-Minor Programming Language

### [B-minor](https://www3.nd.edu/~dthain/courses/cse40243/fall2020/bminor.html) is a C-like, strictly-typed, imperative programming language. It is most notably used in undergraduate computer science compiler courses as a semester-long project. 

This repository contains my own _personal_ implementation of the language's specifications, completed as partial requirement of TCNJ's CSC-425: Compilers and Interpreters course in the Spring 2021 semester. The project was an individual, semester-long guide through our rigorous study of how programming language specifications are implemented as actual, executable compilers. This implementation targets AArch64 or ARM64 as its target architecture, meaning that any computer from this family of architectures can actually execute the generated machine instructions that result from this compiler!

</div>

## Table of Contents

1. [Implemented Features](#implemented-features)
2. [Tools Used](#tools-used)
3. [Compilation and Usage](#compilation-and-usage)
4. [Files in this Repository](#files-in-this-repository)
5. [Implementation Details](#implementation-details)
   - [Lexical Analysis](#step-1-lexical-analysis)
   - [Syntax Analysis](#step-2-syntax-analysis)
   - [Pretty Printing](#step-3-pretty-printing)
   - [Name Resolution and Typechecking](#step-4-name-resolution-and-typechecking)
   - [Code Generation](#step-5-code-generation)
6. [Example Output](#example-output)

## Implemented Features

Suprisingly, B-Minor features a large number of common programming language constructs. While more complex features such as classes, objects, and multi-dimensional arrays are not supported, I was proud of the long list of features that are implemented and working as of the completion of the project. These include:

- Strict typing system:
  - Four primitive data types: `integer`, `boolean`, `char`, and `string`
  - Multiple supported escape sequences (`\n`, `\t`, etc.) for `char` and `string`
- Memory abstraction via variables:
  - Global and local primitive variable declaration and assignments
  - Global declaration and local access of single-dimensional arrays
- Static scoping:
  - Accessing both local and non-local variables within a function
  - Nested scope, such as control statements within a function
  - Variable shadowing in the case of nested scopes
- Expressions:
  - Arithmetic expressions, including exponentiation, unary negation, and modulus
  - Conditional expressions through the use of comparison operators
  - Logical expressions with `&&`, `||`, and `!`
  - Assignment expressions for variable value assignment
- Flow of program control:
  - Multiple-selection statements in the form of `if` and `else` statements
  - Iteration statements in the form of `for` and `while` statements
- Support for subprograms:
  - Global declarations of functions and function prototypes
  - Function calls with up to 6 arguments
  - Returning a value from a function
- Simple program output:
  - Printing single and multiple expressions in a single line
- Compatibility with the standard C library
- Single and multi-line comments
- Stack-dynamic local variables, allowing for recursion

## Tools Used

Though the language's implementation was written mostly from scratch, there were a number of tools and concepts used in order to make the project manageable for a single-semester course. These tools are listed below:

- **C** - The compiler was written entirely in the C programming language. Its low-level nature was a perfect fit for the project, and ability to create more advanced data structures using pointers was essential for features such as symbol tables and abstract syntax trees.
- **Regular Expressions** - Regular expressions were used to describe the patterns of tokens and lexemes in the language. They greatly simplify the process of detecting valid and invalid programs based on their most basic units.
- **[Flex](https://github.com/westes/flex)** - A scanner generator for C. This tool was largely used to create the lexical analyzer for the compiler. Regular expressions were used to descripe the different tokens and lexemes of the language, and Flex could then generate the code for the lexical analyzer.
- **[Bison](https://www.gnu.org/software/bison/)** - A parser generator for C. This tool was used to create the syntax analyzer for the compiler. The language was described using a context-free grammar in [BNF](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form) so that Bison could then generate the code for the parser.
- **[Make and Makefile](https://www.gnu.org/software/make/manual/make.html)** - A build automation tool for C. This tool was used to manage compiling program components in the correct order due to a number of dependency concerns between modules of the program.
- **Git and GitHub** - The VCS and hosting platform used to manage the project's source code.

## Compilation and Usage

In order to compile this B-Minor compiler, you will need to ensure that you have Make, Flex, and Bison installed on your machine.

1. Compile the compiler by running `make` in the root directory of the project. This will generate the executable `bminor` in the root directory.
2. Write your B-Minor source code that you would like to use. This source code must use the `.bminor` file extension.
3. Run the compiler on your source code with the following command. The output file must have the `.s` file extension. The step can be any of the named steps mentioned in the pipeline (`scan`, `parse`, etc.), but to generate the final machine code, you must use the `codegen` step:

```
./bminor -<step> <source_file>.bminor <output_name>.s
```

4. On a machine with an AArch64 or ARM64 processor, you can compile the generated assembly code into an executable binary with the following command. Note that the `library.c` file is required for the `print` function to work properly:

```
gcc -g <source_file>.s library.c -o <output_name>
```

5. Finally, you can run your executable binary as you would expect:

```
./<output_name>
```

## Files in this Repository

In order to keep the project more organized, each logical component of the compiler would be kept in its own pair of header and implementation files. The following is a list of the files in this repository and a brief description of their purpose. Note that files with an asterisk (\*) are the result of some intermediate compilation step, and are not present in the repository:

- General Project Files:
  - `main.c` - This file serves as the entry point for the compiler. It contains the main functions for scanning, parsing, pretty printing, type checking, and generating machine code for a B-Minor program.
  - `library.c` - This file contains the implementation of the B-Minor standard library, which is used to provide the compiler with the necessary functions for printing to the console.
  - `Makefile` - This file contains the build instructions for the compiler. It is used by the Make tool to compile the compiler.
- Lexical Analysis:
  - `scanner.flex` - This file contains the regular expressions used to describe the tokens and lexemes of the language. It is used by Flex to generate the lexical analyzer for the compiler.
  - \*`scanner.c` - This file contains the implementation of the lexical analyzer for the compiler. It was generated by Flex from the `scanner.flex` file.
- Syntax Analysis:
  - `parser.bison` - This file contains the context-free grammar used to describe the syntax of the language. It is used by Bison to generate the parser for the compiler.
  - \*`parser.c` - This file contains the implementation of the parser for the compiler. It was generated by Bison from the `parser.bison` file.
  - \*`token.h` - This file contains the token definitions for the language. It was generated by Bison from the `parser.bison` file.
- Abstract Syntax Tree Node Types:
  - `decl.h` and `decl.c` - These files contain the data structures and functions used to represent, print, typecheck, and generate machine code for declarations in the language.
  - `expr.h` and `expr.c` - These files contain the data structures and functions used to represent, print, typecheck, and generate machine code for expressions in the language.
  - `param_list.h` and `param_list.c` - These files contain the data structures and functions used to represent, print, typecheck, and generate machine code for parameter lists in the language.
  - `stmt.h` and `stmt.c` - These files contain the data structures and functions used to represent, print, typecheck, and generate machine code for statements in the language.
  - `type.h` and `type.c` - These files contain the data structures and functions used to represent, print, and typecheck types in the language.
- Scope and Name Resolution:
  - `hash_table.h` and `hash_table.c` - These files contain the data structures and functions used to represent hash tables in the scope stack for typechecking and name resolution. They were provided by the course instructor.
  - `scope.h` and `scope.c` - These files contain utility functions for entering, exiting, and managing scopes during the typechecking and name resolution phases of the compiler.
  - `symbol.h` and `symbol.c` - These files contain the data structures and functions used to represent symbols in the symbol table for typechecking and name resolution.
- Machine Code Generation:
  - `scratch.h` and `scratch.c` - These files contain utility functions for determining which registers could be used for variable storage during the code generation phase of the compiler.
  - `label.h` and `label.c` - These files contain utility functions for generating unique labels during the code generation phase of the compiler.
  - \*`bminor.exe` - This executable file is the result of compiling the compiler. It is used to compile B-Minor programs into machine code.
- Testing:
  - `run-tests.sh` - A shell script used to run the test suite for the compiler. It compiles the compiler, runs it on each test file, and compares the output to the expected output.
  - `tests/` - A directory containing the test suite for the compiler. Half of the tests were written by the course instructor and the other half were written by me.

## Implementation Details

The compilation process of a B-Minor program (and most other programming languages) is like a pipeline, where the input is the source code and the output is the machine code. The pipeline for B-Minor is broken down into **five** main stages: the lexical analyzer, the syntax analyzer, the 'pretty printer', the type checker / name resolver, and the code generator. The following sections will describe each stage in more detail.

### Step 1. Lexical Analysis

A B-Minor program is nothing more than a sequence _lexemes_, or the smallest meaningful units of a language. `char`, `45`, `myVar` and `for` are all valid lexemes, as per the rules of the language. These rules are specified in the `scanner.flex` file, which is used by Flex to generate the lexical analyzer for the program.

As per the specification of a Flex input file, `scanner.flex` contains a mapping of regular expressions to their associated tokens in the language. For example, take a look at the following lines from this file:

```flex
DIGIT  [0-9]
LETTER [a-zA-Z]
({LETTER}|"_")({LETTER}|"_"|{DIGIT}){0,255}     { return TOKEN_IDENT; }
```

The first two lines are constants used to simplify the rest of the regular expressions and reduce redundancy. As you can see, a `LETTER` is any character that satisfies the regular expression `[a-zA-Z]`.

The third line is a Flex rule used to match a valid _identifier_ in B-Minor. According to the B-Minor language specification, an identifier is a sequence of characters that begins with a letter or underscore and is followed by no more than 255 letters, underscores, or digits. The curly braces `{}` to the right of the regex are used to specify the action to be taken when a match is found. In this case, the action is to return the token `TOKEN_IDENT` to the parser.

The `scanner.flex` file contains rules for the simplest language constructs like keywords...

```flex
else            { return TOKEN_ELSE; }
false           { return TOKEN_FALSE; }
for             { return TOKEN_FOR; }
```

...as well as more complex tokens such as string and char literals...

```flex
'((\\?[^\\\'\n])|(\\\\)|(\\\'))'                { return TOKEN_CHARLIT; }
\"((\\.|[^\\"\n]){0,255})\"                     { return TOKEN_STRINGLIT; }
```

### Step 2. Syntax Analysis

After a program has been scanned by the lexical analyzer, and no invalid lexemes have been found in the source program, the syntax analyzer is used to parse the program into a tree-like structure called an _abstract syntax tree_ (AST). The input for the parser is the output of the scanner: a stream of tokens. You can see how the pipeline analogy is starting to make sense now!

The parser ensures that the syntax of the input program is valid: for example, this is not a valid B-Minor expression:

```b-minor
x: integer + 'e' == foo(1, --);
```

While all of the individual lexemes of this statement are valid (by the way, they are `x`, `:`, `integer`, `+`, `'e'`, `==`, `foo`, `(` `1`, `,`, `--`, `)`, and `;`!), they are not in an order that makes any sense for the language. The parser will catch this error and report it to the user.

The syntax rules for the language are defined in the Bison input file, named `parser.bison`. Just like the scanner, it contains a number of BNF-like rules that contain _terminal_ and _non-terminal_ symbols. The terminal symbols are the tokens that are output by the scanner, and the non-terminal symbols are the rules that describe the structure of the language. For example, take a look at the syntax rule for an `paramslist`, or _parameter list_, in B-Minor:

```bison
paramslist  : ident TOKEN_COLON type TOKEN_COMMA paramslist
            | ident TOKEN_COLON type
            | ident TOKEN_COLON nosizearr TOKEN_COMMA paramslist
            | ident TOKEN_COLON nosizearr
            ;
```

Additionally, each rule, when matched, has an associated action. Here are the actions for each of the rules above:

```bison
{$$ = param_list_create($1, $3, $5);}
{$$ = param_list_create($1, $3, 0);}
{$$ = param_list_create($1, $3, $5);}
{$$ = param_list_create($1, $3, 0);}
```

There's a lot here, but let's break it down:

- The symbol to the left of the colon `:` is the name of the _non-terminal_ symbol, or the rule that is being defined. In this case, we are defining the syntax for a `paramslist`.
- Everything to the right of the colon `:` is a _production_, or a rule that describes how the non-terminal symbol can be constructed. These can be made up of _terminal_ and/or _non-terminal_ symbols. Each production is separated by a pipe `|` character, telling us that there are multiple ways to construct a the non-terminal. In this case, there happens to be 4 different ways to construct a `paramslist`.
- Each production has an associated _action_ inside of curly braces `{}`. This action is executed when the parser encounters this production. Each action in the parser is generally the creation of a new node in the _abstract syntax tree_, or _parse tree_, which is the output of this step in the pipeline.
- The rules are sometimes defined _recursively_, as is made obvious by the fact that the `paramslist` non-terminal is used in the definition of itself. This is a common pattern in the parser, and is used to describe the structure of the language. A parameter list has an arbitrary number of parameters, so the best way to define a list of parameters is as a parameter followed by a comma and another parameter list, or just a parameter by itself.

Each major node type for the AST was given its own set of header and implementation files. These included:

- `decl.h` and `decl.c` for declarations
- `expr.h` and `expr.c` for expressions
- `param_list.h` and `param_list.c` for parameter lists
- `stmt.h` and `stmt.c` for statements
- `type.h` and `type.c` for types

Within these files, functions for creating AST nodes were defined. In fact, they are called in the Bison specification file, `parser.bison`, to create the nodes in the parse tree. Take another look at the production rules for `paramslist`, and you'll notice that the action associated with each matched rule is actually a call to a function called `param_list_create()`.

This step in the compilation process is quite complex, and requires complete knowledge of the language's specifications. You can see how even a simple language such as B-Minor has a number of production rules that must be defined in the parser. The parser is also responsible for operator associativity and precedence, defined in part by the order that production rules are applied to make up the `expr` node in the abstract syntax tree.

### Step 3. Pretty Printing

Though not required for a real compiler, we were tasked with the development of a 'pretty printer'. This component in the pipeline really served as a test of the parser's output: the [AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree). The pretty printer would traverse the nodes of the abstract syntax tree and print each node to the console in a human-readable format, which included proper indentation for nested language constructs and scopes.

The pretty printer could also be seen as an optimizer, as it removed some redundancy in the AST before typechecking and code generation would occur. For example, expressions with redundant or simply unneccesary groupings for precedence, such as `(1 * 2) + 3`, would be printed as `1 + 2 * 3`. This is a simple example, but the pretty printer would also remove redundant parentheses in more complex expressions, such as `(((((((1 + 2) * 3)))) + 4))`. This served to reduce the number of nodes in the AST, as well as ensure that the AST was being traversed in the proper order to take care of the language's associativity and precedence rules.

In order to implement this, each of the AST node files (`decl.c`, `expr.c`, etc.) contains a print function that is called by the pretty printer. The AST is traversed in a depth-first manner, and each visited node is printed to the console. The pretty printer also keeps track of the current indentation level, which is used to print the correct number of spaces before each line of output. By traversing the AST in the same manner that the code generator eventually will, this component of the pipeline was _also_ used to ensure that the parser was outputting the correct intermediate representation of the language.

### Step 4. Name Resolution and Typechecking

Now that the AST was known to have been traversed correctly, it could be passed to this step in the compilation pipeline. Issues including scope, type, and name resolution are all handled in this step. As the AST was being traversed, sometimes a new scope would be found. This could occur in a number of instances, such as nested loops, selection statements within functions, any plenty of other common language constructs. In order to handle name resolution to either local or non-local variables, a series of _symbol tables_ within a _stack_ were used. All of the code for scope resolution and name resolution was contained within the `scope.c` and `scope.h` files. A hash table implementation was provided by the course staff, which can be found in the `hash_table.c` and `hash_table.h` files.

Each time a new scope was found, a new symbol table was pushed onto the stack. This symbol table would contain all of the variables that were declared within the scope. When a variable was declared, it would be added to the symbol table. When a variable was accessed, the symbol table stack would be searched from top to bottom, and the first instance of the variable would be returned. This ensured that the correct variable was accessed, and that the variable was declared in the correct scope. Non-local variables were also taken care of: not finding a variable in the current scope would cause the symbol table stack to be searched for the variable in the next scope up, and so on.

When a scope was exited, the symbol table was popped off of the stack. This ensured that the symbol table stack would always contain the correct symbol tables for the current scope. This was also used to ensure that the correct variable was accessed, as the symbol table stack would be searched from top to bottom, and the first instance of the variable would be returned.

Declarations, expressions, and statements all required the proper typechecking, implemented through the inclusion of an appropriate `typecheck()` function, such as `decl_typecheck()`. For example, any declaration required the type to the right of the assignment operator to match the type to the left of the assignment operator. This was done by checking the type of the expression on the right side of the assignment operator, and comparing it to the type of the variable on the left side of the assignment operator. If the types did not match, an error was thrown.

Take the following declaration as an example:

```b-minor
x: integer = (true && b);
```

The following steps would occur in order to ensure that the proper typeing rules were enforced:

1. The type of `x`, indicated by the declaration as `integer`, is compared against the type of `(true && b)`, which cannot be determined without typechecking this _sub-expression_.
2. The type of `(true && b)` should be `boolean`, as indicated by the operator (`&&`) being used. However, the type checker needs to confirm that both operands of the `&&` operator are of type `boolean`. This is done by typechecking the left and right operands of the `&&` operator, which are `true` and `b`, respectively.
3. `true` is easy to typecheck, but `b` must be searched for in the symbol table, through the process of name resolution described in the scoping section above. If we suppose that `b` is of type `boolean`, then the type of `(true && b)` is `boolean`.
4. With the types of `x` and `(true && b)` now both known, they are compared, and because they are not the same, an error is thrown.

This same process must be repeated for every expression, statement, and declaration in the abstract syntax tree. If at any point the types do not match, it means that the language specifications have been violated, as per the strict typing system of B-Minor as a language.

### Step 5. Code Generation

Finally, the abstract syntax tree could be traversed for a final time in order to produce the final output of the compiler: the assembly code. This was done by traversing the AST in a depth-first manner, and calling the appropriate code generation function for each node. The code generation functions, much like the AST node creation, printing, and typechecking functions, were contained within files such as `decl.c`, `expr.c`, etc.

Because the target architecture was ARM64 or AArch64, the code generated was in the form of ARM64 assembly code. Each node of the AST was called in the same depth-first manner as the typechecker, and the appropriate code generation function was called. This function would determine what machine instructions should be written to the output file, and would also call the code generation functions for any sub-expressions, statements, or declarations that were required.

`scratch.c`, `scratch.h`, `label.c`, and `label.h` were a few additional utility files that were written in order to keep track of register allocation and proper assembly labels to use.

In the end, after the machine code was generated, it could be assembled, linked, and executed on the appropriate architecture.

## Example Output

Below is a sample B-Minor program that features a number of complex features of the language. Following it is the output of the compiler, which contains the raw assembly code that was generated.

### _scores.bminor_

```b-minor
// simple program testing arrays; calculates some basic statistics for a global array
// of positive integer test scores, including average, maximum, and minimum scores


// global array of test scores
scores : array [10] integer = {97, 87, 83, 99, 77, 80, 72, 100, 92, 88};

main : function integer (argc : integer, argv : array [] string) = {

	// variables used for calculating test statistics
	i : integer = 0;
	total : integer = 0;
	avg : integer = 0;
	max : integer = scores[0];
	min : integer = scores[0];

	// looks through each of the test scores
	for(i = 0; i < 10; i++) {

		// totals up all of the test scores
		total = total + scores[i];

		// updates max if applicable
		if(scores[i] > max) {
			max = scores[i];
		}

		// updates min if applicable
		if(scores[i] < min) {
			min = scores[i];
		}
	}

	// compute average using total from above
	avg = total / 10;

	// print the statistics
	print "The average test score is ", avg, "\n";
	print "The maximum test score is ", max, "\n";
	print "The minimum test score is ", min, "\n";

	return 0;
}
```

### _scores.s_ (Result of `./bminor scores.bminor library.c scores.s`)

```s
#	ARMv8-a assembly code generated by
#	'B-Minor Compiler' v1.0,
#	written by:
#
#	PAYTON JAMES SHALTIS
#	COMPLETED MAY 4TH, 2021 for
#
#	CSC-425: "Compilers and Interpreters",
#	Professor John DeGood, Spring 2021 at
#	The College of New Jersey

.arch armv8-a
.file "output.s"

.data
	.global scores
	.align 3
scores:
	.word	97
	.word	87
	.word	83
	.word	99
	.word	77
	.word	80
	.word	72
	.word	100
	.word	92
	.word	88
.text
	.global main
	main:
		stp	x29, x30, [sp, #-120]!
		str	x0, [sp, 16]
		str	x1, [sp, 24]
		mov	x9, 0
		str	x9, [sp, 32]
		mov	x9, 0
		str	x9, [sp, 40]
		mov	x9, 0
		str	x9, [sp, 48]
		adrp	x9, scores
		add	x9, x9, :lo12:scores
		mov	x10, 0
		mov	x11, 4
		mul	x10, x10, x11
		ldr	w0, [x9, x10]
		mov	x9, x0
		str	x9, [sp, 56]
		adrp	x9, scores
		add	x9, x9, :lo12:scores
		mov	x10, 0
		mov	x11, 4
		mul	x10, x10, x11
		ldr	w0, [x9, x10]
		mov	x9, x0
		str	x9, [sp, 64]
		mov	x9, 0
		str	x9, [sp, 32]
	.LSTMT1:
		ldr	x9, [sp, 32]
		mov	x10, 10
		cmp	x9, x10
		b.lt	.LCOND1
		mov	x10, 0
		b	.LCOND2
	.LCOND1:
		mov	x10, 1
	.LCOND2:
		cmp	x10, 0
		b.eq	.LSTMT2
		ldr	x9, [sp, 40]
		adrp	x10, scores
		add	x10, x10, :lo12:scores
		ldr	x11, [sp, 32]
		mov	x12, 4
		mul	x11, x11, x12
		ldr	w0, [x10, x11]
		mov	x10, x0
		add	x10, x9, x10
		str	x10, [sp, 40]
		adrp	x9, scores
		add	x9, x9, :lo12:scores
		ldr	x10, [sp, 32]
		mov	x11, 4
		mul	x10, x10, x11
		ldr	w0, [x9, x10]
		mov	x9, x0
		ldr	x10, [sp, 56]
		cmp	x9, x10
		b.gt	.LCOND3
		mov	x10, 0
		b	.LCOND4
	.LCOND3:
		mov	x10, 1
	.LCOND4:
		cmp	x10, 0
		b.eq	.LSTMT3
		adrp	x9, scores
		add	x9, x9, :lo12:scores
		ldr	x11, [sp, 32]
		mov	x12, 4
		mul	x11, x11, x12
		ldr	w0, [x9, x11]
		mov	x9, x0
		str	x9, [sp, 56]
	.LSTMT3:
		adrp	x9, scores
		add	x9, x9, :lo12:scores
		ldr	x10, [sp, 32]
		mov	x11, 4
		mul	x10, x10, x11
		ldr	w0, [x9, x10]
		mov	x9, x0
		ldr	x10, [sp, 64]
		cmp	x9, x10
		b.lt	.LCOND5
		mov	x10, 0
		b	.LCOND6
	.LCOND5:
		mov	x10, 1
	.LCOND6:
		cmp	x10, 0
		b.eq	.LSTMT4
		adrp	x9, scores
		add	x9, x9, :lo12:scores
		ldr	x11, [sp, 32]
		mov	x12, 4
		mul	x11, x11, x12
		ldr	w0, [x9, x11]
		mov	x9, x0
		str	x9, [sp, 64]
	.LSTMT4:
		ldr	x9, [sp, 32]
		add	x9, x9, 1
		str	x9, [sp, 32]
		b	.LSTMT1
	.LSTMT2:
		ldr	x9, [sp, 40]
		mov	x10, 10
		sdiv	x10, x9, x10
		str	x10, [sp, 48]
		adrp	x9, .LSLIT1
		add	x9, x9, :lo12:.LSLIT1
		mov	x0, x9
		bl	print_string
		ldr	x9, [sp, 48]
		mov	x0, x9
		bl	print_integer
		adrp	x9, .LSLIT2
		add	x9, x9, :lo12:.LSLIT2
		mov	x0, x9
		bl	print_string
		adrp	x9, .LSLIT3
		add	x9, x9, :lo12:.LSLIT3
		mov	x0, x9
		bl	print_string
		ldr	x9, [sp, 56]
		mov	x0, x9
		bl	print_integer
		adrp	x9, .LSLIT4
		add	x9, x9, :lo12:.LSLIT4
		mov	x0, x9
		bl	print_string
		adrp	x9, .LSLIT5
		add	x9, x9, :lo12:.LSLIT5
		mov	x0, x9
		bl	print_string
		ldr	x9, [sp, 64]
		mov	x0, x9
		bl	print_integer
		adrp	x9, .LSLIT6
		add	x9, x9, :lo12:.LSLIT6
		mov	x0, x9
		bl	print_string
		mov	x9, 0
		mov	x0, x9
		ldp	x29, x30, [sp], #120
		ret
		ldp	x29, x30, [sp], #120
		ret
	.section	.rodata
	.align 8
.LSLIT1:
	.string "The average test score is "
	.section	.rodata
	.align 8
.LSLIT2:
	.string "\n"
	.section	.rodata
	.align 8
.LSLIT3:
	.string "The maximum test score is "
	.section	.rodata
	.align 8
.LSLIT4:
	.string "\n"
	.section	.rodata
	.align 8
.LSLIT5:
	.string "The minimum test score is "
	.section	.rodata
	.align 8
.LSLIT6:
	.string "\n"
```
