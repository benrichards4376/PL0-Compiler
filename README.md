# PL0-Compiler
This is my project for Systems Software. We are working on a compiler for the PL/0 language. This will be updated more and more as it gets completed.
As of now, this is what has been completed:

Lexicographical Analyzer (lex.c):
This takes in a program in PL/0 code and converts it into lexemes, a struct as defined below. A lexeme can be for a variable, a keyword, or a token. lex_test.txt is a sample input file that can be run as an example. For a full list of accepted tokens, please refer to "compiler.h".

typedef struct lexeme { <br />
	token_type type; <br />
	char name[12]; <br />
	int value; <br />
	int error_type; <br />
} lexeme <br />

To run the program you must use command line arguments. Where the first argument is the lex.c file and the second is the text document with PL/0 code instructions formatted as below.

PL/0 File Example:
var x, y; <br />
begin <br />
	y := 3; <br />
	x := y + 56; <br />
end. <br />

To compile the code type gcc lex.c into the command line and hit enter. Then to run, type ./a.out followed by a space and the name of the PL/0 code text document as seen below. <br />
	Example: <br /> $ gcc lex.c <br />
		     $ ./a.out lex_test.txt
		     
Virtual Machine (vm.c):
This fetches instructions from assembly language and performs them as described in the ISA listed in the attached "VirtualMachineISA.docx" file.
This simulates a really primitive version of the call stack, storing instructions in the first half of an array and the stack (i.e. activation records and variables), in the other half. vm_test.txt is a sample input file that can be run as an example. These are instructions on how to run it:

To run the program you must use command line arguments. Where the first argument is the vm.c file and the second is the text document with assembly code instructions formatted as below.

Text File Example: <br />
7 0 45 <br />
7 0 6 <br />
6 0 4 <br />
1 0 4 <br />
1 0 3 <br />
(Note that the text file must contain only integers separated by spaces, and the total number of integers must be divisible by 3)

To compile the code type gcc vm.c into the command line and hit enter. Then to run, type ./a.out followed by a space and the name of the assembly code text document as seen below. <br />
	Example: <br /> $ gcc vm.c <br />
		     $ ./a.out vm_test.txt
