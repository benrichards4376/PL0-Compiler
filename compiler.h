#define MAX_ARRAY_SIZE 500

typedef enum token_type {
	elsesym = 1, identsym, numbersym, plussym, minussym,
	multsym, slashsym, modsym, eqlsym, neqsym,
	lessym, leqsym, gtrsym, geqsym, lparentsym,
	rparentsym, commasym, semicolonsym, periodsym, becomessym,
	beginsym, endsym, ifsym, thensym, whilesym, 
	dosym, callsym, constsym, varsym, procsym, 
	writesym, readsym, addassignsym, subassignsym, timesassignsym,
	divassignsym, modassignsym
} token_type;

typedef struct lexeme {
	token_type type;
	char name[12];
	int value;
	int error_type;
} lexeme;

typedef struct instruction {
	int op;
	int l;
	int m;
} instruction;

typedef struct symbol {
	int kind;
	char name[12];
	int value;
	int level;
	int addr;
	int mark;
} symbol;

lexeme *lexical_analyzer(char *input);
instruction *parser_code_generator(lexeme *list);
void virtual_machine(instruction *code);