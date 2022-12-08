# PL0-Compiler
In Systems software we were tasked with writing a compiler for the PL/0 language in C. This is my completed version.
To compile, make sure all C files are downloaded and in the same folder, as well as any text files of PL/0 code to read from. Then, at the command line,
type "gcc driver.c parser.c lex.c vm.c -lm" and hit enter. Then, type "./a.out FILE_NAME.txt" and hit enter. This will print the lexeme list, the symbol table, the assembly code, and the execution of the assembly code on the stack. The following are descriptions of each file:

compiler.h contains the struct definitions, token enumerations, and function prototypes for everthing used in the C files.
<br />
driver.c reads input from a file of PL/0 code, and calls the proper functions from lex., parser.c, and vm.c to act as an easier to read hub for the other 3 C files. The text files provided are examples to try out. <br />
PL/0 File Example: <br />
var x, y; <br />
begin <br />
>y := 3; <br />
>x := y + 56; <br />

end. <br />

Lexicographical Analyzer (lex.c):
This takes in a program in PL/0 code, checks for symbol validity, and converts it into lexemes, a struct as defined below. A lexeme can be for a variable, a keyword, or a token. For a full list of accepted tokens, please refer to "compiler.h". <br />

typedef struct lexeme { <br />
>token_type type; <br />
>char name[12]; <br />
>int value; <br />
>int error_type; <br />

} lexeme <br />
		     
Parser (parser.c):
This takes a lexeme list, like the one returned from "lex.c", creates a symbol table, checks for proper syntax, and generates an array of integers to represent assembly code, as seen in "vm.c". <br />

typedef struct symbol { <br />
>int kind;<br />
>char name[12];<br />
>int value;<br />
>int level;<br />
>int addr;<br />
>int mark;<br />

} symbol;<br />

Virtual Machine (vm.c):
This fetches instructions from assembly language and performs them as described in the ISA listed in the attached "VirtualMachineISA.docx" file.
This simulates a really primitive version of the call stack, storing instructions in the first half of an array and the stack (i.e. activation records and variables), in the other half. <br /> <br />
Text File Example: <br /> <br />
7 0 45 (JMP 0 15) <br />
7 0 6  (JMP 0 2) <br />
6 0 4  (INC 0 4) <br />
1 0 4  (LIT 0 4) <br />
1 0 3  (LIT 0 3) <br /> <br />
(Note that assembly instructions contain opcode, L, and M so the total number of integers must be divisible by 3)
