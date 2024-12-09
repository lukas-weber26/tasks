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

	while ((ret = fgetc(f)) != -1) {

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

int main () {
	list * l = load_data(10000);
	//list_print(l, false);
	list_compress(l);
	//list_print(l, false);
	printf("Final checksum: %lld\n", list_checksum(l));
}






//make sure to use long long ints for this one...
