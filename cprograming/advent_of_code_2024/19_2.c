#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

//typedef struct table {
//	char ** values;
//	int length;
//} table;
//
//int hash(table * t, char * s) {
//	long long int sum = 0;
//	for (int i = 0; i < strlen(s); i++) {
//		sum += 37*s[i];
//	}
//	int index = sum%t->length;
//	if (index < 0) {
//		printf("Bad hash for value: %s\n", s);
//		exit(1);
//	}
//	return index;
//}
//
//table * table_create(int length) {
//	table * t = calloc(1, sizeof(table));
//	t->values = calloc(length, sizeof(char *));
//	t->length = length;
//	return t;
//}
//
//void table_print(table * t) {
//	for (int i = 0; i < t->length; i++) {
//		if (t->values[i] != NULL) {
//			printf("%s\n", t->values[i]);
//		}
//	}
//}
//
//bool table_val_exits(table * t, char * val) {
//	int index = hash(t, val);
//	int index_cpy = index;
//
//	while(1) {
//		if (t->values[index] == NULL) {
//			return false;
//		}
//		if (strcmp(t->values[index], val) == 0) {
//			return true;
//		}	
//		index = (index + 1)%t->length;
//		if (index == index_cpy) {
//			return false;
//		}
//	}
//}
//
//void table_add(table * t, char * val) {
//	int index = hash(t, val);
//	int index_cpy = index;
//
//	while(1) {
//		if (t->values[index] == NULL) {
//			t->values[index] = val;
//		}
//		if (strcmp(t->values[index], val) == 0) {
//			return;
//		}	
//		index = (index + 1)%t->length;
//		if (index == index_cpy) {
//			printf("Insufficient table size\n");
//			exit(0);
//		}
//	}
//}

typedef struct string_list {
	char **strings;
	int cur;
	int max;
} string_list;

string_list * string_list_create() {
	string_list * sl = calloc(1, sizeof(string_list));
	sl->cur = 0;
	sl->max = 64;
	sl->strings = calloc(sl->max, sizeof(char *));
	return sl;
}

void string_list_push(string_list * sl, char * data) {
	sl->strings[sl->cur++] = data;
	if (sl->cur == sl->max) {
		sl->max *= 2;
		sl->strings = realloc(sl->strings, sizeof(char *)*sl->max);
	}	
}

bool check_string_advanced(char * s, string_list * sources);

void string_list_push_unique(string_list * sl, char * data) {
	if (!check_string_advanced(data, sl)) {
		string_list_push(sl, data);
	}
}

void string_list_print(string_list * sl) {
	for (int i = 0; i < sl->cur; i++) {
		printf("%s\n", sl->strings[i]);
	}
}

void string_list_generate_sizes(string_list * s) {

}

void string_list_sort(string_list * s) {

}

string_list * string_list_from_file(char * file, bool push_only_unique) {
	string_list * l = string_list_create();

	FILE * f = fopen(file, "r");
	char * buff = NULL;
	unsigned long size;
	int len;

	while ((len = getline(&buff, &size, f)) != -1) {
		if (buff[len - 1] == '\n') {
			buff[len -1] = '\0';
		}
		if (push_only_unique) {
			string_list_push_unique(l, buff);
		} else {
			string_list_push(l, buff);
		}
		buff = NULL;	
	}

	return l;
}

bool check_string_advanced(char * s, string_list * sources) {
	int len = strlen(s);
	//printf("Current goal: %s\n", s);
	for (int l = len; l > 0; l--) {
		//printf("len: %d\n", l);
		for (int i = 0; i < sources->cur; i++) {
			if (strlen(sources->strings[i]) <= l) {
				if (strncmp(s, sources->strings[i], l) == 0) {
					//printf("%s\n", sources->strings[i]);
					if (l == len) {
						return true; 
					} else {
						bool rest = check_string_advanced(s+l, sources);
						if (rest == true) {
							return true;
						}
					}
					
				}
			} 
		}
	}
	return false;
}

int size_compare(const void * a, const void * b) {
	char * A = *(char **)a;
	char * B = *(char **)b;
	int result = (strlen(A) - strlen(B));
	return result;
}

string_list * clean_string_list(string_list * sl) {
	qsort(sl->strings, sl->cur, sizeof(char*), size_compare);
	string_list * new = string_list_create();
	for (int i = 0; i < sl->cur; i++) {
		string_list_push_unique(new,sl->strings[i]);
	}
	return new;
}


int main() {
	string_list * sources = string_list_from_file("./input19s.txt", false);
	string_list * targets = string_list_from_file("./input19t.txt", false);
	//string_list * sources_new = clean_string_list(sources);
	string_list * sources_new = sources;

	int count = 0;
	for (int i = 0; i < targets->cur; i++) {
		if(check_string_advanced(targets->strings[i], sources_new)) {
			count ++;
			printf("%s\n", targets->strings[i]);
		}
	}
	printf("N valid: %d\n", count);
}

