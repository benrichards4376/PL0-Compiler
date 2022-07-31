#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"
int main(int argc, char *argv[])
{
	FILE *ifp;
	char *input = calloc(MAX_ARRAY_SIZE, sizeof(char));
	lexeme *list;
	instruction *code;
	char buffer;
	int i = 0;
	
	if (argc < 2)
	{
		printf("Error : please include the file name\n");
		return 0;
	}
	ifp = fopen(argv[1], "r");
	while (fscanf(ifp, "%c", &buffer) != EOF)
		input[i++]  = buffer;
	input[i] = '\0';
	fclose(ifp);
	
	list = lexical_analyzer(input);
	if (list == NULL)
	{
		free(input);
		return 0;
	}
	code = parser_code_generator(list);
	if (code == NULL)
	{
		free(input);
		free(list);
		return 0;
	}
	
	virtual_machine(code);
	
	free(input);
	free(list);
	free(code);
	return 0;
}