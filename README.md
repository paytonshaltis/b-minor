# CSC-425 Compilers Project: Codegen

## UPDATE: Codegen Progress Check #1
The last stage of the compiler project is codegen. Again, the AST must be traversed, spitting out the proper ARMv8 Assembly code. Currently, using the `-codegen` command-line option conducts scanning, parsing, name resolution, and typechecking as usual, as well as converting a subset of the language to ARMv8 Assembly and outputting it to `stdout`. 

Here is a list of what is currently implemented:
  - Global declarations of integers, booleans, chars, and strings
  - Global declarations of functions and function prototypes
  - Local declarations of integers, booleans, chars, and strings
  - Accessing both global and local variables locally within a function
  - Most expression types (see below for those not implemented yet)
  - Printing single and multiple expressions in a single line
  - Returning a value from a function
  - Function calls of sizes 0-6 (properly throws error of more than 6)

Here is a list of what is currently NOT implemented:
  - Global declaration of single-dimensional arrays
  - Accessing global single-dimensional arrays
  - Equals and Not Equals conditional expressions
  - All conditional expressions other than Equals and Not Equals
  - If-statements and for-loops

Needless to say, much testing is still required to make sure that all of the implemented and soon-to-be implemented components work correctly together, but I believe that I should be able to get the rest of these components of the language implemented by next Friday for final submission.
