#include <stdio.h>
#include <stdlib.h>
// Benjamin Richards
// Anthony Arronte
// COP3402, Summer 2022

typedef struct CPU
{
	int bp;
	int sp;
	int pc;
} CPU;


typedef struct IR
{
	int opcode;
	int l;
	int m;
} IR;

// Prints the contents of the stack starting at the original bp and ending at the current sp
void printInstruction(CPU CPU, int pas[], int AR[], int start)
{
	int i;
	for (i = start; i <= CPU.sp; i++)
	{
		printf("%d ", pas[i]);
		// Checks if current element is an sp of any function and prints a vertical line if it is
		if (i != CPU.sp && AR[i] == 1)
			printf("| ");
	}
	printf("\n");
	
}

// Given from instructions but changes parameters to access the required elements
int base(CPU CPU, int pas[], int l)
{
	int arb = CPU.bp;	// arb = activation record base
	while (l > 0)     // find base L levels down
	{
		arb = pas[arb];
		l--;
	}
	return arb;
}

int main(int argc, char **argv)
{
	FILE *ifp;
	CPU CPU;
	IR IR;
	int pas[500] = {0};
	
	// Stores the location of the stack pointers of each function in order to print the
	// vertical bar
	int AR[500] = {0};
	
	int i;
	int temp = 0;
	int halt = 1;
	
	// stores the start of the stack to start printing from
	int startPrint;
		
	// Safety Checks
	if (argc < 2)
	{
		fprintf(stderr,  "Arg count fewer than 2");
		return 1;
	}
	if ((ifp = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "Program aborted, failed to open %s in main()\n", argv[2]);
		return 1;
	}

	CPU.bp = 0;
	// Reads each integer from the file and stores it in "pas"
	// Uses bp as the index so that it ends up at the proper address after being incremented
	while (fscanf(ifp, "%d", &temp) != EOF)
	{
		pas[CPU.bp] = temp;
		CPU.bp++;
	}
	
	// Initialize the values in CPU to Properly
	CPU.sp = CPU.bp - 1;
	CPU.pc = 0;
	startPrint = CPU.bp;

	// Table header
	printf("                 PC  BP  SP  stack\n");
	printf("Initial values:  %d   %d  %d\n", CPU.pc, CPU.bp, CPU.sp);
	
	
	while (halt == 1)
	{
		// Instruction fetch
		IR.opcode = pas[CPU.pc];
		IR.l = pas[CPU.pc + 1];
		IR.m = pas[CPU.pc + 2];
		CPU.pc += 3;

		// Instruction execute
		switch (IR.opcode)
		{
			// LIT 0, M
			case 1:
			// Sets the new sp index to 1 and resets the previous to 0 to keep track
			AR[CPU.sp] = 0;
			AR[CPU.sp + 1] = 1;
			
			// Pushes 'M' onto the stack
			CPU.sp++;
			pas[CPU.sp] = IR.m;
			printf("LIT  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
			break;
			
			// OPR 0, #
			case 2:
			switch (IR.m)
			{
				// RTN
				case 0:
				AR[CPU.sp] = 0;
				// Returns to the previous function by putting the AR in the CPU
				CPU.sp = CPU.bp - 1;
				CPU.bp = pas[CPU.sp + 2];
				CPU.pc = pas[CPU.sp + 3];
				printf("RTN  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// NEG
				case 1:
				// Negates the value at the top of the stack
				pas[CPU.sp] *= -1;
				printf("NEG  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// ADD
				case 2:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// Adds the value at the top of the stack with the previous one,
				// storing the result in the previous value's location
				CPU.sp--;
				pas[CPU.sp] += pas[CPU.sp + 1];
				printf("ADD  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// SUB
				case 3:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// Subtracts the value at the top of the stack with the previous one,
				// storing the result in the previous value's location
				CPU.sp--;
				pas[CPU.sp] -= pas[CPU.sp + 1];
				printf("SUB  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// MUL
				case 4:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// Multiplies the value at the top of the stack by the previous one,
				// storing the result in the previous value's location
				CPU.sp--;
				pas[CPU.sp] = pas[CPU.sp] * pas[CPU.sp + 1];
				printf("MUL  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// DIV
				case 5:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// Divides the value at the top of the stack by the previous one,
				// storing the result in the previous value's location
				CPU.sp--;
				pas[CPU.sp] /= pas[CPU.sp + 1];
				printf("DIV  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// MOD
				case 6:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// Mods the value at the top of the stack by the previous one,
				// storing the result in the previous value's location
				CPU.sp--;
				pas[CPU.sp] %= pas[CPU.sp + 1];
				printf("MOD  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// EQL
				case 7:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// If the value at the top of the stack is equal to the one before it,
				// stores a 1, otherwise stores a 0 in the previous value's location.
				CPU.sp--;
				pas[CPU.sp] = (pas[CPU.sp] == pas[CPU.sp + 1]);
				printf("EQL  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// NEQ
				case 8:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// If the value at the top of the stack is not equal to the one before it,
				// stores a 1, otherwise stores a 0 in the previous value's location.
				CPU.sp--;
				// If the value at the top of the stack is not equal to the one before it,
				// stores a 1, otherwise stores a 0 in the previous value's location.
				pas[CPU.sp] = (pas[CPU.sp] != pas[CPU.sp + 1]);
				printf("NEQ  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// LSS
				case 9:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// If the value at the top of the stack is less than the one before it,
				// stores a 1, otherwise stores a 0 in the previous value's location.
				CPU.sp--;
				pas[CPU.sp] = pas[CPU.sp] < pas[CPU.sp + 1];
				printf("LSS  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// LEQ
				case 10:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// If the value at the top of the stack is less than or equal to the one before it,
				// stores a 1, otherwise stores a 0 in the previous value's location.
				CPU.sp--;
				pas[CPU.sp] = pas[CPU.sp] <= pas[CPU.sp + 1];
				printf("LEQ  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// GTR
				case 11:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// If the value at the top of the stack is greater than the one before it,
				// stores a 1, otherwise stores a 0 in the previous value's location.
				CPU.sp--;
				pas[CPU.sp] = pas[CPU.sp] > pas[CPU.sp + 1];
				printf("GTR  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				
				// GEQ
				case 12:
				// Sets the new sp index to 1 and resets the previous to 0 to keep track
				AR[CPU.sp] = 0;
				AR[CPU.sp - 1] = 1;
				// If the value at the top of the stack is greater than or equal to the one before it,
				// stores a 1, otherwise stores a 0 in the previous value's location.
				CPU.sp--;
				pas[CPU.sp] = pas[CPU.sp] >= pas[CPU.sp + 1];
				printf("GEQ  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
			}
			break;
			
			// LOD L, M
			case 3:
			// Sets the new sp index to 1 and resets the previous to 0 to keep track
			AR[CPU.sp] = 0;
			AR[CPU.sp + 1] = 1;
			// Uses the base function to go back 'IR.l' levels and retrieves the value at 'IR.m'
			// offset from the bp, pushing it to the top of the stack
			CPU.sp++;
			pas[CPU.sp] = pas[base(CPU, pas, IR.l) + IR.m];
			printf("LOD  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
			break;
			
			// STO L, M
			case 4:
			// Sets the new sp index to 1 and resets the previous to 0 to keep track
			AR[CPU.sp] = 0;
			AR[CPU.sp - 1] = 1;
			// Uses the base function to go back 'IR.l' levels and stores the value at the
			// top of the stack in the address at 'IR.m' offset from the bp
			pas[base(CPU, pas, IR.l) + IR.m] = pas[CPU.sp];
			CPU.sp--;
			printf("STO  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
			break;
			
			// CAL L, M
			case 5:
			// Calls a function - creates the activation record for a new function
			// and moves the bp and pc to their new locations
			pas[CPU.sp + 1] = base(CPU, pas, IR.l);
			pas[CPU.sp + 2] = CPU.bp;
			pas[CPU.sp + 3] = CPU.pc;
			CPU.bp = CPU.sp + 1;
			CPU.pc = IR.m;
			printf("CAL  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
			break;
			
			// INC 0, M
			case 6:
			// Moves the stack pointer by 'IR.m' spaces
			CPU.sp = CPU.sp + IR.m;
			AR[CPU.sp + 1] = 1;
			printf("INC  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
			break;
			
			// JMP 0, M
			case 7:
			// Jumps to line 'IR.m' in the instruction register
			CPU.pc = IR.m;
			printf("JMP  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
			break;
			
			// JPC 0, M
			case 8:
			// If the value at the top of the stack is 0, jumps to line 'IR.m' in the instruction register
			if (pas[CPU.sp] == 0)
				CPU.pc = IR.m;
			CPU.sp--;
			printf("JPC  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
			break;
			
			// SYS 0, #
			case 9:
			switch(IR.m)
			{
				// SYS 0, 1 (PRINT)
				case 1:
				// Prints out the value at the top of the stack
				printf("Output result is: %d\n", pas[CPU.sp]);
				CPU.sp--;
				printf("SYS  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				// SYS 0, 2 (SCAN)
				case 2:
				// Scans an input from the console and pushes it to the top of the stack
				CPU.sp++;
				printf("Please Enter an Integer: ");
				scanf("%d", &pas[CPU.sp]);
				printf("SYS  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
				// SYS 0, 3 (END PROGRAM)
				case 3:
				// Sets the halt flag to 0 and ends the program
				halt = 0;
				printf("SYS  %d  %d  %s    %d  %d  %d  ", IR.l, IR.m, (IR.m > 9)? " ": "  ", CPU.pc, CPU.bp, CPU.sp);
				break;
			}
			break;
		}
		
		printInstruction(CPU, pas, AR, startPrint);
	}
	fclose(ifp);
}
