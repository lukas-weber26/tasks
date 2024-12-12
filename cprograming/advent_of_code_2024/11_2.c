#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct table {
	long long int * values;
	long long int * counts;
	long long int n_values;	
} table;

table * table_create(long long int n_values) {
	table * t = malloc(sizeof(table));
	t->n_values = n_values;
	t->values = malloc(n_values* sizeof(long long int));
	t->counts = malloc(n_values* sizeof(long long int));
	for (int i = 0; i < n_values; i++) {
		t->values[i] = -1;
		t->counts[i] = 0;
	}
	return t;
}

void table_free(table * t) {
	free(t->values);
	free(t->counts);
	free(t);
}

void table_push_n(table * t,long long int v,long long int n) {
	if (v < 0) {
		printf("Values bellow zero\n");
		exit(0);
	}
	long long int index = (v%t->n_values);
	long long int index_cpy = index; 

	while (1) {
		if (t->values[index] == v) {
			t->counts[index] += n; 
			return;
		} else if (t->values[index] == -1) {
			t->values[index] = v;
			t->counts[index] += n; 
			return;
		} else {
			index = (index + 1)%t->n_values;
			if (index == index_cpy) {
				printf("Table size is insufficient\n");
			}
		}
	}

}

void table_push(table * t,long long int v) {
	table_push_n(t, v, 1);
}

long long int table_count (table * t) {
	long long int c = 0;
	for (int i = 0; i < t->n_values; i++) {
		c += (long long int)t->counts[i];
	}
	return c;
}

void table_print(table * t) {
	long long int c = 0;
	for (int i = 0; i < t->n_values; i++) {
		if (t->values[i] != -1) {
			printf("%lld:%lld\n", t->values[i],t->counts[i]);
		}
	}
	printf("\n");
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

void right_left(long long int val, int n_digits, long long int * r,long long int * l) {
	long long int base = 1;
	for (int i = 0; i < n_digits/2; i++) {
		base *= 10;
	}
	*r = (val % base);	
	*l = (val - *r)/base;
}

table * table_blink(table * t) {
	table * new = table_create(t->n_values);
	
	for (int i = 0; i < t->n_values; i++) {
		long long int val = t->values[i];
		long long int count = t->counts[i];
		int digits;

		if (val != -1) {
			assert(count > 0);
			if (val == 0) {
				table_push_n(new, 1, count);	
			}
			else if ((digits = n_digits(val)) %2 == 0) {
				long long int left_num, right_num; 
				right_left(val, digits, &right_num, &left_num);
				table_push_n(new, left_num, count);	
				table_push_n(new, right_num, count);	
			} else {
				table_push_n(new, val*2024, count);	
			}	

		}
	}

	table_free(t);
	return new;
}

int main () {

	table * starting_table = table_create(1048576);
	long long int data [8] = {5688,62084,2,3248809,179,79,0,172169};
	
	for (int i = 0; i < 8; i++) {
		table_push(starting_table, data[i]);
	}

	char c;
	int i = 0;
	while (i < 75) {
		starting_table = table_blink(starting_table);
		i++;
	}
	printf ("Blinked %d times, n items: %lld\n", i, table_count(starting_table));

}

//int main () {
//	table * starting_table = table_create(1048576);
//	long long int data [8] = {5688,62084,2,3248809,179,79,0,172169};
//	
//	for (int i = 0; i < 8; i++) {
//		table_push(starting_table, data[i]);
//	}
//
//	char c;
//	int i = 0;
//	while (i < 75) {
//		starting_table = table_blink(starting_table);
//		i++;
//	}
//	printf ("Blinked %d times, n items: %lld\n", i, table_count(starting_table));
//
//}










