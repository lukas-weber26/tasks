#include "./project.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

int lexer_log_level = 1;

void lexer_log(char *message, int level) {
	if (lexer_log_level > level) {
		printf("%s", message);
	}
}

typedef enum {AST_RETURN, AST_IF} ast_statement_type;

typedef struct ast_program {
	struct ast_function * function;	
} ast_program;

typedef struct ast_function {
	struct ast_identifier * identifier;	
	struct ast_statement ** statements;	
	int n_statements;
} ast_function;

typedef struct ast_return_statement_body {
	struct ast_expression * expression;	
} ast_return_statement_body;

typedef struct ast_if_statement_body {
	struct ast_expression * condition;	
	struct ast_statement * body;	
	struct ast_expression * alt_condition;	
	struct ast_statement * alt_body;	
} ast_if_statement_body;

typedef union ast_statement_body {
	ast_return_statement_body return_body;
	ast_if_statement_body if_body;
} ast_statement_body;

typedef struct ast_statement {
	ast_statement_type type;
	ast_statement_body body;	
} ast_statement;

typedef struct ast_expression {
	struct ast_integer * integer; 
} ast_expression;

typedef struct ast_identifier {
	char identifier_token[64];
} ast_identifier;

typedef struct ast_integer {
	long long int constant_token;
} ast_integer;

void print_depth(int depth);

void ast_integer_print(ast_integer * ast_int, int depth);
void ast_identifier_print(ast_identifier * ast_identifier, int depth);
void ast_expression_print(ast_expression * ast_expression, int depth);
void ast_statement_print(ast_statement * ast_statement, int depth);
void ast_function_print(ast_function * ast_function, int depth);
void ast_program_print(ast_program * ast_program);

ast_program * ast_program_create(token_list * tokens);
ast_function * ast_function_create(token_list * tokens, int * index);
ast_statement * ast_statement_create(token_list * tokens, int * index);
ast_expression * ast_expression_create(token_list * tokens, int * index);
ast_identifier * ast_identifier_create(token_list * tokens, int * index);
ast_integer * ast_integer_create(token_list * tokens, int * index);

void print_depth(int depth) {
	for (int i = 0; i < depth; i++) {
		printf("\t");
	}
}

void ast_program_print(ast_program * ast_program) {
	printf("PROGRAM:\n");	
	ast_function_print(ast_program->function, 1);
	printf("\n");
}

void ast_function_print(ast_function * ast_function, int depth) {
	print_depth(depth);	
	printf("FUNCTION:\n");
	depth ++;
	ast_identifier_print(ast_function->identifier, depth);
	for (int i = 0; i < ast_function->n_statements; i++) {
		ast_statement_print(ast_function->statements[i], depth);
	}
}

void ast_statement_print(ast_statement * ast_statement, int depth) {
	print_depth(depth);
	switch (ast_statement->type) {
		case AST_RETURN: 
			printf("RETURN STATEMENT: \n"); 
			ast_expression_print(ast_statement->body.return_body.expression, ++depth);
			break;
		case AST_IF: 
			printf("RETURN STATEMENT: \n"); 
			print_depth(depth ++);
			break;
		default : printf("ERROR\n"); exit(1);
	};

} 

void ast_expression_print(ast_expression * ast_expression, int depth) {
	print_depth(depth);
	printf("EXPRESSION TOKEN:\n");
	ast_integer_print(ast_expression->integer, ++depth);
} 

void ast_identifier_print(ast_identifier * ast_identifier, int depth) {
	print_depth(depth);
	printf("IDENTIFIER TOKEN: %s\n", ast_identifier->identifier_token);
} 

void ast_integer_print(ast_integer * ast_int, int depth) {
	print_depth(depth);
	printf("INT TOKEN: %lld\n", ast_int->constant_token);
} 

int expect(token_list * tokens, int * index, token_type type) {
	if (tokens->data[*index]->type == type) {
		(*index) ++;
		return 1;
	}
	return 0;
}

ast_program * ast_program_create(token_list * tokens) {
	lexer_log("AST program create\n", 0);
	
	ast_function * new_function = NULL;
	int index = 0;
	
	if ((new_function = ast_function_create(tokens, &index)) == NULL)
		printf("Failed to create program due to failiure to create function.\n");

	ast_program * new_program = calloc(1, sizeof(ast_program));
	new_program->function = new_function;
	return new_program;
}

ast_function * ast_function_create(token_list * tokens, int * index) {
	lexer_log("AST function create\n", 0);
	ast_identifier * new_identifier = NULL;
	ast_statement ** new_statements = NULL;
	int index_cpy = *index;

	if (!expect(tokens, &index_cpy, INT_KEYWORD))
		goto INT_KEYWORD_ERROR;

	if ((new_identifier = ast_identifier_create(tokens, &index_cpy)) == NULL)
		goto IDENTIFIER_ERROR;
	
	if (!expect(tokens, &index_cpy, OPEN_PAREN))
		goto PAREN_ERROR;

	expect(tokens, &index_cpy, VOID_KEYWORD);

	if (!expect(tokens, &index_cpy, CLOSE_PAREN))
		goto PAREN_ERROR;

	if (!expect(tokens, &index_cpy, OPEN_BRACE))
		goto OPEN_BRACE_ERROR;

	//statement
	int n_statements = 0, max_statements = 8;
	new_statements = calloc(max_statements, sizeof(ast_statement *));

	while ((new_statements[n_statements] = ast_statement_create(tokens, &index_cpy)) != NULL) {
		n_statements ++;
		if (n_statements == max_statements) {
			max_statements *= 2;
			new_statements = realloc(new_statements,  sizeof(ast_statement *) * max_statements);
		}
	}

	if (n_statements == 0)
		goto STATEMENT_ERROR;
	
	if (!expect(tokens, &index_cpy, CLOSE_BRACE))
		goto CLOSE_BRACE_ERROR;

	ast_function * new_function = calloc(1, sizeof(ast_function));
	new_function->identifier = new_identifier;
	new_function->statements = new_statements;
	new_function->n_statements = n_statements;
	*index = index_cpy;
	return new_function;

	CLOSE_BRACE_ERROR:
	for (int i = 0; i < n_statements; i ++) {
		//ast_statement_free(new_statements[i]);
	}
	STATEMENT_ERROR:
	OPEN_BRACE_ERROR:
	PAREN_ERROR:
	//ast_identifier_free(new_identifier);
	IDENTIFIER_ERROR:
	INT_KEYWORD_ERROR:

	return NULL;
}

ast_return_statement_body ast_return_statement_body_create(token_list * tokens, int * index, bool * success) {
	lexer_log("AST return statement create\n", 0);

	ast_return_statement_body new;
	int index_cpy = *index;

	ast_expression * new_expression = NULL;

	if (!expect(tokens, &index_cpy, RETURN_KEYWORD))
		goto RETURN_KEYWORD_ERROR;

	if ((new_expression = ast_expression_create(tokens, &index_cpy)) == NULL)
		goto EXPRESSION_ERROR;
	
	if (!expect(tokens, &index_cpy, SEMICOLON))
		goto SEMICOLON_ERROR;

	new.expression = new_expression;
	*index = index_cpy;
	*success = true;
	return new;
	
	SEMICOLON_ERROR:
	//ast_expression_free(new_expression);
	EXPRESSION_ERROR:
	RETURN_KEYWORD_ERROR:

	return new;
}

ast_if_statement_body ast_if_statement_body_create(token_list * tokens, int * index, bool * success) {
	lexer_log("AST if statement create\n", 0);
	ast_if_statement_body new;
	return new;
}

ast_statement * ast_statement_create(token_list * tokens, int * index) {
	lexer_log("AST statement create\n", 0);
	bool success = false;
	int index_cpy = *index;
	ast_statement * new_statement = NULL;

	ast_return_statement_body return_body = ast_return_statement_body_create(tokens, &index_cpy, &success);
	if (success) {
		new_statement = calloc(1, sizeof(ast_statement));
		new_statement->type = AST_RETURN;
		new_statement->body.return_body = return_body;
		*index = index_cpy;
		return new_statement;
	}
	
	ast_if_statement_body if_body = ast_if_statement_body_create(tokens, &index_cpy, &success);
	if (success) {
		new_statement = calloc(1, sizeof(ast_statement));
		new_statement->type = AST_IF;
		new_statement->body.if_body = if_body;
		*index = index_cpy;
		return new_statement;
	}

	return new_statement;
}

ast_expression * ast_expression_create(token_list * tokens, int * index) {
	lexer_log("AST expression create\n", 0);
	ast_integer * new_int = NULL;
	int index_cpy = *index;

	if ((new_int = ast_integer_create(tokens, &index_cpy)) == NULL)
		goto IDENTIFIER_ERROR;
	
	ast_expression * new_expression = calloc(1, sizeof(ast_expression));
	new_expression->integer = new_int;
	*index = index_cpy;
	return new_expression;

	IDENTIFIER_ERROR:
	printf("Failed to create statement due to inablility to parse integer.\n");
	return NULL;
}

ast_identifier * ast_identifier_create(token_list * tokens, int * index) {
	lexer_log("AST identifier create\n", 0);
	if (tokens->data[*index]->type == IDENTIFIER) {
		ast_identifier * new_identifier = calloc(1,sizeof(ast_identifier));
		strcpy(new_identifier->identifier_token,((identifier_data *)(tokens->data[*index]->data))->identifier);
		(*index) ++;
		return new_identifier;
	} else {
		printf("Expected an identifier but failed to recieve.\n");
		return NULL;
	}
}

ast_integer * ast_integer_create(token_list * tokens, int * index) {
	lexer_log("AST integer create\n", 0);
	if (tokens->data[*index]->type == CONSTANT) {
		ast_integer * new_integer = calloc(1,sizeof(ast_integer));
		new_integer->constant_token =  ((constant_data*)(tokens->data[*index]->data))->value;	
		(*index) ++;
		return new_integer;
	} else {
		printf("Expected an identifier but failed to recieve.\n");
		return NULL;
	}
}

//time to do the asm version of these
int asm_log_level = 1;

void asm_log(char *message, int level) {
	if (asm_log_level > level) {
		printf("%s", message);
	}
}

typedef enum {MOV, RET} asm_instruction_type;
typedef enum {IMM, REG} asm_operand_type;

typedef struct asm_program {
	struct asm_function * function;	
} asm_program;

typedef struct asm_function {
	char identifier[64];	
	struct asm_instruction ** instructions;	
	int n_instructions;
} asm_function;

typedef struct asm_mov_instruction_body {
	struct asm_operand * src;
	struct asm_operand * dest;
} asm_mov_instruction_body;

typedef union asm_instruction_body {
	asm_mov_instruction_body mov_body;
	bool no_body;	
} asm_instruction_body;

typedef struct asm_instruction {
	asm_instruction_type type;
	asm_instruction_body body;	
} asm_instruction;

typedef struct asm_operand {
	asm_operand_type type;
	long long int data;
} asm_operand;

long long int new_register() {
	static long long int reg = 0;
	return reg++;
}

//time to make this thing
void asm_program_print(asm_program * program); 
void asm_function_print(asm_function * function, int depth); 
void asm_instruction_print(asm_instruction * instruction, int depth); 
void asm_operand_print(asm_operand * operand, int depth); 

void asm_program_print(asm_program * program) {
	asm_log("ASM program print\n", 1);
	int depth = 1;
	printf("ASM PROGRAM:\n");
	asm_function_print(program->function, depth);
	printf("\n");
}

void asm_function_print(asm_function * function, int depth) {
	asm_log("ASM function print\n", 1);
	print_depth(depth);
	printf("ASM FUNCTION: %s\n", function->identifier);
	depth++;
	for (int i = 0; i < function->n_instructions; i++) {
		asm_instruction_print(function->instructions[i], depth);
	}
}

void asm_instruction_print (asm_instruction * instruction , int depth) {
	asm_log("ASM instruction print\n", 1);
	print_depth(depth++);
	printf("ASM_STATEMENT: \n");
	switch (instruction ->type) {
		case MOV: 
			print_depth(depth);
			printf("MOV ");
			asm_operand_print(instruction ->body.mov_body.src, depth);	
			asm_operand_print(instruction ->body.mov_body.dest, depth);	
			printf("\n");
			break;
		case RET: 
			print_depth(depth);
			printf("RET ");
			printf("\n");
			break;
		default: printf("Invalid asm statement type\n"); exit(0);
	}
}

void asm_operand_print(asm_operand * operand, int depth) {
	asm_log("ASM operand print\n", 1);
	if (operand->type == IMM) {
		printf("IMM %lld ", operand->data);
	} else if (operand->type == REG) {
		printf("REG %lld ", operand->data);
	}
} 

//time to transfer ast to asm 
asm_program * asm_program_create(ast_program * program);
asm_function * asm_function_create(ast_function * function);
asm_operand * asm_operand_create();

asm_program * asm_program_create(ast_program * program) {
	asm_log("ASM program create\n", 0);
	assert(program && program->function);
	asm_program * new_program = calloc(1, sizeof(asm_program));
	new_program->function = asm_function_create(program->function);
	return new_program;
}

//this is a lot because 1. There can be multiple input statements, 2. each statement can produce multiple instructions
//in response, this thing needs to take a vector (and possibly expand it), and insert instructions into it
//because this operation is a lot it also needs to update the index of the list being inserted into and update the vector size in case of a realloc
void asm_from_ast_return (asm_instruction ** instructions, int * max_instructions, int * starting_index, ast_statement * input_statement) {
	asm_log("ASM from ast return\n", 0);
	asm_instruction * mov = calloc(1, sizeof(asm_instruction)), * ret = calloc(1, sizeof(asm_instruction));	
	//not fond of how these things mix dynamic and stack allocations
	
	ret->type = RET;
	ret->body.no_body = 1;
	mov->type = MOV;
	mov->body.mov_body.src = calloc(1, sizeof(asm_operand));
	mov->body.mov_body.dest = calloc(1, sizeof(asm_operand));
	mov->body.mov_body.src->data = input_statement->body.return_body.expression->integer->constant_token;
	mov->body.mov_body.src->type = IMM;
	mov->body.mov_body.dest->data = new_register();
	mov->body.mov_body.dest->type = REG;
	
	if (*max_instructions >= *starting_index + 2) {
		(*max_instructions) *= 2;
		*instructions = realloc(*instructions, sizeof(asm_instruction*) * (*max_instructions));
	}

	instructions[*starting_index] = mov;
	instructions[(*starting_index) + 1] = ret;
	(*starting_index) += 2;
}

asm_function * asm_function_create(ast_function * function) {
	asm_log("ASM function create\n", 0);
	assert(function && function->identifier && function->statements && function->n_statements > 0);
	asm_function * new_function = calloc(1,sizeof(asm_function));

	int max_instructions= 8;
	strcpy(new_function->identifier, function->identifier->identifier_token);
	new_function->instructions = calloc(max_instructions, sizeof(asm_instruction *));
	int index = 0;

	for (int i = 0; i < function->n_statements; i++) {
		switch (function->statements[i]->type) {
			case AST_RETURN: 
				asm_from_ast_return(new_function->instructions, &max_instructions, &index, function->statements[i]);
				break;
			case AST_IF:
			default: 
				printf("Error, invalid statement\n");
				exit(0);
		}
	}	

	new_function->n_instructions = index;
	return new_function;
}

//code emission: These closely mirror the assembly print instructions except with less fluff
void asm_operand_emit(asm_operand * operand, FILE * fp);
void asm_instruction_emit(asm_instruction * instruction, FILE * fp);
void asm_function_emit(asm_function * function, FILE * fp);
void asm_program_emit(asm_program * program, FILE * fp);

void asm_program_emit(asm_program * program, FILE * fp) {
	fprintf(fp,"\t.globl main\n");
	asm_function_emit(program->function, fp);
	fprintf(fp,"\n.section .note.GNU-stack,\"\",@progbits\n");
}

void asm_function_emit(asm_function * function, FILE * fp) {
	fprintf(fp,"%s:\n", function->identifier);
	for (int i = 0; i < function->n_instructions; i++) {
		asm_instruction_emit(function->instructions[i], fp);
	}
}

void asm_instruction_emit(asm_instruction * instruction, FILE * fp) {
	switch (instruction ->type) {
		case MOV: 
			fprintf(fp,"\tmovl\t");
			asm_operand_emit(instruction ->body.mov_body.src, fp);	
			fprintf(fp,", ");
			asm_operand_emit(instruction ->body.mov_body.dest, fp);	
			fprintf(fp,"\n");
			break;
		case RET: 
			fprintf(fp,"\tret\n");
			break;
		default: printf("Invalid asm statement type\n"); exit(0);
	}
}

void asm_operand_emit(asm_operand * operand, FILE * fp) {
	if (operand->type == IMM) {
		fprintf(fp,"$%lld", operand->data);
	} else if (operand->type == REG) {
		fprintf(fp,"%%eax");
	}
} 

void lex(token_list * tokens) {
	token_list_print(tokens);
	ast_program * program = ast_program_create(tokens);
	ast_program_print(program);
	asm_program * asm_program = asm_program_create(program);
	asm_program_print(asm_program);
	asm_program_emit(asm_program, stdout);
}

