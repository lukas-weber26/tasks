#include "./project.h"

token * get_identifier_token(token_type type) {
	token * new = calloc(1, sizeof(token));
	new->type = type;
	new->data = NULL;
	return new;
}

token * emit_identifier_token(char * identifier) {
	token * new = get_identifier_token(IDENTIFIER);
	new->data = calloc(1,sizeof(identifier_data));
	strncpy(((identifier_data *)new->data)->identifier, identifier, 64);
	return new;
}

token * emit_constant_token(long long int constant) {
	token * new = get_identifier_token(CONSTANT);
	new->data = calloc(1,sizeof(constant_data));
	((constant_data *)new->data)->value = constant;
	return new;
}

token * emit_int_keyword_token() {
	token * new = get_identifier_token(INT_KEYWORD);
	return new;
}

token * emit_void_keyword_token() {
	token * new = get_identifier_token(VOID_KEYWORD);
	return new;
}

token * emit_return_keyword_token() {
	token * new = get_identifier_token(RETURN_KEYWORD);
	return new;
}

token * emit_open_paren_token() {
	token * new = get_identifier_token(OPEN_PAREN);
	return new;
}

token * emit_close_paren_token() {
	token * new = get_identifier_token(CLOSE_PAREN);
	return new;
}

token * emit_open_brace_token() {
	token * new = get_identifier_token(OPEN_BRACE);
	return new;
}

token * emit_close_brace_token() {
	token * new = get_identifier_token(CLOSE_BRACE);
	return new;
}

token * emit_semicolon_token() {
	token * new = get_identifier_token(SEMICOLON);
	return new;
}

void token_print(token * t) {
	switch (t->type) {
		case IDENTIFIER: printf("%s", ((identifier_data *)(t->data))->identifier); break; 
		case CONSTANT: printf("%lld", ((constant_data *)(t->data))->value); break; 
		case INT_KEYWORD: printf("int"); break; 
		case VOID_KEYWORD: printf("void"); break;
		case RETURN_KEYWORD: printf("return"); break;
		case OPEN_PAREN: printf("("); break; 
		case CLOSE_PAREN:  printf(")"); break;
		case OPEN_BRACE:  printf("{"); break;
		case CLOSE_BRACE: printf("}"); break;
		case SEMICOLON: printf(";"); break;
		default: printf("Unexpected token type. Exiting.\n"); exit(0);
	}
}

token_list * token_list_create(int len) {
	token_list * l = calloc(1, sizeof(token_list));
	l->cur = 0;
	l->max= len;
	l->data = calloc(l->max, sizeof(token *));
	return l;
}

void token_list_push(token_list * tl, token * t) {
	if (t == NULL) {
		return;
	}
	tl->data[tl->cur] = t;
	tl->cur ++;
	if (tl->cur == tl->max) {
		tl->max *= 2;
		tl->data = realloc(tl->data, tl->max*sizeof(token *));
	}
}

void token_list_print(token_list * tl) {
	printf("Token list:\n");
	for (int i = 0; i < tl->cur; i++) {
		token_print(tl->data[i]);
		printf("\n");
	}
	printf("\n");
}

int is_whitespace(char c) {
	if (c == ' ' || c == '\n' || c == '\t') {
		return 1;
	}
	return 0;
}

int is_number(char * c, int count) {
	for (int i = 0; i < count; i++) {
		if (!isdigit(c[i])) {
			return 0;	
		}
	}
	return 1;
}

int is_identifier(char * c, int count) {
	if (!(*c == '_' || isalpha(*c))) {
		return 0;
	}

	for (int i = 1; i < count; i++) {
		if (!(isalnum(c[i]) || c[i] == '_')) {
			return 0;	
		}
	}
	return 1;
}

token * emit_word(char * word, int * index) {
	if (*index == 0) {
		return NULL;
	}

	word[*index] = '\0';

	token * return_token;

	if (strcmp(word, "int") == 0) {
		return_token = emit_int_keyword_token(); 
	} else if (strcmp(word, "void") == 0) {
		return_token = emit_void_keyword_token(); 
	} else if (strcmp(word, "return") == 0) {
		return_token = emit_return_keyword_token(); 
	} else if (is_number(word, *index)) {
		return_token = emit_constant_token(atoll(word)); 
	} else if (is_identifier(word, *index)) {
		return_token = emit_identifier_token(word); 
	} else if (word[0] == '\0') {
		NULL;
	} else {
		printf("Error. Token %s is invalid.\n", word);
		exit(0);	
	}

	(*index) = 0;
	return return_token;
}

token_list * tokenize (FILE * file) {
	int input, index = 0;
	char word[64];
	token_list * tokens = token_list_create(64);

	while ((input = fgetc(file)) != -1) {
		if (is_whitespace(input)) {
			token_list_push(tokens,emit_word(word, &index));
			continue;
		}
		
		switch (input) {
			case '(': 
				token_list_push(tokens,emit_word(word, &index));
				token_list_push(tokens,emit_open_paren_token());
				break;
			case ')': 
				token_list_push(tokens,emit_word(word, &index));
				token_list_push(tokens,emit_close_paren_token());
				break;
			case '{': 
				token_list_push(tokens,emit_word(word, &index));
				token_list_push(tokens,emit_open_brace_token());
				break;
			case '}': 
				token_list_push(tokens,emit_word(word, &index));
				token_list_push(tokens,emit_close_brace_token());
				break;
			case ';': 
				token_list_push(tokens,emit_word(word, &index));
				token_list_push(tokens,emit_semicolon_token());
				break;
			default: 
				word[index] = input;
				index ++;
				break;
		}	

		if (index > 63) {
			printf("Error, maximum identifier length exceeded.\n");
			exit(1);
		}

	}
	token_list_print(tokens);
	return tokens;
}
