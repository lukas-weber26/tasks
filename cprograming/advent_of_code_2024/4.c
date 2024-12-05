#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef char target[4];
typedef char t3[3];

void target_print(target t) {
	for (int i = 0; i < 4; i++) {
		printf("%c", t[i]);
	}
	printf("\n");
}

void t3_print(t3 t) {
	for (int i = 0; i < 3; i++) {
		printf("%c", t[i]);
	}
	printf("\n");
}

typedef struct matrix {
	char ** data;
	int row_length;
	int n_rows;
	int max_rows;
} matrix;

matrix matrix_create() {
	matrix new;
	new.data = calloc(100,sizeof(char *));
	new.max_rows = 100;
	new.n_rows = 0;
	return new;
}

char get(matrix m, int x, int y) {
	//return m.data[y][x];
	return m.data[y%m.n_rows][x%m.row_length];
}

matrix add_col_and_expand(matrix m) {
	m.n_rows ++;
	if (m.n_rows >= m.max_rows - 1) {
		int rows = m.max_rows;
		m.max_rows *= 2;
		m.data = realloc(m.data, m.max_rows*sizeof(char *));
		for (int i = rows ; i < m.max_rows; i ++) {
			m.data[i] = NULL;
		}
	}

	return m;
}

matrix load_from_file() {
	matrix n = matrix_create();
	FILE * f = fopen("./input4.txt", "r");
	unsigned long size;
	int read;

	while ((read = getline(&n.data[n.n_rows], &size, f)) > 0) {
		//printf("%d\n", n.n_rows);
		if (n.n_rows == 0) {
			n.row_length = read - 1;
		}
		n = add_col_and_expand(n);
	}

	printf("Read %d lines, with size %d\n", n.n_rows, n.row_length);

	fclose(f);

	FILE * teststream = fopen("./tester.txt", "w");
	assert(teststream);
	for (int i = 0; i < n.n_rows; i ++) {
		for (int j = 0; j < n.row_length; j ++) {
			char c = get(n,j,i);
			fputc(c, teststream);
		}
		fputc('\n', teststream);
	}

	fclose(teststream);

	return n;
}

int t3_compare(t3 a, t3 b) {
	for (int i = 0; i < 3; i++) {
		if (a[i] != b[i]) {
			return 0;
		}
	}
	return 1;
}

int target_compare(target a, target b) {
	for (int i = 0; i < 4; i++) {
		if (a[i] != b[i]) {
			return 0;
		}
	}
	return 1;
}

int horizontal_conv(target t, matrix m) {
	int count = 0;
	for (int x  = 0; x < m.row_length - 3; x++) {
		for (int y  = 0; y < m.n_rows; y++) {
			target local = {get(m,x,y), get(m,x+1,y), get(m,x+2,y), get(m,x+3,y)};
					
			if (target_compare(t, local)) {
				count ++;
			}
		}
	}
	return count;
}

int vertical_conv(target t, matrix m) {
	int count = 0;
	for (int x  = 0; x < m.row_length; x++) {
		for (int y  = 0; y < m.n_rows-3; y++) {
			target local = {get(m,x,y), get(m,x,y+1), get(m,x,y+2), get(m,x,y+3)};
			if (target_compare(t, local)) {
				count ++;
			}
		}
	}
	return count;
}

int rdiag_conv(target t, matrix m) {
	int count = 0;
	for (int x  = 0; x < m.row_length-3; x++) {
		for (int y  = 0; y < m.n_rows-3; y++) {
			target local = {get(m,x,y), get(m,x+1,y+1), get(m,x+2,y+2), get(m,x+3,y+3)};
			if (target_compare(t, local)) {
				count ++;
			}
		}
	}
	return count;
}

int ldiag_conv(target t, matrix m) {
	int count = 0;
	for (int x  = 0; x < m.row_length-3; x++) {
		for (int y  = 0; y < m.n_rows-3; y++) {
			target local = {get(m,x,y+3), get(m,x+1,y+2), get(m,x+2,y+1), get(m,x+3,y)};
			if (target_compare(t, local)) {
				count ++;
			}
		}
	}
	return count;
}

int mas_conv(matrix m) {
	int count = 0;

	t3 one = {'M','A','S'};
	t3 two = {'S','A','M'};

	for (int x  = 0; x < m.row_length-2; x++) {
		for (int y  = 0; y < m.n_rows-2; y++) {
			t3 r = {get(m,x,y), get(m,x+1,y+1), get(m,x+2,y+2)};
			t3 l = {get(m,x+2,y), get(m,x+1,y+1), get(m,x,y+2)};


			if ((t3_compare(one, r) || t3_compare(two, r)) && (t3_compare(one,l) || t3_compare(two,l))) {
				count ++;
			} else {
				t3_print(r);	
				t3_print(l);	
				printf("\n");
			}

		}
	}
	return count;
}

int main() {
	matrix m = load_from_file();
	int x = 0;

	//target t1 = {'X','M','A','S'};
	//target t2 = {'S','A','M','X'};
	
	//x += horizontal_conv(t1, m);
	//printf("Count: %d\n", x);
	//x += horizontal_conv(t2, m);
	//printf("Count: %d\n", x);
	//x += vertical_conv(t1, m);
	//printf("Count: %d\n", x);
	//x += vertical_conv(t2, m);
	//printf("Count: %d\n", x);
	//x += rdiag_conv(t1, m);
	//printf("Count: %d\n", x);
	//x += rdiag_conv(t2, m);
	//printf("Count: %d\n", x);
	//x += ldiag_conv(t1, m);
	//printf("Count: %d\n", x);
	//x += ldiag_conv(t2, m);
	//printf("Final count: %d\n", x);
	
	x = mas_conv(m);
	printf("Final count: %d\n", x);
	
}
