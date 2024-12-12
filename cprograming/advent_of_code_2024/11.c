#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./arena.h"
#include <stdbool.h>

#define ARENA_IMPLEMENTATION
#include "arena.h"
static Arena default_arena = {0};

typedef struct list {
	long long int value;
	struct list * next;
	struct list * prev;
} list;

list * list_create(long long int val, list * next, list * prev) {
	list * l = arena_alloc(&default_arena, sizeof(list));//malloc(sizeof(list));
	l->value = val;
	l->next = next;
	l->prev = prev;
	return l;
}

void list_free(list * head) {
	head->prev->next = NULL;

	while (head) {
		list * next = head->next;
		free(head);
		head = next;
	}
}

void list_print(list * head) {
	list * it = head; 
	do {
		printf("%lld ", it->value);
		it = it->next;
	} while (it != head);
	printf("\n");
}

void list_split(list * item, long long int new_value) {
	list * new_item = list_create(new_value, item->next, item);
	item->next = new_item;
	new_item->next->prev = new_item;
}

list * list_create_from_input() {
	long long int vals [8] = {5688, 62084, 2, 3248809, 179, 79, 0, 172169};

	list * tail = NULL;
	int index = 0;

	do {
		tail = list_create(vals[index], NULL, tail);
		index++;
	} while (index < 8);

	list * head = tail;
	while (head->prev) { 
		head->prev->next = head;
		head = head->prev;
	}

	head->prev = tail;
	tail->next = head;

	return head;
}

int n_digits(long long int val) {
	long long int compare = 10;
	int digits = 1;
	while (compare <= val) {
		digits ++; 
		compare *= 10;
	}
	return digits;
}

void right_left(long long int val, int n_digits, long long int * r, long long int * l) {
	int base = 1;
	for (int i = 0; i < n_digits/2; i++) {
		base *= 10;
	}
	*r = (val % base);	
	*l = (val - *r)/base;
}

void list_step(list * head) {
	list * it = head; 
	int digits;
	do {
		if (it->value == 0) {
			it->value = 1;	
		} else if ((digits = n_digits(it->value)) %2 == 0) {
			long long int left_num, right_num; 
			right_left(it->value, digits, &right_num, &left_num);
			it->value = left_num;
			list_split(it, right_num);
			it = it->next;
		} else {
			it->value = it->value*2024;	
		}
		it = it->next;
		if (!it) {
			printf("Disaster, reached a bad point");
			exit(0);
		}
		//printf("%p\n", it);
	} while (it != head);
}

void list_step_type2(list * it, int count, bool allow_base) {
	list * origional_next = it->next;
	int digits = 0;
	int index = 0;

	for (int i = 0; i < count; i++) {
		if (it->value == 0) {
			it->value = 1;	
		} else if ((digits = n_digits(it->value)) %2 == 0) {
			long long int left_num, right_num; 
			right_left(it->value, digits, &right_num, &left_num);
			it->value = left_num;
			list_split(it, right_num);
			list_step_type2(it->next, count - index, false);
		} else {
			it->value = it->value*2024;	
		}

		index++;
	}

	if (origional_next && allow_base) {
		list_step_type2(origional_next, count, true);
	}
}

void list_count(list * head) {
	int c = 0;
	list * it = head; 
	do {
		c++;
		it = it->next;
	} while (it != head);
	printf("list count: %d\n", c);
}

int main() {
	list * l = list_create_from_input();
	list_print(l);
	for (int i = 0; i < 75; i++) {
		printf("%d\n", i);
		list_step(l);
		list_count(l);
		//list_print(l);
	}
	list_count(l);
	//list_free(l);	
}











