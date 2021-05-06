# CSC-425 Compilers Project: Codegen

## FINAL: Codegen Project Submission
The B-Minor compiler is complete. After a long semester of work, I can proudly say that I have successfully written a compiler for the B-Minor language to ARMv8-a Assembly.

After the progress made by Friday of last week, there was still much to be done. Many of the things I implemented were either not completely correct or not finished, and a few bugs came up when writing my own student test cases. Luckily, I was able to identify these issues and make the changes needed. If-statements and for-loops required a new label scheme that was not too difficult to code. Global integer arrays were more challenging, since they required me to understand how arrays were stored in the ARMv8-a architecture. Equals and not equals were trivial for integers, booleans, and characters, but strings required a more complex approach of generating assembly code for a loop that would compare character by character. The same algorithm was used as the foundation for string assignment and declaration of strings with variable values, so after I did one the other was trivial. Conditional expressions required the labeling scheme that loops and if-statements did too, so I decided to revamp the labeling scheme so that labels fell into one of a few different categories, so that looking at assembly code was more clear.

After the compiler was working, I 'branded' my code by putting headers at the top of each file with a short description of what they do. All instructor and student test cases work properly, including many of the standard C library functions (atoi, printf, etc.) and recursion.

This project is far from over for me, however. There were many aspects of the language that we were told to leave out from the final submission, such as higher-dimension arrays of different types, arrays as parameters, unlimited function parameters, etc. that I would love to try and get my hands dirty with. I will definitely be trying to updated the compiler with some of these features, now that I have a solid foundation to work with.

## Codegen Progress Check #1
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
