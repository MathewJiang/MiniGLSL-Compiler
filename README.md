# MiniGLSL Compiler

## Project Overview
This project is to implement a compiler which interprets the code written in MiniGLSL into ARB Assembly. This project is written in C.


***
## Milestone 1:
Objective: To implement the tokenization functionality on the source code.
* Implemented the identification mechanism by treating the input file as a stream of characters.
* Separated the token into categories such as INT_TYPE (and other type decorators), VARIABLE (i.e. variable name), IF (and other MiniGLSL keywords), ADD (and other MiniGLSL mathematical or logical operators).
* Tested with 20+ source codes written in MiniGLSL.

***
## Milestone 2:
Objective: To implement the parsing of the syntax tree.
* Designed and implemented the syntax grammar, which will be used to build up a syntax tree.
* Tested with 10+ different scenarios to make sure the grammar being unambiguous.

(Note: this step will not be executed if there exists tokenization errors (i.e. not passing step 1))

***
## Milestone 3:
Objective: To build a AST (i.e. Abstract Syntax Tree) and implement the type checking and scope checking.
* Implemented of an AST based on the syntax grammars written in Milestone 2.
* Implemented the type checking by associating the variable name with its corresponding type and the value which it has been assigned to; this includes primary data types and vector data types.
* Implemented the scoping checking using spaghetti stacks (i.e. stacks and linked lists); variable names are out of scope if such variable cannot be found within the stack.
* Tested with 20+ source codes written in MiniGLSL.

(Note: this step will not be executed if there exists syntax errors(i.e. not passing step 2).)

***
## Milestone 4:
Objective: To implement the code generation.
* Implemented the code generation by passing in the AST
* Implemented some easy dead code elimination (i.e. if the statement within the IF clause is false, no assembly code within that clause will be generated).
* Generated successfully the image as the intention of the sample MiniGLSL code.

(Note: Code generation will not be executed if there exists type errors and scope errors.)




