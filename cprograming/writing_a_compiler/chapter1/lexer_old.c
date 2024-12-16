#include "./project.h"

typedef enum {AST_PROGRAM, AST_FUNCTION,AST_STATEMENT, AST_EXPRESSION, AST_IDENTIFIER, AST_INT} ast_token_type;

typedef struct ast_token {
	ast_token_type type;
	void ** children;	
} ast_token;

void print_depth(int depth) {
	while (depth-- > 0) {
		printf(" ");
	}	
}

void internal_ast_token_print(ast_token * token, int depth);

void ast_token_print_program(ast_token * token, int depth) {
	print_depth(depth);
	printf("PROGRAM:\n");
	internal_ast_token_print(token->children[0], ++depth);	
}

void ast_token_print_function(ast_token * token, int depth) {
	print_depth(depth);
	printf("FUNCTION:\n");
	++depth;
	internal_ast_token_print(token->children[0], depth);	
	internal_ast_token_print(token->children[1], depth); 
}

void ast_token_print_statement(ast_token * token, int depth) {
	print_depth(depth);
	printf("RETURN STATEMENT\n");
	internal_ast_token_print(token->children[0], ++depth);	
}

void ast_token_print_expression(ast_token * token, int depth) {
	print_depth(depth);
	printf("EXPRESSION\n");
	internal_ast_token_print(token->children[0], ++depth);	
}

void ast_token_print_identifier(ast_token * token, int depth) {
	print_depth(depth);
	printf("IDENTIFIER: %s\n", ((identifier_data *)token->children[0])->identifier);
}

void ast_token_print_int(ast_token * token, int depth) {
	print_depth(depth);
	printf("INT: %lld\n", ((constant_data*)token->children[0])->value);
}

void internal_ast_token_print(ast_token * token, int depth) {
	switch (token->type) {
		case AST_PROGRAM: ast_token_print_program(token, depth); return;
		case AST_FUNCTION: ast_token_print_function(token, depth); return;
		case AST_STATEMENT: ast_token_print_statement(token, depth); return;
		case AST_EXPRESSION: ast_token_print_expression(token, depth); return;
		case AST_IDENTIFIER: ast_token_print_identifier(token, depth); return;
		case AST_INT: ast_token_print_int(token, depth); return;
		default: printf("Invalid ast token type.\n"); exit(0);
	}
}

void ast_token_print(ast_token * token) {
	internal_ast_token_print(token, 0);	
}

int ast_token_type_n_children(ast_token_type type) {
	switch (type) {
		case AST_PROGRAM: return 1;	
		case AST_FUNCTION: return 2;	
		case AST_STATEMENT: return 1;	
		case AST_EXPRESSION: return 1;	
		case AST_IDENTIFIER: return 1;	
		case AST_INT: return 1;	
		default: printf("Invalid ast token type.\n"); exit(0);
	}
}

//would bre cool to make this be capable of multiple steps
int token_list_simple_expect(token_list * tokens, int *index, token_type type) {
	if (tokens->data[*index]->type != type) {
		return 0;
	}
	(*index)++;
	return 1;
}

ast_token * ast_token_alloc(ast_token_type type) {
	ast_token * t = calloc(1, sizeof(ast_token));
	t->type = type;
	t->children = calloc(ast_token_type_n_children(type),sizeof(void *));
	return t;
}

void ast_token_free(ast_token * t) {
	switch (t->type) {
		case AST_PROGRAM: break;	
		case AST_FUNCTION: break;	
		case AST_STATEMENT: ast_token_free(t->children[0]); break;	
		case AST_EXPRESSION: ast_token_free(t->children[0]); break;	
		case AST_IDENTIFIER: break;	
		case AST_INT: break;	
		default: printf("Invalid ast token type.\n"); exit(0);
	}
	free(t);
}

#define token_require(a,b) if (a == 0) goto b;
#define ast_require(a,b) if (a == NULL) goto b;
#define token_maybe(a,b) if (a == 0) goto b;

//get just return and expr to work 
ast_token * ast_token_match_int_token(token_list * tokens, int * index) {
	ast_token * new = NULL;
	if (tokens->data[*index]->type == CONSTANT) {
		new = ast_token_alloc(AST_INT);
		new->children[0] = tokens->data[*index]->data;
		(*index)++;
	}
	return new;
}

ast_token * ast_token_match_identifier_token(token_list * tokens, int * index) {
	ast_token * new = NULL;
	if (tokens->data[*index]->type == IDENTIFIER) {
		new = ast_token_alloc(AST_IDENTIFIER);
		new->children[0] = tokens->data[*index]->data;
		(*index)++;
	}
	return new;
}

ast_token * ast_token_match_expression(token_list * tokens, int * index) {
	ast_token * new = NULL; 
	ast_token * new_int = ast_token_match_int_token(tokens, index);
	if (new_int != NULL) {
		new = ast_token_alloc(AST_EXPRESSION);
		new->children[0] = new_int;
	}
	return new;
}

ast_token * ast_token_match_statement(token_list * tokens, int * index) {
	ast_token * new_expression = NULL;
	int index_cpy = *index;
	
	token_require(token_list_simple_expect(tokens, &index_cpy, RETURN_KEYWORD), ERROR_RETURN);
	ast_require((new_expression = ast_token_match_expression(tokens, &index_cpy)), ERROR_EXPRESSION);
	token_require(token_list_simple_expect(tokens, &index_cpy, SEMICOLON), ERROR_SEMICOLON);

	*index = index_cpy;
	ast_token * new = ast_token_alloc(AST_STATEMENT);
	new->children[0] = new_expression;
	return new;

	ERROR_SEMICOLON:
	ast_token_free(new_expression);
	ERROR_EXPRESSION:
	ERROR_RETURN:
	return NULL;	
}

ast_token * ast_token_match_function(token_list * tokens, int * index) {
	ast_token * new_identifier = NULL, *new_statement = NULL;
	int index_cpy = *index;
	
	token_require(token_list_simple_expect(tokens, &index_cpy, INT_KEYWORD), ERROR_FUNCTION_TYPE);
	ast_require((new_identifier = ast_token_match_identifier_token(tokens, &index_cpy)), ERROR_FUNCTION_NAME);
	token_require(token_list_simple_expect(tokens, &index_cpy, OPEN_PAREN), ERROR_OPEN_PAREN);
	token_list_simple_expect(tokens, &index_cpy, VOID_KEYWORD); 
	token_require(token_list_simple_expect(tokens, &index_cpy, CLOSE_PAREN), ERROR_CLOSE_PAREN);
	token_require(token_list_simple_expect(tokens, &index_cpy, OPEN_BRACE), ERROR_OPEN_BRACE);
	ast_require((new_statement = ast_token_match_statement(tokens, &index_cpy)), ERROR_FUNCTION_BODY);
	token_require(token_list_simple_expect(tokens, &index_cpy, CLOSE_BRACE), ERROR_CLOSE_BRACE);

	*index = index_cpy;
	ast_token * new = ast_token_alloc(AST_FUNCTION);
	new->children[0] = new_identifier;
	new->children[1] = new_statement;
	return new;

	ERROR_CLOSE_BRACE:
	ast_token_free(new_statement);
	ERROR_FUNCTION_BODY:
	ERROR_OPEN_BRACE:
	ERROR_CLOSE_PAREN:
	ERROR_OPEN_PAREN:
	ast_token_free(new_identifier);
	ERROR_FUNCTION_NAME:
	ERROR_FUNCTION_TYPE:

	return NULL;
}

ast_token * ast_token_match_program(token_list * tokens) {
	ast_token * new_function = NULL;
	int index = 0;
	
	ast_require((new_function = ast_token_match_function(tokens, &index)), ERROR_FUNCTION);

	ast_token * new = ast_token_alloc(AST_PROGRAM);
	new->children[0] = new_function;
	return new;

	ERROR_FUNCTION:
	printf("Failed to create program.\n");
	exit(0);
}

void lex(token_list * tokens) {
	token_list_print(tokens);
	ast_token * program = ast_token_match_program(tokens);
	ast_token_print(program);
}


typedef enum {ASM_PROGRAM, ASM_FUNCTION, ASM_INSTRUCTIONS, ASM_OPERAND} asm_token_type;
typedef enum {MOV, RET} asm_instruction_type;
typedef enum {IMMEDIATE, REGISTER} asm_operand_type; 

typedef struct asm_token {
	asm_token_type type;
	void * data;
} asm_token;

typedef struct asm_program_data {
	asm_token * function_defenition;	
} asm_program_data;

typedef struct asm_function_data {
	char identifier_name[64];
	asm_token ** instructions;
	int instruction_count;
} asm_function_data;

typedef struct asm_instruction_data {
	asm_instruction_type type;
	asm_token * operands[2];
} asm_instruction_data;

typedef struct asm_operand_data {
	asm_operand_type type;
	long long int value;
} asm_operand_data;

asm_token * asm_token_alloc() {
	asm_token * new_token = calloc(1, sizeof(asm_token));
	new_token->data = NULL;
	return new_token;
}

asm_token * asm_tree_create(ast_token * token);

asm_token * asm_create_program(ast_token * token) {
	asm_token * new = asm_token_alloc();
	new->data = calloc(1, sizeof(asm_program_data));
	((asm_program_data *)(new->data))->function_defenition = asm_create_program((token->children)[0]);
	return new;
}; 

asm_token * asm_create_function(ast_token * token) {
	asm_token * new = asm_token_alloc();
	new->data = calloc(1, sizeof(asm_function_data));
	strcpy(((identifier_data *)(token->children[0]))->identifier, ((asm_function_data*)(new->data))->identifier_name);

	int instruction_count = 1;
	((asm_function_data*)(new->data))->instruction_count = instruction_count;
	((asm_function_data*)(new->data))->instructions = calloc(instruction_count, sizeof(void *));
	((asm_function_data*)(new->data))->instructions[0] = asm_tree_create(token->children[1]);

	return new;
};

asm_token * asm_create_statement(ast_token * token) {
	asm_token * new = asm_token_alloc();
	new->data = expression_dat
};

asm_token * asm_create_expression(ast_token * token) {

};

asm_token * asm_create_identifier(ast_token * token) {

};

asm_token * asm_create_int(ast_token * token) {

};

asm_token * asm_tree_create(ast_token * token) {
	switch (token->type) {
		case AST_PROGRAM: return asm_create_program(token); 
		case AST_FUNCTION: return asm_create_function(token); 
		case AST_STATEMENT: return asm_create_statement(token); 
		case AST_EXPRESSION: return asm_create_expression(token); 
		case AST_IDENTIFIER: return asm_create_identifier(token);
		case AST_INT: return asm_create_int(token); 
	}
}


