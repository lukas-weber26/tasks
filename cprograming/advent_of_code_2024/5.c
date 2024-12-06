#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int collisions = 0; 

typedef struct rule {
	int number;
	int * later_numbers;
	int cur_len;
	int max_len;
} rule;

rule * rule_create(int number, int size_hint) {
	rule * n = calloc(1, sizeof(rule));
	n->number = number;
	n->later_numbers = calloc(size_hint, sizeof(int));
	n->cur_len = 0;
	n->max_len = size_hint;
	return n;
}

void rule_add_later(rule * r, int n) {
	r->later_numbers[r->cur_len] = n;
	r->cur_len ++;
	if (r->cur_len >= r->max_len) {
		r->max_len *= 2;
		r->later_numbers = realloc(r->later_numbers, sizeof(int)*r->max_len);
	}
}

typedef struct rule_dictionary {
	rule ** rules;
	int n_rules;
	int filled_slots;
} rule_dictionary;

rule_dictionary * global_rules;

rule_dictionary * rule_dictionary_create(int size_hint) {
	rule_dictionary * r = calloc(1, sizeof(rule_dictionary));
	r->n_rules = size_hint;
	r->rules = calloc(size_hint, sizeof(rule*));
	r->filled_slots = 0;
	return r;
}

int hash(int X, int size) {
	return (X*37>>2)%size;
}

void rule_dictionary_add_item(rule_dictionary * dict, int X, int Y) {
	int index = hash(X, dict->n_rules);
	int iterate = index;
	while (1) {
		if (dict->rules[iterate] == NULL) {
			dict->rules[iterate] = rule_create(X, 10);	
			rule_add_later(dict->rules[iterate], Y);
			dict->filled_slots ++;
			return;
		} else if (dict->rules[iterate]->number == X) {
			rule_add_later(dict->rules[iterate], Y);
			return;
		} 
		
		iterate = (iterate + 1) % dict->n_rules;
		collisions ++;
		
		if (iterate == index || dict->filled_slots == dict->n_rules) {
			printf("Unable to find number of free slot.\n");
			exit(0);
		}
	}	
}

rule * rule_dictionary_get_rule(rule_dictionary * dict, int X) {
	int index = hash(X, dict->n_rules);
	int iterate = index;
	while (1) {
		if (dict->rules[iterate] == NULL) {
			return NULL;
		} else if (dict->rules[iterate]->number == X) {
			return dict->rules[iterate];
		} else {
			iterate = (iterate + 1) % dict->n_rules;
			collisions ++;
			
			if (iterate == index || dict->filled_slots == dict->n_rules) {
				return NULL;
			}
		} 
	}	
}

rule_dictionary * rule_dictionary_from_file(int size_hint) {
	FILE * f = fopen("./input5r.txt","r");
	
	rule_dictionary * r = rule_dictionary_create(size_hint);
	char * buff = NULL;
	unsigned long size;

	while (getline(&buff, &size, f) > 0) {
	
		char * working_buff = buff;

		int X, Y;
		X = atoi(working_buff);

		while (*working_buff != '|') {
			working_buff ++;	
		}

		Y = atoi(working_buff + 1);
		rule_dictionary_add_item(r, X, Y);

		printf("%d|%d\n", X,Y);

		free(buff);
		buff = NULL;
	}

	fclose(f);
	return r;
}

void rule_dictionary_print(rule_dictionary * d) {
	int n = 0;
	for (int i = 0; i < d->n_rules; i++) {
		if (d->rules[i] != NULL) {
			rule * r = d->rules[i];
			printf("%d: ", r->number);
			for (int j = 0; j < r->cur_len; j++) {
				printf("%d, ", r->later_numbers[j]);
				n++;
			}
			printf("\n");
		}
	}
	printf("Totoal number of rules: %d\n", n);
}

typedef struct list {
	int * numbers;
	int cur_n;
	int max_n;
} list;

list * list_create(int size_hint) {
	list * l = calloc(1,sizeof(list));
	l->cur_n = 0; 	
	l->max_n = 100;
	l->numbers = calloc(l->max_n, sizeof(int));
	return l;
}

void list_free(list * l) {
	free(l->numbers);
	free(l);
	l = NULL;
}

void list_push(list * l, int x) {
	l->numbers[l->cur_n] = x;
	l->cur_n ++;
	if (l->cur_n >= l->max_n) {
		l->max_n *= 2;
		l->numbers = realloc(l->numbers, l->max_n*sizeof(int));
	}
}

int list_validate(list * l, rule_dictionary * dict) {
	//if it were guaranteed that rules are consistent in each list I could create a precedence and simply check that without n2
	for (int i = l->cur_n - 1; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			
			rule * r = rule_dictionary_get_rule(dict, l->numbers[i]);	
			if (r != NULL) {
				for (int z = 0; z < r->cur_len; z++) {
					if (r->later_numbers[z] == l->numbers[j]) {
						return 0;
					}
				}
			}

		}
	}	
	return 1;	
}

void list_print(list * l) {
	for (int i = 0; i < l->cur_n; i++) {
		printf("%d, ", l->numbers[i]);
	}
	printf("\n");
}

int list_middle(list * l) {
	return l->numbers[l->cur_n/2];
}

typedef struct bad_list {
	list ** lists;
	int n_curr;
	int n_max;
} bad_list;

bad_list * bad_list_create() {
	bad_list * b = calloc(1, sizeof(bad_list));
	b->n_max = 512;
	b->n_curr = 0;
	b->lists = malloc(sizeof(list *) * b->n_max);
	return b;
}

void bad_list_push(bad_list * b, list * l) {
	b->lists[b->n_curr] = l;
	b->n_curr ++;
	if (b->n_curr >= b->n_max) {
		b->n_max *= 2;
		b->lists = realloc(b->lists, b->n_max * sizeof(list *));
	}
}

void bad_list_print(bad_list * b) {
	printf("Invalid lists:\n");
	for (int i = 0; i < b->n_curr; i++) {
		list_print(b->lists[i]);
	}
}

int validate_lists(rule_dictionary * r, bad_list * b) {
	FILE * lists = fopen("./input5l.txt", "r");

	int count = 0;
	char * buff = NULL;
	unsigned long size;
	assert(lists);

	while (getline(&buff, &size, lists) > 0) {
	
		char * working_buff = buff;
		list * l = list_create(50); 

		while (1) {
			int X = atoi(working_buff);
			list_push(l, X);
			while (isdigit(*(++working_buff)));
			
			if (*working_buff == '\n') {
				break;
			} else {
				working_buff ++;
			}
		}
			
		if (list_validate(l, r)) {		
			count += list_middle(l);
			printf("Valid: ");
			list_print(l);	
			list_free(l);
		} else {
			printf("Invalid: ");
			list_print(l);	
			bad_list_push(b, l);
		}

		free(buff);
		buff = NULL;
	}

	fclose(lists);
	return count;
}

int compare(const void * a, const void * b) {
	int * A = (int *)a;
	int * B = (int *)b;

	rule * ra = rule_dictionary_get_rule(global_rules, *A);

	if (ra != NULL) {
		for (int i = 0; i < ra->cur_len; i++) {
			if (ra->later_numbers[i] == *B) {
				return 1;
			}
		}
	}

	rule * rb = rule_dictionary_get_rule(global_rules, *B);
	if (rb != NULL) {
		for (int i = 0; i < rb->cur_len; i++) {
			if (rb->later_numbers[i] == *A) {
				return -1;
			}
		}
	}

	return 0;

}

void bad_list_fix(bad_list * b, rule_dictionary * d) {
	global_rules = d;
	int count = 0;

	for (int i = 0; i < b->n_curr; i++) {
		qsort(b->lists[i]->numbers, b->lists[i]->cur_n, sizeof(int), compare);
		count += list_middle(b->lists[i]);
	}

	printf("Fixed lists: %d\n", count);

}

int main() {
	rule_dictionary * d = rule_dictionary_from_file(2003);
	bad_list * b = bad_list_create();
	rule_dictionary_print(d);	
	printf("Collisions: %d\n", collisions);
	int x = validate_lists(d, b); 
	printf("Number of valid lists: %d\n", x);
	bad_list_print(b);
	bad_list_fix(b,d);
}
