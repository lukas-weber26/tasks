#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int gt(const void * a, const void * b) {
	int * A = (int *)a;
	int * B = (int *)b;
	return (*A > *B);
}

void merge(int * a, int * b, int length) {
	int ac = 0;
	int bc = 0;

	for (int i = 0; i < length; i++) {
		if (a[ac] > b[bc]) {
			int t = a[ac];
			a[ac] = b[bc];
			b[bc] = t;
			ac ++;
		} else {
			ac ++;
		}
	}
}

void sort(int * list, int length) {
	if (length == 2) {
		if (list[0] > list[1]) {
			int t = list[0];
			list[0] = list[1];
			list[1] = t;
			return;
		}
		return;
	} else if (length < 2) {
		return;
	} else {
		int len_one = length/2;
		int len_two = length - len_one;

		sort(list, len_one);
		sort(list+len_one, len_two);
		merge(list, list+len_one, len_one);
		return;
	}
}

int dist(int a, int b) {
	if (a-b > 0) {
		return a-b;
	}
	return b-a;
}

int sum(int * a, int *b) {
	int s = 0;
	for (int i = 0; i < 6; i++) {
		s += dist(a[i], b[i]);
	}
	return s;
}

void run_example() {
	int a [6] = {3,4,2,1,3,3};
	int b [6] = {4,3,5,3,9,3};

	sort(a, 6);
	sort(b, 6);

	for (int i = 0; i < 6; i++) {
		printf("%d, %d\n", a[i], b[i]);
	}

	int s = sum(a,b);
	printf("Sum: %d\n", s);
}

typedef struct list {
	int * data;
	int max_len;
	int curr_len;
} list;

list list_create() {
	list new;
	new.data = malloc(100*sizeof(int));
	new.max_len = 100;
	new.curr_len = 0;
	return new;
}

list list_append(list l, int a) {
	l.data[l.curr_len] = a;
	l.curr_len ++; 
	if (l.curr_len == l.max_len) {
		l.max_len *= 2;
		l.data = realloc(l.data, l.max_len*sizeof(int));
	}
	return l;
}

typedef struct table {
	int * item_names;	
	int * item_values;	
} table;

table table_new() {
	table new;
	new.item_names = malloc(sizeof(int) * 2000);
	new.item_values = malloc(sizeof(int) * 2000);
	for (int i = 0; i < 2000; i++) {
		new.item_names[i] = -1;
		new.item_values[i] = 0;
	}
	return new;
}

int hash(int i) {
	return (37*(i>>2))%2000;
}

void table_add_name(table t, int name) {
	int loc = hash(name);
	while(1) {
		if (t.item_names[loc] == name) {
			return;
		} else if (t.item_names[loc] == -1) {
			t.item_names[loc] = name;
		} else {
			loc = (loc + 1) % 2000;
		}
	}
}

void table_increment(table t, int name) {
	int loc = hash(name);
	while(1) {
		if (t.item_names[loc] == name) {
			break;
		} else if (t.item_names[loc] == -1) {
			return;	
		} else {
			loc = (loc + 1) % 2000;
		}
	}
	t.item_values[loc] ++;
}

int main() {
	//run_example();
	//exit(0);

	list a = list_create();
	list b = list_create();
	char * buff;
	unsigned long size;

	FILE * f = fopen("./input.txt", "r"); 

	while(getline(&buff, &size, f) != -1) {
		a = list_append(a, atoi(buff));
		char * next_num = buff;
		while (*next_num != ' ') {
			next_num ++;
		}
		b = list_append(b, atoi(next_num + 3));
	}

	fclose(f);

	assert(a.curr_len == b.curr_len);

	qsort(a.data, a.curr_len, sizeof(int), gt);
	qsort(b.data, b.curr_len, sizeof(int), gt);

	int s = 0;
	for (int i = 0; i < a.curr_len; i++) {
		//printf("%d, %d\n", a.data[i], b.data[i]);
		s += abs(a.data[i] - b.data[i]);
	}

	table t = table_new();
	int global_sum = 0;

	for (int i = 0; i < a.curr_len; i++) {
		table_add_name(t, a.data[i]);
	}

	for (int i = 0; i < b.curr_len; i++) {
		table_increment(t, b.data[i]);
	}

	for (int i = 0; i < 2000; i++) {
		if (t.item_names[i] != -1 && (t.item_values[i] != 0)) {
			global_sum += t.item_names[i] * t.item_values[i];
		}
	}

	printf("global sum: %d\n", global_sum);
	//int s = sum(a.data,b.data);
	//printf("Sum: %d\n", s);
	
	//similarity score time
		

}










