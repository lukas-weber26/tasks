#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef struct node {
	char * key;
	int val;
	struct node * next;
} node;

int char_hash(char c) {
	if (c >= 65 && c <= 90) {
		return c -65;
	}

	if (c >= 97 && c <= 122) {
		return c - 97;
	}

	return 26;
}

int hash_val (int first , int second, int third) {
	return 26*26*first + 26* second + third;
}

int string_hash(char * string) {
	int first, second, third = 0;
	if (string[0] != '\0') {
		first = char_hash(string[0]);
	} else {
		return hash_val(0, 0, 0);
	}
	
	if (string[1] != '\0') {
		second = char_hash(string[1]);
	} else {
		return hash_val(first, 0, 0);
	}
	
	if (string[2] != '\0') {
		third = char_hash(string[2]);
	} else {
		return hash_val(first, second, 0);
	}

	return hash_val(first, second, third);

}

void insert(node ** hash_map, char * s, int val) {
	int index = string_hash(s);
	if (hash_map[index] != NULL) {
		if (strcmp(hash_map[index] ->key, s) == 0) {
			hash_map[index]->val += val;
			return;
		}

		node * n = hash_map[index];
		
		while (n->next) {
			if (strcmp(n -> next ->key, s) == 0) {
				n -> next -> val += val;
				return;
			}
			n = n -> next;
		}
	
		n->next = malloc(sizeof(node));
		n->next ->next = NULL;
		n->next ->key = malloc(sizeof(char) * (1+strlen(s)));
		strcpy(n -> next ->key, s);
		n->next ->key[strlen(s)] = '\0';
		n->next ->val = val;
		

	} else {
		hash_map[index] = malloc(sizeof(node));
		hash_map[index] ->next = NULL;
		hash_map[index] ->key = malloc(sizeof(char) * (1+strlen(s)));
		strcpy(hash_map[index] ->key, s);
		hash_map[index] ->key[strlen(s)] = '\0';
		hash_map[index] ->val = val;
	}
}

void print(node ** hash_map) {
	for (int i = 0; i < 27*27*27; i++) {
		node * n = hash_map[i];
		while (n) {
			printf("%s: %d\n", n ->key, n -> val);
			n = n -> next;
		}
	}	
}

int main() {
	node * hash_map[27 * 27 * 27] = {NULL}; 
	insert(hash_map, "bingus", 10);
	insert(hash_map, "bingus", 10);
	insert(hash_map, "bingsu", 10);
	insert(hash_map, "cingus", 10);
	print(hash_map);
}
