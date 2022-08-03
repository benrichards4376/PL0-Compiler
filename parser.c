// Converts the lexeme list provided by lex.c into assembly instructions, 'code'
// Benjamin Richards
// Anthony Arronte
// COP3402, Summer 2022

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"
// To run, put the following in the command line and hit enter.
// Then, type ./a.out YOURFILENAME.txt and hit enter.
//gcc driver.c lex.o parser.c vm.o -lm 

// Lexeme list from lex.c
lexeme *tokens;
// Index of the list
int lIndex = 0;
// Array of assembly instructions to be returned
instruction *code;
// Index of code array
int cIndex = 0;
// Symbol table
symbol *table;
// Table index
int tIndex = 0;
// Keeps track of the current lexicographical level
int level = 0;
// In case a procedure is empty
int emptyProcFlag = 0;

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void mark();
int multipleDeclarationCheck(char name[]);
int findSymbol(char name[], int kind);
int betterFindSymbol(char name[]);
void printParseError(int err_code);
void printSymbolTable();
void printAssemblyCode();
int programs(void);
int block(void);
int constDec(void);
int variableDec(void);
int procDec(void);
int statement(void);
int condition(void);
int relOp(void);
int expression(void);
int term(void);
int factor(void);
void getToken(void);

// Allocates all needed data, starts the code generation, returns the assembly code array
// and frees all other data
instruction *parser_code_generator(lexeme *list)
{
	int i;
	tokens = list;
	code = malloc(sizeof(instruction) * MAX_ARRAY_SIZE);
	table = malloc(sizeof(symbol) * MAX_ARRAY_SIZE);
	
	if (table == NULL)
	{
		free(code);
		return NULL;
	}
	
	// If an error is encountered in any of the functions it will return -1
	// and cascade back to here.
	if (programs() == -1)
	{
		free(table);
		return NULL;
	}
	
	// Loops through CAL and STO instructions and replaces the "dummy" values with
	// their correct ones
	for (i = 0; i < cIndex; i++)
	{
		if (code[i].op == 5)
		{
			code[i].m = table[code[i].m].addr * 3;
		}
	}
	// printSymbolTable();
	// printAssemblyCode();
	// Your code here. 
	// Make sure to remember to free table before returning always
	// and to free code and return NULL if there was an error
	
	code[cIndex].op = -1;
	free(table);
	return code;
}

// Processes the whole program
int programs(void)
{
	// Dummy JMP for 'main' to be assigned at the end
	emit(7, 0, 0);
	addToSymbolTable(3, "main", 0, 0, 0, 0);
	level = -1;
	
	if (block() == -1)
		return -1;
	
	// All programs must end with a '.'
	if (tokens->type != periodsym)
		getToken();
	if (tokens->type != periodsym)
	{
		printParseError(1);
		return -1;
	}
	// Halt
	emit(9, 0 , 3);
	// Fixes the Dummy value in the JMP code for 'main'
	code->m = table->addr * 3;
	
}

// Handles everything up until the period in the program
int block(void)
{
	// tIndex is always at the next available space in the symbol table
	// so tIndex - 1 is the last entry.
	int currProc = tIndex - 1;
	
	// Stores the number of variables declared so we can INC properly later
	int numVar;
	level++;
	
	// While loop we're only declaring variables, constants, and procedures and not assigning
	while (tokens->type == constsym || tokens->type == varsym || tokens->type == procsym)
	{
		if (tokens->type == constsym)
		{
			if (constDec() == -1)
				return -1;
		}
		else if (tokens->type == varsym)
		{
			numVar = variableDec();
			if (numVar == -1)
				return -1;
		}
		else if (tokens->type == procsym)
		{
			if (procDec() == -1)
				return -1;
		}
	}
	
	// After all declarations are done we can find and store 
	// the proper address of the current procedure
	table[currProc].addr = cIndex;
	
	// INC for the variables declared and + 3 for the Activation Record (SL, DL, RA)
	emit(6, 0, numVar + 3);
	if (emptyProcFlag == 1)
	{
		getToken();
	}
	if (statement() == -1)
		return -1;
	emptyProcFlag = 0;
	// Mark all symbols and decrement the level as we leave the function
	mark();
	level--;
	return 0;
}

// Declares and assigns all constants, passed the 'const' token
int constDec(void)
{
	int tempIndex;
	getToken();
	// While the end of the declarations have not been reached
	while (tokens->type != semicolonsym)
	{
		if (tokens->type == identsym)
		{
			// If the current token is an identifier but is already in the symbol table
			if ((tempIndex = findSymbol(tokens->name, 1) != -1))
			{
				// If the found token and the current token are at the same level
				// Multiple declarations found
				if (table[tempIndex].level == level)
				{
					printParseError(18);
					return -1;
				}
			}
			addToSymbolTable(1, tokens->name, 0, level, 0, 0);
		}
		// Const Declarations must follow form (ident := number {, ident := number};
		else
		{
			printParseError(2);
			return -1;			
		}
		getToken();
		if (tokens->type == becomessym)
		{
			getToken();
			// Store the value
			if (tokens->type == numbersym)
			{
				table[tIndex - 1].value = tokens->value;
			}
			// Const Declarations must follow form (ident := number {, ident := number};
			// No number after becomes
			else
			{
				printParseError(2);
				return -1;
			}
		}
		// Const Declarations must follow form (ident := number {, ident := number};
		// No becomes
		else
		{
			printParseError(2);
			return -1;
		}
		getToken();
		// End of the current statement
		if (tokens->type == semicolonsym)
			break;
		// Multiple symbol declarations must be separated by commas
		else if (tokens->type == identsym)
		{
			printParseError(13);
			return -1;
		}
		else if (tokens->type == commasym)
			getToken();
		// Symbol declarations should close with a semicolon
		else
		{
			printParseError(14);
			return -1;
		}
	}
	getToken();
	return 0;
}

// Reponsible for all variable declarations, passed the 'var' token
int variableDec(void)
{
	int numVar = 0;
	int tempIndex;
	do 
	{
		getToken();
		// Variable declarations should follow the pattern 'ident {, ident}'
		// No identifier
		if (tokens->type != identsym)
		{
			printParseError(3);
			return -1;
		}
		tempIndex = findSymbol(tokens->name, 2);
		// Confliciting symbol declarations
		if (tempIndex != -1 && table[tempIndex].level == level)
		{
			printParseError(18);
			return -1;
		}
		
		addToSymbolTable(2, tokens->name, 0, level, 3 + numVar++, 0);
		getToken();
		// End of the variable declarations (semicolon)
		if (tokens->type == semicolonsym)
			break;
		// Multiple symbols in variable and constant declarations must be separated by commas
		else if (tokens->type == identsym)
		{
			printParseError(13);
			return -1;
		}
		// Symbol declarations should close with a semicolon\n
		else if (tokens->type != commasym)
		{
			printParseError(14);
			return -1;
		}
	}
	while (tokens->type == commasym);
	getToken();
	return numVar;
}

// Handles all procedure declarations, passed 'procedure'
int procDec(void)
{
	int tempIndex;
	getToken();
	// Procedure declarations should follow the pattern 'ident ;'
	// Missing 'ident'
	if (tokens->type != identsym)
	{
		printParseError(4);
		return -1;
	}
	// Confliciting symbol declarations
	if ((tempIndex = findSymbol(tokens->name, 3) != -1))
	{
		if (table[tempIndex].level == level)
		{
			printParseError(18);
			return -1;
		}
	}
	addToSymbolTable(3, tokens->name, 0, level, cIndex, 0);
	getToken();
	// Procedure declarations should follow the pattern 'ident ;'
	// Missing ';'
	if (tokens->type != semicolonsym)
	{
		printParseError(4);
		return -1;
	}
	getToken();
	// If end of the procedure reached
	if (tokens->type == semicolonsym)
	{
		emptyProcFlag = 1;
		emit(2, 0, 0);
		return 0;
	}

	if (block() == -1)
		return -1;
	if (tokens->type != semicolonsym)
		getToken();
	
	if (tokens->type == semicolonsym)
	{
		// RTN
		emit(2, 0, 0);
		getToken();
		return 0;
	}
	// Symbol declarations should close with a semicolon
	if (tokens->type != semicolonsym)
	{
		printParseError(14);
		return -1;
	}
	// RTN
	emit(2, 0, 0);
	return 0;
}
// Processes all statements
int statement(void)
{
	int tempVar, otherVar;
	int jpcIdx;
	int jmpIdx;
	int loopIdx;
	// Stores the assignment operator before calling expression
	int tempAssign = -1;
	switch (tokens->type)
	{
		case identsym:
			tempVar = betterFindSymbol(tokens->name);
			// Parser Error: Only variables may be assigned to or read
			if (table[tempVar].kind != 2)
			{
				if (table[tempVar].kind == 1)
				{
					if ((otherVar = findSymbol(tokens->name, 2)) != -1)
					{
						tempVar = otherVar;
					}
					else
					{
						printParseError(6);
						return -1;
					}
				}
				else
				{
					printParseError(6);
					return -1;
				}
			}
			// Undeclared identifier
			if (tempVar == -1)
			{
				printParseError(19);
				return -1;
			}
			getToken();
			// Variables must be assigned using :=
			if (tokens->type != becomessym && tokens->type != addassignsym && tokens->type != subassignsym && tokens->type != timesassignsym && tokens->type != divassignsym && tokens->type != modassignsym)
			{
				printParseError(5);
				return -1;
			}
			else if (tokens->type != becomessym)
			{
				tempAssign = tokens->type;
				emit(3, level - table[tempVar].level, table[tempVar].addr);
			}
			if (expression() == -1)
				return -1;
			if (tempAssign != -1)
			{
				switch (tempAssign)
				{
					case addassignsym:
						emit (2, 0, 2);
						break;
					
					case subassignsym:
						emit (2, 0, 3);
						break;
					
					case timesassignsym:
						emit (2, 0, 4);
						break;
					
					case divassignsym:
						emit (2, 0, 5);
						break;
					
					case modassignsym:
						emit (2, 0, 6);
						break;
				}
			}
			// STO
			emit(4, level - table[tempVar].level, table[tempVar].addr);
			break;
		case callsym:
			getToken();
			tempVar = betterFindSymbol(tokens->name);
			// Call must be followed by a procedure identifier
			if (tokens->type != identsym)
			{
				printParseError(7);
				return -1;
			}
			// Undeclared identifier
			if (tempVar == -1)
			{
				printParseError(19);
				return -1;
			}
			// Call must be followed by a procedure identifier
			if (table[tempVar].kind != 3)
			{
				printParseError(7);
				return -1;
			}
			// CAL
			emit(5, level - table[tempVar].level, tempVar);
			getToken();
			break;
		case beginsym:
			getToken();
			if (statement() == -1)
				return -1;
			do
			{
				getToken();
				if (tokens->type == endsym)
					break;
				if(statement() == -1)
					return -1;
			}
			while (tokens->type == semicolonsym);
			// Statements within begin-end must be separated by a semicolon
			if (tokens->type == identsym || tokens->type == readsym || tokens->type == writesym || tokens->type == beginsym || tokens->type == callsym || tokens->type == ifsym || tokens->type == whilesym)
			{
				printParseError(15);
				return -1;
			}
			// Begin must be followed by end
			else if (tokens->type != endsym) 
			{
				printParseError(16);
				return -1;
			}
			break;
		case ifsym:
			if (condition() == -1)
				return -1;
			// if must be followed by then
			if (tokens->type != thensym)
			{
				printParseError(8);
				return -1;
			}
			jpcIdx = cIndex;
			// JPC
			emit(8, 0, 0);
			getToken();
			if (statement() == -1)
				return -1;
			if (tokens->type == elsesym)
			{
				jmpIdx = cIndex;
				// JMP
				emit(7, 0, 0);
				// Multiply addr * 3 for the PAS in vm.c
				code[jpcIdx].m = cIndex * 3;
				getToken();
				if (statement() == -1)
					return -1;
				// Multiply addr * 3 for the PAS in vm.c
				code[jmpIdx].m = cIndex * 3;
			}
			else
			// Multiply addr * 3 for the PAS in vm.c
			code[jpcIdx].m = cIndex * 3;
			break;
		case whilesym:
			loopIdx = cIndex;
			if (condition() == -1)
				return -1;
			// while must be followed by do
			if (tokens->type != dosym)
			{
				printParseError(9);
				return -1;
			}
			jpcIdx = cIndex;
			// JPC
			emit(8, 0, 0);
			getToken();
			if (statement() == -1)
				return -1;
			// Multiply addr * 3 for the PAS in vm.c
			emit(7, 0, loopIdx * 3);
			// Multiply addr * 3 for the PAS in vm.c
			code[jpcIdx].m = cIndex * 3;
			break;
		case readsym:
			getToken();
			tempVar = betterFindSymbol(tokens->name);
			// Only variables may be assigned to or read
			if (table[tempVar].kind != 2)
			{
				if (table[tempVar].kind == 1)
				{
					if ((otherVar = findSymbol(tokens->name, 2)) != -1)
					{
						tempVar = otherVar;
					}
					else
					{
						printParseError(6);
						return -1;
					}
				}
				else
				{
					printParseError(6);
					return -1;
				}
			}
			// Undeclared identifier
			if (tempVar == -1)
			{
				printParseError(19);
				return -1;
			}
			// SYS 0 2, READ
			emit(9, 0, 2);
			// STO
			emit(4, level - table[tempVar].level, table[tempVar].addr);
			getToken();
			break;
		case writesym:
			if (expression() == -1)
				return -1;
			// SYS 0 1, WRITE
			emit(9, 0, 1);
			break;
		default:
			return 0;	
	}
}

// Handles all the conditions in if and while
int condition(void)
{
	if (expression() == -1)
		return -1;
	if (tokens->type == eqlsym)
	{
		if (expression() == -1)
			return -1;
		// OPR 0 7, EQL
		emit(2, 0, 7);
	}
	else if (tokens->type == neqsym)
	{
		if (expression() == -1)
			return -1;
		// OPR 0 8, NEQ
		emit(2, 0, 8);
	}
	else if (tokens->type == lessym)
	{
		if (expression() == -1)
			return -1;
		// OPR 0 9, LSS
		emit(2, 0, 9);
	}
	else if (tokens->type == leqsym)
	{
		if (expression() == -1)
			return -1;
		// OPR 0 10, LEQ
		emit(2, 0, 10);
	}
	else if (tokens->type == gtrsym)
	{
		if (expression() == -1)
			return -1;
		// OPR 0 11, GTR
		emit(2, 0, 11);
	}
	else if (tokens->type == geqsym)
	{
		if (expression() == -1)
			return -1;
		// OPR 0 12, GEQ
		emit(2, 0, 12);
	}
	// Relational operator missing from condition
	else
	{
		printParseError(10);
		return -1;
	}
}

// Handles all expressions
int expression(void)
{
	getToken();
	if (tokens->type == minussym)
	{
		getToken();
		if(term() == -1)
			return -1;
		// opr 0 1, NEG
		emit(2, 0, 1);
	}
	else
	{
		if (term() == -1)
			return -1;
	}
	while (tokens->type == plussym || tokens->type == minussym)
	{
		if (tokens->type == plussym)
		{
			getToken();
			if (term() == -1)
				return -1;
			// OPR 0 2, ADD
			emit(2, 0, 2);
		}
		else if (tokens->type == minussym)
		{
			getToken();
			if (term() == -1)
				return -1;
			// OPR 0 3, SUB
			emit(2, 0, 3);
		}
	}
	// Bad arithmetic
	if (tokens->type == identsym || tokens->type == numbersym || tokens->type == lparentsym)
	{
		printParseError(17);
		return -1;
	}
}
int term(void)
{
	
	if (factor() == -1)
		return -1;
	getToken();
	while (tokens->type == multsym || tokens->type == slashsym || tokens->type == modsym)
	{
		if (tokens->type == multsym)
		{
			getToken();
			if (factor() == -1)
				return -1;
			// OPR 0 4, MUL
			emit(2, 0, 4);
			getToken();
		}
		else if (tokens->type == slashsym)
		{
			getToken();
			if (factor() == -1)
				return -1;
			// OPR 0 5, DIV
			emit(2, 0, 5);
			getToken();
		}
		else if (tokens->type == modsym)
		{
			getToken();
			if (factor() == -1)
				return -1;
			// OPR 0 5, MOD
			emit(2, 0, 6);
			getToken();
		}
	}
}

int factor(void)
{
	int tempVar, tempConst;
	if (tokens->type == identsym)
	{
		tempConst = findSymbol(tokens->name, 1);
		tempVar = findSymbol(tokens->name, 2);
		if (tempConst > tempVar)
			// LIT
			emit(1, 0, table[tempConst].value);
		else if (tempConst < tempVar)
			// LOD
			emit(3, level - table[tempVar].level, table[tempVar].addr);
		// Arithmetic expressions may only contain arithmetic operators, numbers, parentheses,
		// constants, and variables
		else if ((findSymbol(tokens->name, 3) != -1))
		{
			printParseError(11);
			return -1;
		}
	}
	else if (tokens->type == numbersym)
		// LIT
		emit(1, 0, tokens->value);
	else if (tokens->type == lparentsym)
	{
		if (expression() == -1)
			return -1;
		//getToken();

		// ( must be followed by )
		if (tokens->type != rparentsym)
		{
			printParseError(12);
			return -1;
		}
	}
	else if (tokens->type == semicolonsym)
	{
		return 0;
	}
	// Arithmetic expressions may only contain arithmetic operators,
	// numbers, parentheses, constants, and variables
	else 
	{
		printParseError(11);
		return -1;
	}
}

// adds an instruction to the end of the code array
void emit(int opname, int level, int mvalue)
{
	code[cIndex].op = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

// increments the lIndex which tracks the list array
// also increments the value of tokens, replacing its current value with the next indexs value
void getToken(void)
{
	lIndex++;
	tokens += 1;
}
// adds a symbol to the end of the symbol table
void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].value = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}


// starts at the end of the table and works backward (ignoring already
// marked entries (symbols of subprocedures)) to mark the symbols of
// the current procedure. it knows it's finished the current procedure
// by looking at level: if level is less than the current level and unmarked
// (meaning it belongs to the parent procedure) it stops
void mark(void)
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}
// does a linear pass through the symbol table looking for the symbol
// who's name is the passed argument. it's found a match when the names
// match, the entry is unmarked, and the level is equal to the current
// level. it returns the index of the match. returns -1 if there are no
// matches
int multipleDeclarationCheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

// returns -1 if a symbol matching the arguments is not in the symbol table
// returns the index of the desired entry if found (maximizing the level value)
int findSymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		// checks if token is of the same name, kind and mark 
		// as the other objects in the symbol table
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

// Like the findSymbol function but does not account for kind in the table
int betterFindSymbol(char name[])
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;	
}
// Easy to use function that prints errors that may occur
void printParseError(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=, +=, -=, *=, /=, or %%=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
}
// Prints the symbol table after it is updated in the previous functions
void printSymbolTable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].value, table[i].level, table[i].addr, table[i].mark); 
	printf("\n");
}

// Responsible for printing the assembly code table using our instruction pointer, code
void printAssemblyCode()
{
	int i;
	printf("Assembly Code:\n");
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].op);
		// Prints operation based on the op code value
		switch (code[i].op)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("MOD\t");
						break;
					case 7:
						printf("EQL\t");
						break;
					case 8:
						printf("NEQ\t");
						break;
					case 9:
						printf("LSS\t");
						break;
					case 10:
						printf("LEQ\t");
						break;
					case 11:
						printf("GTR\t");
						break;
					case 12:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				printf("SYS\t");
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	printf("\n");
}