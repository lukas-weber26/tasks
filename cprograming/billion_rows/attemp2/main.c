#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	char * string;
	int count; 
	double total;
	double min;
	double max;
	struct node * next_node;
} node; 

typedef struct map {
	node * map_body[19683];	
} map;

node * new_node (char * string, double val) {
	node * n = malloc(sizeof(node));
	n -> count = 1;
	n -> total = val;
	n -> min = val;
	n-> max = val;
	n->next_node = NULL;
	return n;
}

map * new_map () {
	map * m = calloc(1,sizeof(map));
	return m;
}

int char_hash(char s) {
	if (s >= 65 && s <= 90 ) {
		return  s - 65;
	} else if (s >= 97 && s <= 122) {
		return s - 97; 
	} else {
		return 26;
	}
}

int get_hash(int first, int second, int third) {
	return first * 26 * 26 + second * 26 + third;
}

int string_hash(char * s) {
	int first;
	int second; 
	int third; 

	if (*s == '\0') {
		first = 0; 
		second = 0; 
		third = 0;
	} else {
		first = char_hash(*s);
		return get_hash(first, 0, 0);
	}

	if (*(s+1) == '\0') {
		second = 0; 
		third = 0;
	} else {
		second = char_hash(*(s+1));
		return get_hash(first, second, 0);
	}
	
	if (*(s+2) == '\0') {
		third = 0; 
	} else {
		third = char_hash(*(s+2));
	}
	
	return get_hash(first, second, third);
}

void insert_string(char * s, double v, map * m ) {
	printf("Inserting string\n");
	int hash = string_hash(s);
	printf("Found hash: %d\n", hash);
	node *n = m->map_body[string_hash(s)];

	if (n) {
		while (n -> next_node != NULL) {
			if (strcmp(s, n->string) == 0)	{
				n->total += v;
				n->count ++;
				n -> min = v < n->min ? v : n->min;
				n -> max = v > n->max? v : n->max;
				return;
			}
			n = n -> next_node;
		}

		if (strcmp(s, n->string) == 0)	{
			n->total += v;
			n->count ++;
			n -> min = v < n->min ? v : n->min;
			n -> max = v > n->max? v : n->max;
			return;
		}

		n -> next_node = new_node(s, v);
		return;
	} else {
		m->map_body[string_hash(s)] = new_node(s, v);
	}
}

void print_map(map * m) {
	for (int i = 0; i < 19683; i++) {
		if (m->map_body[i] != 0) {
			printf("printing\n");
			node * n = m->map_body[i];
			printf("%s\n", n->string);
			//while (n) {
			//	printf("%s\n",n->string);
			//	n = n -> next_node;
			//}
		}
	}	
}

int main() {
	//debug time 
	map * m = new_map();
	char *test_string = malloc(sizeof(char) * 6);
	memcpy(test_string, "Test1",6);
	insert_string(test_string, 12, m);
	print_map(m); }
