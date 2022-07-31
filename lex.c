#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#define MAX_STRING_LENGTH 1024
#define MAX_TOKEN_LENGTH 11
#define MAX_DIGITS 5
#define TOKEN_NUM 37
// Benjamin Richards
// Anthony Arronte
// COP3402, Summer 2022

// Initializes the 'symbols' array of strings cooresponding with the tokens above
char **assign_symbols(char **symbols)
{
	if (symbols == NULL)
		return NULL;
	for (int i = 0; i < TOKEN_NUM; i++)
		symbols[i] = malloc(sizeof(char) * MAX_TOKEN_LENGTH);
	
	// Needs to be token - 1 because tokens start at 1 not 0
	strcpy(symbols[elsesym - 1], "else");
	strcpy(symbols[plussym - 1], "+\0");
	strcpy(symbols[minussym - 1], "-\0");
	strcpy(symbols[multsym - 1], "*\0");
	strcpy(symbols[slashsym - 1], "/\0");
	strcpy(symbols[modsym - 1], "%\0");
	strcpy(symbols[eqlsym - 1], "=\0");
	strcpy(symbols[neqsym - 1], "!=\0");
	strcpy(symbols[lessym - 1], "<\0");
	strcpy(symbols[leqsym - 1], "<=\0");
	strcpy(symbols[gtrsym - 1], ">\0");
	strcpy(symbols[geqsym - 1], ">=\0");
	strcpy(symbols[lparentsym - 1], "(\0");
	strcpy(symbols[rparentsym - 1], ")\0");
	strcpy(symbols[commasym - 1], ",\0");
	strcpy(symbols[semicolonsym - 1], ";\0");
	strcpy(symbols[periodsym - 1], ".\0");
	strcpy(symbols[becomessym - 1], ":=\0");
	strcpy(symbols[beginsym - 1], "begin");
	strcpy(symbols[endsym - 1], "end");
	strcpy(symbols[ifsym - 1], "if");
	strcpy(symbols[thensym - 1], "then");
	strcpy(symbols[whilesym - 1], "while");
	strcpy(symbols[dosym - 1], "do");
	strcpy(symbols[callsym - 1], "call");
	strcpy(symbols[constsym - 1], "const");
	strcpy(symbols[varsym - 1], "var");
	strcpy(symbols[procsym - 1], "procedure");
	strcpy(symbols[writesym - 1], "write");
	strcpy(symbols[readsym - 1], "read");
	strcpy(symbols[addassignsym - 1], "+=");
	strcpy(symbols[subassignsym - 1], "-=");
	strcpy(symbols[timesassignsym - 1], "*=");
	strcpy(symbols[divassignsym - 1], "/=");
	strcpy(symbols[modassignsym - 1], "%=");
	
	return symbols;
}

// Reads from the next part of the program, determines and assigns the lexeme
lexeme *get_lexeme(lexeme *lexeme_list, char **symbols, char *source_program, int *list_index, int *source_index)
{
	int beginCom;
	// Safety Check
	if (source_program == NULL || lexeme_list == NULL)
		return NULL;
	// Holds the current section of program text
	char buffer[MAX_STRING_LENGTH];
	int i = 0, j = 0;
	int len = strlen(source_program);
	// Ignores whitespace, tabs, and newline characters
	while (source_program[*source_index] == ' ' || iscntrl(source_program[*source_index]))
	{
		(*source_index)++;
		if (*source_index >= len)
			return lexeme_list;
	}
	
	// If the first two characters are "/*" then a comment has begun
	if (source_program[*source_index] == '/' && source_program[(*source_index) + 1] == '*')
	{
		while (1)
		{
			// If the string "*/" is read, end the comment
			if (source_program[*source_index] == '*' && source_program[(*source_index) + 1] == '/')
			{
				*source_index += 2;
				return lexeme_list;
			}
			// If the index has reached the end of the file, the comment was never terminated
			if (*source_index == len)
			{
				lexeme_list[*list_index].error_type = 5;
				*list_index += 1;
				return lexeme_list;
			}
			// Otherwise, ignore everything in between the comment symbols
			(*source_index)++;
		}
	}
	// If the first character is a letter it must be a token or variable name
	if ((isalpha(source_program[*source_index])))
	{
		// Variable names must only contain numbers or letters, so if the current index is
		// neither then the end of the lexeme has been reached
		while (isalnum(source_program[*source_index]))
		{
			buffer[i] = source_program[*source_index];
			buffer[i + 1] = '\0';
			i++;
			(*source_index)++;
		}
		
		// After the lexeme is stored in buffer, loop through the 'symbols' array and
		// compare the lexeme to every token to see if it matches
		for (j = 1; j <= TOKEN_NUM; j++)
		{
			// If there is a match, allocate and assign the name, and assign the token
			if (strcmp(buffer, symbols[j - 1]) == 0)
			{
				lexeme_list[*list_index].type = j;
				strcpy(lexeme_list[*list_index].name, buffer);
				*list_index += 1;
				return lexeme_list;
			}
		}
		// If no match is found, the lexeme must be a variable name
		// which can be no longer than 11 characters as specified in instructions.
		if (strlen(buffer) > MAX_TOKEN_LENGTH)
		{
			lexeme_list[*list_index].error_type = 3;
			*list_index += 1;
			return lexeme_list;
		}
		
		// Otherwise it is a valid variable name. Allocate and the assign the name, and
		// set the token to the 'identifier' token
		strcpy(lexeme_list[*list_index].name, buffer);
		lexeme_list[*list_index].type = identsym;
		*list_index += 1;
		return lexeme_list;
	}
	// If the first character of the lexeme is a number
	else if (source_program[*source_index] >= '0' && source_program[*source_index] <= '9')
	{
		// A number can only have numbers in it so once a non-numeric char is reached
		// the end of the lexeme has been reached.
		while (source_program[*source_index] >= '0' && source_program[*source_index] <= '9')
		{
			// Store the string representation of the number in 'buffer'
			buffer[i] = source_program[*source_index];
			buffer[i + 1] = '\0';
			i++;
			(*source_index)++;
		}
		// If the non-numeric char is a letter, it must be an improper variable name
		// as variable names and numbers MUST be separated with a space
		if (isalpha(source_program[*source_index]))
		{
			// Store the improperly named variable and increment the source program index
			// So we can continue reading from the proper place
			while (isalnum(source_program[*source_index]))
			{
				buffer[i] = source_program[*source_index];
				buffer[i + 1] = '\0';
				i++;
				(*source_index)++;
			}
			
			lexeme_list[*list_index].error_type = 1;
			*list_index += 1;
			return lexeme_list;
		}
		// As per the instructions, numbers cannot exceed 5 digits in length
		if (strlen(buffer) > 5)
		{
			lexeme_list[*list_index].error_type = 2;
			*list_index += 1;
			return lexeme_list;
		}
		// Otherwise it is a valid number. Use the 'atoi()' function to convert 'buffer' into
		// the actual number and store it in 'value'. Set the token to the 'number' token.
		lexeme_list[*list_index].type = numbersym;
		lexeme_list[*list_index].value = atoi(buffer);
		*list_index += 1;
		return lexeme_list;
	}
	// If the first character is not alphabetical or numerical, it is either a token or invalid
	else
	{
		i = 0;
		// Symbol tokens must only contain symbols so if an alphabetical or numerical char is
		// reached, it must be the end of the lexeme
		while (ispunct(source_program[*source_index]))
		{
			buffer[i++] = source_program[*source_index];
			buffer[i] = '\0';
			(*source_index) += 1; 
			if (buffer[0] == '<' || buffer[0] == '>' || buffer[0] == '+' || buffer[0] == '-' || buffer[0] == '*' || buffer[0] == '/' || buffer[0] == '%')
			{
				if (source_program[*source_index] == '=')
				{
					buffer[i++] = '=';
					buffer[i] = '\0';
					// lexeme_list[*list_index].type = j;
					// strcpy(lexeme_list[*list_index].name, buffer);
					// *list_index += 1;
					*source_index += 1;
				}
			}
			for (j = 1; j <= TOKEN_NUM; j++)
			{
				// If a match is found, allocate and store the name. The index of symbols that matched
				// is also the token integer.
				if (strcmp(buffer, symbols[j - 1]) == 0)
				{
					lexeme_list[*list_index].type = j;
					strcpy(lexeme_list[*list_index].name, buffer);
					*list_index += 1;
					return lexeme_list;
				}
			}
		}
		// Loop through the 'symbols' array and compare the lexeme to the token list above
		// If no match is found, the symbol is not in our lexicon and is invalid
		lexeme_list[*list_index].error_type = 4;
		*list_index += 1;
	}
	return lexeme_list;
}
void print_lex_error(int error_type)
{
	switch (error_type)
	{
		case 1:
		printf("Lexical Analyzer Error: Variable identifier does not start with letter.\n");
		break;
		
		case 2:
		printf("Lexical Analyzer Error: Number length exceeds 5 digits.\n");
		break;
		
		case 3:
		printf("Lexical Analyzer Error: Identifier length exceeds 11 characters.\n");
		break;
		
		case 4:
		printf("Lexical Analyzer Error: Invalid Symbol.\n");
		break;
		
		case 5:
		printf("Lexical Analyzer Error: Neverending comment\n");
		break;
	}
}
// Prints each Lexeme and token type in the read file, followed by the Lexeme list
int print_lex_list(lexeme *lexeme_list, int list_index)
{
	int i, is_error = 0;
	printf("Lexeme Table:\n");
	printf("lexeme       tokentype\n");
	// Prints in the same format as given in the example in instructions
	for (i = 0; i < list_index; i++)
	{
		if (lexeme_list[i].error_type > 0)
		{
			is_error = 1;
			print_lex_error(lexeme_list[i].error_type);
			continue;
		}
		if (strcmp(lexeme_list[i].name, "") != 0)
			printf("%-13s", lexeme_list[i].name);
		else if (lexeme_list[i].value >= 0)
			printf("%-13d", lexeme_list[i].value);
		
		if (lexeme_list[i].type > 0)
			printf("%d\n", lexeme_list[i].type);		
	}
	printf("\nToken List:\n");
	for (i = 0; i < list_index; i++)
	{
		if (lexeme_list[i].type > 0)
			printf("%d ", lexeme_list[i].type);		
		if (lexeme_list[i].type == 2)
			printf("%s ", lexeme_list[i].name);
		else if (lexeme_list[i].type == 3)
			printf("%d ", lexeme_list[i].value);
	}
	printf("\n\n");
	return is_error;
}
// void write_lex_list(lexeme *lexeme_list, int list_index)
// {
	// int i;
	// FILE *ifp;
	// char filename[30];
	// strcpy(filename, "lex_output.txt");
	// if ((ifp = fopen(filename, "w")) == NULL)
	// {
		// fprintf(stderr, "Unable to open or create file in 'write_lex_list'\n");
		// abort();
	// }
	// for (i = 0; i < list_index; i++)
	// {
		// if (lexeme_list[i].type > 0)
			// fprintf(ifp, "%d ", lexeme_list[i].type);		
		// if (lexeme_list[i].type == 2)
			// fprintf(ifp, "%s ", lexeme_list[i].name);
		// else if (lexeme_list[i].type == 3)
			// fprintf(ifp, "%d ", lexeme_list[i].value);
	// }
	
	// fclose(ifp);
// }

// Reads PL/0 code from a text file and converts it into 'lexemes' using "get_lexeme()"
lexeme *lexical_analyzer(char *input)
{
	// Stores the actual lines of code from the txt file
	char *source_program = malloc(sizeof(char) * MAX_STRING_LENGTH + 1);
	
	// An array of 'lexeme' structs that holds each lexeme gotten from the txt file
	lexeme *lexeme_list = malloc(sizeof(lexeme) * MAX_STRING_LENGTH + 1);
	
	// Stores the list of valid tokens as enumerated above
	char **symbols = malloc(sizeof(char *) * TOKEN_NUM);
	symbols = assign_symbols(symbols);
	int i = 0;
	
	// Stores the current index of the 'source_program'
	int *source_index = malloc(sizeof(int));
	
	// Stores the current index of the 'lexeme_list'
	int *list_index = malloc(sizeof(int));
	
	// Stores the length of the 'source_program'
	int source_len;
	*list_index = 0;
	*source_index = 0;
	
	// Gets the PL/0 code from 'input'
	strcpy(source_program, input);
	source_len = strlen(source_program);
	while (*source_index < source_len)
	{
		lexeme_list = get_lexeme(lexeme_list, symbols, source_program, list_index, source_index);
	}
	//write_lex_list(lexeme_list, *list_index);
	if (print_lex_list(lexeme_list, *list_index) == 1)
	{
		free(lexeme_list);
		lexeme_list = NULL;
	}
		
	// Free all dynamically allocated memory
	for (i = 0; i < TOKEN_NUM; i++)
		free(symbols[i]);
	
	free(list_index);
	free(symbols);
	free(source_index);
	free(source_program);
	return lexeme_list;
}
