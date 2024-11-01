# C++ Fortran Interpreter

Interpreter for SFort95, a Fortran 95-like language. Written in C++. Consists of a lexical analyzer, parser, and evaluator.

# Setup

### 1. Clone the repository

Paste this link into Visual Studio and clone the repo:

`https://github.com/siudn/FortranInterpreter.git`

### 2. Enter "Interpreter" folder

Expand the "Interpreter" folder.

### 3. Enter the `Interpreter.sln` solution file

Double click this file to open the full interpreter.

### 4. Build solution, add desired test case and run!

Test cases with expected outcomes are provided under "Test Cases". You can add the filename in through VS launch profiles or from the command line.

Example: `./interpreter test1`

# Additional Notes

A full description of the language with EBNF notations, operator precedence table, and additional notes has been provided in the file `SFort95.txt`.

The parser and lexer are also provided as separate executables with their own sets of test cases if you want to run/test them alone.
