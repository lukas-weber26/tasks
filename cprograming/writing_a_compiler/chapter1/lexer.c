#include "./project.h"

token * token_match(token_list * list, int index, token tokens[]) {
	token_type program_form[] = {FUNCTION, COMPLETE};
	token_type function_form[] = {INT_KEYWORD, IDENTIFIER, OPEN_PAREN, VOID_KEYWORD, CLOSE_PAREN, OPEN_BRACE, STATEMENT, CLOSE_BRACE, COMPLETE}; 
	token_type statement_form[] = {RETURN_KEYWORD, EXPRESSION, COMPLETE};
	token_type expression_form[] = {INT_KEYWORD, COMPLETE};
	token_type identifier_form[] = {IDENTIFIER, COMPLETE};
	token_type int_form[] = {CONSTANT, COMPLETE};

	token_type * forms[6] = {program_form, function_form, statement_form, expression_form, identifier_form, int_form};

	for (int i = 0; i < 6; i++) {
		//try to match the list with a form, recursively call the form always keep track of your index
		//this will be a ouchie type function		
	}		
	return NULL;
}
