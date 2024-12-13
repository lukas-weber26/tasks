#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef enum {LEX, PARSE, CODEGEN, ASSEMBLY, FULL} compiler_mode;
typedef enum {IDENTIFIER, CONSTANT, INT_KEYWORD, VOID_KEYWORD, RETURN_KEYWORD, OPEN_PAREN, CLOSE_PAREN, OPEN_BRACE, CLOSE_BRACE, SEMICOLON} token_type; //added complete as a general nan 
//tokenizer tokens and more advanced lexer tokens are mixed. why? because these types need to shade a potential list for matching to occur

typedef struct token {
	token_type type;
	void * data;
} token; 

typedef struct identifier_data {
	char identifier[64];	
} identifier_data;

typedef struct constant_data {
	long long int value;	
}constant_data;

typedef struct token_list {
	token ** data;
	int cur;
	int max;
} token_list;

//tokenization
token * get_identifier_token(token_type type);
token * emit_identifier_token(char * identifier);
token * emit_constant_token(long long int constant);
token * emit_int_keyword_token();
token * emit_void_keyword_token();
token * emit_return_keyword_token();
token * emit_open_paren_token();
token * emit_close_paren_token();
token * emit_open_brace_token();
token * emit_close_brace_token();
token * emit_semicolon_token();
void token_print(token * t);
token_list * token_list_create(int len);
void token_list_push(token_list * tl, token * t);
void token_list_print(token_list * tl);
int is_whitespace(char c);
int is_number(char * c, int count);
int is_identifier(char * c, int count);
token * emit_word(char * word, int * index);
token_list * tokenize (FILE * file);

//lexing
void lex(token_list * tokens);

