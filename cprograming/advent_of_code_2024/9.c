#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#define INT int
enum {FREE_MODE = 0, FILE_MODE = 1};

typedef struct list {
	INT * values;
	int cur;
	int max;
} list;

list * list_create(int size) {
	list * l = calloc(1,sizeof(list));
	l->cur = 0;
	l->max = size;
	l->values = malloc(l->max * sizeof(INT));
	for (int i = 0;  i< l->max; i++) {
		l->values[i] = -1;
	}
	return l;
}

void list_free(list ** l) {
	free((*l)->values);
	free((*l));
	*l = NULL;
}

void list_print(list * l, bool explicit) {
	for (int i = 0;  i< l->cur; i++) {
		if (l->values[i] == -1) {
			printf(".");
		} else if (explicit){
			printf("|%d|", l->values[i]);
		} else {
			printf("%d", l->values[i]);
		}
	}
	printf("\n");
}

void list_push(list * l, int i) {
	l->values[l->cur] = i;
	l->cur ++;
	if (l->cur == l->max) {
		l->max *= 2;
		l->values = realloc(l->values, l->max*sizeof(INT));
		for (int i = l->cur; i < l->max; i++) {
			l->values[i] = -1;
		}
	}
}

void list_compress(list * l) {
	int low = 0, high = l->cur - 1;
	while (1) {
		while (low < l->cur && l->values[low] != -1) {
			low ++;
		}	
		while (high >= 0 && l->values[high] == -1) {
			high --;
		}
		if (high < low) {
			return;	
		} else {
			int high_cpy = l->values[high];
			l->values[high] = l->values[low];
			l->values[low] = high_cpy;
		}
	}
}

list * load_data(int size) {
	FILE * f = fopen("./input9.txt", "r");
	int ret, mode = FILE_MODE, index = 0;
	list * l = list_create(size);

	while ((ret = fgetc(f)) != EOF) {
		int value = ret - '0';
		if (mode == FILE_MODE) {
			for (int i = 0; i < value; i++) {
				list_push(l, index);
			}
			index ++;
		} else if (mode == FREE_MODE) {
			for (int i = 0; i < value; i++) {
				list_push(l, -1);
			}
		}

		mode = (mode + 1)&1;
	}

	fclose(f);
	return l;
}

long long int list_checksum(list * l) {
	long long int checksum = 0;
	int index = 0;
	while (index < l->cur && l->values[index] != -1) {
		checksum += index*l->values[index];
		index ++;
	}
	return checksum;
}

void part_1() {
	list * l = load_data(10000);
	//list_print(l, false);
	list_compress(l);
	//list_print(l, false);
	printf("Final checksum: %lld\n", list_checksum(l));
}

typedef struct block {
	int id;
	int size;
	struct block * next;	
	struct block * prev;	
} block;

block * block_create(int id, int size, block * prev) {
	block * b = calloc(1, sizeof(block));
	b->id = id;
	b->size = size;
	b->prev = prev;
	return b;
}

void block_print(block * b) {
	if (b->id == -1) {
		printf(".:%d\n", b->size);
	} else {
		printf("%d:%d\n", b->id, b->size);
	}
}

void block_list_free(block * b) {
	if (b->next) {
		block_list_free(b->next);	
	}
	free(b);
}

void block_list_print(block * b) {
	block * it = b;
	while (1) {
		for (int i = 0; i < it->size; i++) {
			if (it->id == -1) {
				printf(".");
			} else {
				printf("%d", it->id);
			}
		}
		it = it->next;
		if (it == b) {
			printf("\n");
			return;
		}
	}
}

block * load_liked_data() {
	FILE * f = fopen("./input9.txt", "r");
	int ret, mode = FILE_MODE, index = 0;
	block * tail = NULL; 

	while ((ret = fgetc(f)) != EOF && ret != '\n') {
		int value = ret - '0';
		if (mode == FILE_MODE) {
			tail = block_create(index, value, tail);	
			index ++;
		} else if (mode == FREE_MODE) {
			tail = block_create(-1, value, tail);	
		}

		mode = (mode + 1)&1;
	}

	printf("Done\n");

	block * head = tail;
	while (head->prev != NULL) {
		head->prev->next = head;
		head = head->prev;
	}
	head->prev = tail;
	tail->next = head;

	fclose(f);
	return head;
}

void list_linked_compress(block * head) {
	block * moveable = head->prev;
	
	while (moveable != head) {

		block * first = head;

		while (first != moveable && moveable->id != -1) {
			
			if (first ->id == -1) {
				if (first->size >= moveable->size) {
					first->size -= moveable->size;
				
					block * before_insert = first->prev;	
					block * new = block_create(moveable->id, moveable->size, NULL);

					before_insert->next = new;
					new->prev = before_insert;
					new->next = first;
					first->prev = new;

					moveable->id = -1;
					moveable = moveable->prev;	
					//block_list_print(head);

					break;
				}
			}
			first = first->next;
		}

		moveable = moveable->prev;
	}
}

long long int linked_list_checksum(block * head) {
	block * b = head;
	long long int checksum = 0;
	int index = 0;

	while (1) {
		for (int i = 0; i < b->size; i++) {
			if (b->id != -1) {
				checksum += index * b->id;	
			}
		
			index ++;
		};

		b = b->next;

		if (b == head) {
			break;
		}
	}	

	return checksum;

}

int main () {
	block * head = load_liked_data();
	//block_list_print(head);
	//block_print(head->prev);
	list_linked_compress(head);
	block_list_print(head);
	printf("Final checksum: %lld\n", linked_list_checksum(head));
}






//make sure to use long long ints for this one...
