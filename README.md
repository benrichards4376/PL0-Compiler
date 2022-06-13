# PL0-Compiler
This is my project for Systems Software. We are working on a compiler for the PL/0 language. This will be updated more and more as it gets completed.
As of now, this is what has been completed:

Virtual Machine:
This fetches instructions from assembly language and performs them as described in the ISA listed in the attached "VirtualMachineISA.docx" file.
This simulates a really primitive version of the call stack, storing instructions in the first half of an array and the stack (i.e. activation records and variables), in the other half. These are instructions on how to run it:

To run the program you must use command line arguments. Where the first argument is the vm.c file and the second is the text document with assembly code instructions formatted as below.

Text File Example: <br />
7 0 45 <br />
7 0 6 <br />
6 0 4 <br />
1 0 4 <br />
1 0 3 <br />
(Note that the text file must contain only integers separated by spaces, and the total number of integers must be divisible by 3)

To compile the code type gcc vm.c into the command line and hit enter. Then to run, type ./a.out followed by a space and the name of the assembly code text document as seen below.
	Example:  $ gcc vm.c
		     $ ./a.out input.txt
