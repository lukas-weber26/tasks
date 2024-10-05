#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
	LPAREN, RPAREN, LBRACE, RBRACE, LSQUARE, RSQUARE, LANGLED, RANGLED, //bracket types	
	PLUS, MINUS, TIMES, DIVIDE, MOD, AND, OR, POWER, NOT, EQUAL,//arithmetic
	COMMA, COLON, SEMICOLON, ENDFILE,//punctuation
	FUNCTION, LET, IF, FOR, WHILE, RETURN, //keywords
	NUMBER, IDENTIFIER, STRING //vaiables 
} token_type;  

typedef struct token {
	token_type type;
	char * data_string;
	void * token_data;
} token;

typedef struct tokenizer {
	FILE * target_file;
	char * current_token_string;
	int max_string_size;
	int current_string_size;
	token ** tokens; 
	int token_count; 
	int max_tokens;
} tokenizer;

token * token_create(token_type type, char * token_string){
	token * new_token = calloc(1, sizeof(token));
	new_token -> type = type;
	new_token -> data_string = strdup(token_string);
	new_token -> token_data = NULL;
	return new_token;
}

void token_free(token * token) {
	free(token->data_string);
	free(token);
}

tokenizer * tokenizer_create(char * file_name, int max_tokens){
	tokenizer * new_tokenizer = calloc(1, sizeof(tokenizer));
	new_tokenizer->target_file = fopen(file_name, "r");
	if (new_tokenizer->target_file == NULL || max_tokens == 0){
		printf("File %s could not be read or invalid max tokens. Exiting.\n",file_name);
		free(new_tokenizer);
		exit(0);
	}
	new_tokenizer->max_string_size= 256; 
	new_tokenizer->current_string_size= 0; 
	new_tokenizer->current_token_string = calloc(new_tokenizer->max_string_size, sizeof(char));
	new_tokenizer->token_count = 0;
	new_tokenizer->max_tokens = max_tokens;
	new_tokenizer->tokens= calloc(new_tokenizer->max_tokens, sizeof(token *));
	return new_tokenizer;
}

void tokenizer_free(tokenizer * tokenizer){
	fclose(tokenizer->target_file);
	free(tokenizer->current_token_string);
	for (int i = 0; i< tokenizer->token_count; i++){
		token_free(tokenizer->tokens[i]);
		tokenizer->tokens[i] = NULL;	
	}
	free(tokenizer->tokens);
}

void tokenizer_test_validity(tokenizer * tokenizer){
	if (tokenizer->max_tokens < tokenizer->token_count) {
		printf("Invalid tokenizer. Max tokens less than token count. Exiting.\n");	
		exit(0);
	}

	for (int i = 0; i < tokenizer->token_count; i++) {
		for (int j = 0; j < tokenizer->token_count; j++) {
			if (i != j && tokenizer->tokens[i] == tokenizer->tokens[j]) {
				printf("Invalid tokenizer. Repeated token. Exiting.\n");	
				exit(0);
			}
		}	
	}	

	for (int i = tokenizer->token_count; i < tokenizer->max_tokens; i++){
		if (tokenizer->tokens[i] != NULL) {
			printf("Invalid tokenizer. Non-null tokens past reported token count. Exiting.\n");	
			exit(0);
		}
	}	

	if (tokenizer->current_token_string == NULL){
		printf("Invalid tokenizer. Current token string is NULL. Exiting.\n");	
		exit(0);
	}
}

void tokenizer_check_and_resize(tokenizer * tokenizer){
	if(tokenizer->max_tokens== tokenizer->token_count){
		tokenizer->max_tokens*= 2;
		tokenizer->tokens = realloc(tokenizer->tokens, tokenizer->max_tokens*sizeof(token *));
	}		
}

	//COMMA, COLON, SEMICOLON, ENDFILE,//punctuation
	//FUNCTION, LET, IF, FOR, WHILE, RETURN, //keywords
	//NUMBER, IDENTIFIER, STRING //vaiables 

void print_type(token_type type, FILE * print_destination){
	switch(type) {
		case LPAREN: fprintf(print_destination, "("); break;
		case RPAREN: fprintf(print_destination, ")"); break;
		case LBRACE: fprintf(print_destination, "{"); break;
		case RBRACE: fprintf(print_destination, "}"); break;
		case LSQUARE: fprintf(print_destination, "["); break;
		case RSQUARE: fprintf(print_destination, "]"); break;
		case LANGLED: fprintf(print_destination, "<"); break;
		case RANGLED: fprintf(print_destination, ">"); break;
		case PLUS: fprintf(print_destination, "+"); break;
		case MINUS: fprintf(print_destination, "-"); break;
		case TIMES: fprintf(print_destination, "*"); break;
		case DIVIDE: fprintf(print_destination, "/"); break;
		case MOD: fprintf(print_destination, "%%"); break;
		case AND: fprintf(print_destination, "&"); break;
		case OR: fprintf(print_destination, "|"); break;
		case POWER: fprintf(print_destination, "^"); break;
		case NOT: fprintf(print_destination, "!"); break;
		case EQUAL: fprintf(print_destination, "="); break;
		case COMMA: fprintf(print_destination, ","); break;
		case COLON: fprintf(print_destination, ":"); break;
		case SEMICOLON: fprintf(print_destination, ";"); break;
		case ENDFILE: fprintf(print_destination, "EOF"); break;
		case FUNCTION: fprintf(print_destination, "FUNCTION"); break;
		case LET: fprintf(print_destination, "LET"); break;
		case IF: fprintf(print_destination, "IF"); break;
		case FOR: fprintf(print_destination, "FOR"); break;
		case WHILE: fprintf(print_destination, "WHILE"); break;
		case RETURN: fprintf(print_destination, "RETURN"); break;
		case NUMBER: fprintf(print_destination, "NUMBER"); break;
		case IDENTIFIER: fprintf(print_destination, "IDENTIFIER"); break;
		case STRING: fprintf(print_destination, "STRING"); break;
		default: printf("Invalid type.\n"); exit(0); break;
	}
}

void token_print(token * print_token, FILE * print_destination){
	if (print_destination == NULL) {
		print_destination = stdout;
	}
	if (print_token->type == NUMBER || print_token->type == STRING || print_token->type == IDENTIFIER) {
		fprintf(print_destination,"Type: ");
		print_type(print_token->type, print_destination);
		fprintf(print_destination, " string: %s.\n", print_token->data_string);
	} else {
		fprintf(print_destination,"Type: ");
		print_type(print_token->type, print_destination);
		fprintf(print_destination, " \n");
	}	
};

void tokenizer_print(tokenizer * print_tokenizer, FILE * print_destination){
	if (print_destination == NULL) {
		print_destination = stdout;
	}

	fprintf(print_destination,"Tokenizer stats: max tokens %d, current tokens %d.\n\n",print_tokenizer->max_tokens, print_tokenizer->token_count);

	for (int i = 0; i< print_tokenizer->token_count; i++){
		token_print(print_tokenizer->tokens[i], print_destination);
	}
};

void tokenizer_add_token(tokenizer * tokenizer, char * token_string, token_type type){
	tokenizer_check_and_resize(tokenizer);	
	if (token_string != NULL) {
		tokenizer->tokens[tokenizer->token_count] = token_create(type, token_string); 
		tokenizer->token_count += 1;
	} else if (tokenizer->current_token_string != NULL){
		tokenizer->tokens[tokenizer->token_count] = token_create(type, tokenizer->current_token_string); 
		tokenizer->token_count += 1;
	}
}

void tokenizer_eat_word(tokenizer * tokenizer, char first_char) {
	int length = 1;
	tokenizer->current_token_string[length-1] = first_char;
	int read_char;

	while ((read_char = fgetc(tokenizer->target_file)) != EOF) {
		if (read_char == ' ' || read_char == '\n' || read_char == '\t') {
			break;
		}

		if (tokenizer->max_string_size- 2 == length) {
			printf("Toeknization warning: something probably went wrong in tokenizer_eat_word!\n");
			exit(0);
			break;
		}
		
		tokenizer->current_token_string[length] = read_char;	
		length ++;	
	}
	tokenizer->current_token_string[length] = '\0';	
}

void tokenizer_eat_string(tokenizer * tokenizer, char first_char) {
	tokenizer->current_string_size= 1;
	tokenizer->current_token_string[tokenizer->current_string_size-1] = first_char;
	int read_char;

	while ((read_char = fgetc(tokenizer->target_file)) != EOF) {

		if (tokenizer->max_string_size- 2 == tokenizer->current_string_size) {
			printf("Toeknization warning: something probably went wrong in tokenizer_eat_word!\n");
			exit(0);
			break;
		}
		
		tokenizer->current_token_string[tokenizer->current_string_size] = read_char;	
		tokenizer->current_string_size++;	
		
		if (read_char == '"') {
			break;
		}

	}
	tokenizer->current_token_string[tokenizer->current_string_size] = '\0';	
}

int tokenizer_is_number(tokenizer * tokenizer) {
	int n_periods = 0;
	for (int i = 0; i < tokenizer-> current_string_size; i++) {
		if (tokenizer->current_token_string[i] == '.')	{
			n_periods++;
			if (n_periods > 1) {
				return 0;
			}
		} else if (tokenizer->current_token_string[i] < '0' || tokenizer->current_token_string[i] > '9') {
			return 0;
		}
	}
	return 1;
}

void tokenizer_get_next_token(tokenizer * tokenizer){
	int current_char = fgetc(tokenizer->target_file);
	switch (current_char) {
		case ' ':
		case '\n':
		case '\t':
			tokenizer_get_next_token(tokenizer); //in case of whitespace, just skip it...
			break;
		case '(': tokenizer_add_token(tokenizer, "(", LPAREN);break;
		case ')': tokenizer_add_token(tokenizer, ")", RPAREN);break;
		case '{': tokenizer_add_token(tokenizer, "{", LBRACE);break;
		case '}': tokenizer_add_token(tokenizer, "}", RBRACE);break;
		case '[': tokenizer_add_token(tokenizer, "[", LSQUARE);break;
		case ']': tokenizer_add_token(tokenizer, "]", RSQUARE);break;
		case '<': tokenizer_add_token(tokenizer, "<", LANGLED);break;
		case '>': tokenizer_add_token(tokenizer, ">", RANGLED);break;
		case '+': tokenizer_add_token(tokenizer, "+", PLUS);break;
		case '-': tokenizer_add_token(tokenizer, "-", MINUS);break;
		case '*': tokenizer_add_token(tokenizer, "*", TIMES);break;
		case '/': tokenizer_add_token(tokenizer, "/", DIVIDE);break;
		case '%': tokenizer_add_token(tokenizer, "%", MOD);break;
		case '&': tokenizer_add_token(tokenizer, "&", AND);break;
		case '^': tokenizer_add_token(tokenizer, "^", POWER);break;
		case '!': tokenizer_add_token(tokenizer, "!", NOT);break;
		case '=': tokenizer_add_token(tokenizer, "=", EQUAL);break;
		case ',': tokenizer_add_token(tokenizer, ",", COMMA);break;
		case ':': tokenizer_add_token(tokenizer, ":", COLON);break;
		case ';': tokenizer_add_token(tokenizer, ";", SEMICOLON);break;
		case '"': 
			tokenizer_eat_string(tokenizer, current_char);
			tokenizer_add_token(tokenizer, NULL, STRING);
			break;
		case EOF: tokenizer_add_token(tokenizer, "EOF", ENDFILE);break;
		default:
			tokenizer_eat_word(tokenizer, current_char);
			if (strcmp(tokenizer->current_token_string, "function") == 0) {
				tokenizer_add_token(tokenizer,NULL, FUNCTION);break;
			} else if (strcmp(tokenizer->current_token_string, "let") == 0) {
				tokenizer_add_token(tokenizer,NULL, LET);break;
			}else if (strcmp(tokenizer->current_token_string, "if") == 0) {
				tokenizer_add_token(tokenizer,NULL , IF);break;
			} else if (strcmp(tokenizer->current_token_string, "for") == 0) {
				tokenizer_add_token(tokenizer,NULL , FOR);break;
			}else if (strcmp(tokenizer->current_token_string, "while") == 0) {
				tokenizer_add_token(tokenizer,NULL , WHILE);break;
			}else if (strcmp(tokenizer->current_token_string, "return") == 0) {
				tokenizer_add_token(tokenizer,NULL , RETURN);break;
			} else if (tokenizer_is_number(tokenizer)){
				tokenizer_add_token(tokenizer,NULL , NUMBER);break;
			} else {
				tokenizer_add_token(tokenizer,NULL , IDENTIFIER);break;
			} 
	}
}

//typedef enum {
//	LPAREN, RPAREN, LBRACE, RBRACE, LSQUARE, RSQUARE, LANGLED, RANGLED, //bracket types	
//	PLUS, MINUS, TIMES, DIVIDE, MOD, AND, OR, POWER, NOT, EQUAL,//arithmetic
//	COMMA, COLON, SEMICOLON, ENDFILE,//punctuation
//	FUNCTION, LET, IF, FOR, WHILE, RETURN, //keywords
//	NUMBER, IDENTIFIER, STRING //vaiables 
//} token_type;  

//void tokenizer_test_tokenizer() {
//	
//	token expected_tokens[] = {
//		{LPAREN, "(", NULL},{RPAREN, ")"}
//	};
//
//	tokenizer * tok = tokenizer_create("tokenizer_test.txt",10);
//	for (int i = 0; i < 1; i++){
//		
//	}
//
//	tokenizer_free(tok);
//}

void tokenizer_tokenize_whole_file(tokenizer * tokenizer) {
	tokenizer_get_next_token(tokenizer);
	while (tokenizer->tokens[tokenizer->token_count]->type != ENDFILE) {
		tokenizer_get_next_token(tokenizer);
	}
}

void tokenizer_basic_test() {
	tokenizer * tok = tokenizer_create("tokenizer_test.txt",10);
	tokenizer_tokenize_whole_file(tok);
	tokenizer_print(tok, NULL);
	tokenizer_free(tok);
}

int main(){
	//tokenizer_test_tokenizer();
	tokenizer_basic_test();
	exit(0);
}
