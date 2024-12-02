#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct array{
	int data[100];
	int n_items;
} array;

int check_array(array a) {
	if (a.n_items < 2) {
		return 1;
	}

	int prev = a.data[0], next, diff, dir = 0;
	int i = 1;

	while (i < a.n_items) {
		next = a.data[i];
	
		diff = next - prev;

		if (diff == 0) {
			return 0;
		}
		
		if (dir == 0) {
			dir = diff/abs(diff);	
		}

		if (diff*dir > 3 || diff*dir < 1) {
			return 0;
		}

		prev = next;
		i++;
	}

	return 1;
 
}

int all_safe(array a) {
	int safe = 0;
	safe = check_array(a);
	if (safe) {
		return safe;
	} 

	for (int disable = 0; disable < a.n_items; disable ++) {
		array b;
		b.n_items = a.n_items - 1;
		int j = 0;
		for (int i = 0; i < a.n_items; i++) {
			if (i != disable) {
				b.data[j] = a.data[i];
				j++;
			}
		}
		safe = check_array(b);
		if (safe) {
			return safe;
		}
	}
	return 0;
}

array buff_to_array(char * buff) {
	array new;
	new.n_items = 0;
	int i = 0;

	do {
		new.data[new.n_items] = atoi(buff + i);
		new.n_items ++;

		while (isdigit(buff[i])) {
			i++;
		}

	} while (buff[i++] != '\n');

	return new;

}

int main() {
	FILE * fp = fopen("./input2.txt", "r");
	
	char * buff;
	unsigned long size;
	int safe_count = 0;

	while(getline(&buff, &size, fp) != -1) {
	
		array new = buff_to_array(buff);
		//for (int i = 0; i < new.n_items; i ++) {
		//	printf("%d, ", new.data[i]);
		//}
		//printf("\n");
		safe_count += all_safe(new);
		
	}

	printf("Valid count: %d\n", safe_count);

	fclose(fp);
}
