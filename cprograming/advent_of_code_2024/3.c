#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct reader {
	char current;
	char next;
	FILE * f;
} reader;

reader * reader_create(char * filename) {
	reader *new = malloc(sizeof(reader));
	new->f = fopen(filename, "r");
	new->current = fgetc(new->f);
	new->next = fgetc(new->f);
	return new;
};

void reader_next(reader * r) {
	r->current = r->next;
	int result = fgetc(r->f);
	if (result == -1) {
		r->next = '\0';
		return;
	}
	r->next = result;

}

void reader_delete(reader * r) {
	fclose(r->f);
	free(r);
}

void reader_eat_illigals(reader * r) {
	while (1) {
		if (r->current == 'm' || r->current == 'd' || r->current == '\0') {
			return;
		}
		reader_next(r);
	}
}

int reader_expect_char(reader * r, char c) {
	char current = r->current;
	reader_next(r);
	return current == c;
}

int reader_expect_number(reader * r, int * output, char terminator) {
	int n = 0, count = 0;
	
	while (1) {
		if (isdigit(r->current) && count < 3) {
			n = 10*n + ((r->current)-'0');
			count ++;
			reader_next(r);
		} else if (r->current == terminator && count > 0) {
			*output = n;
			reader_next(r);
			return 1;
		} else {
			reader_next(r);
			return 0;
		}
	}

}

int reader_extract_mul(reader * r) {
	int a,b;
	if (!reader_expect_char(r, 'm'))
		return 0;
	if (!reader_expect_char(r, 'u'))
		return 0;
	if (!reader_expect_char(r, 'l'))
		return 0;
	if (!reader_expect_char(r, '('))
		return 0;
	if (!reader_expect_number(r, &a, ','))
		return 0;
	if (!reader_expect_number(r, &b, ')'))
		return 0;
	return a*b;
}

void reader_extract_do(reader * r, int * active) {
	if (!reader_expect_char(r, 'd'))
		return;
	if (!reader_expect_char(r, 'o'))
		return;
	if (r->current == '(') {
		if (!reader_expect_char(r, '('))
			return;
		if (!reader_expect_char(r, ')'))
			return;
		*active = 1;
	} else if (r->current == 'n') {
		if (!reader_expect_char(r, 'n'))
			return;
		if (!reader_expect_char(r, '\''))
			return;
		if (!reader_expect_char(r, 't'))
			return;
		if (!reader_expect_char(r, '('))
			return;
		if (!reader_expect_char(r, ')'))
			return;
		*active = 0;
	} else {
		return;
	}

}

void mul_it_over() {
	int sum = 0;
	int active = 1;
	reader * r = reader_create("./input3.txt");

	while(1) {
		if (r->current == '\0' || r->next == '\0') {
			reader_delete(r);	
			printf("Final value: %d\n", sum);
			return;
		}
		reader_eat_illigals(r);
		if (r->current == 'm') {
			sum += reader_extract_mul(r)*active;
		} else if (r->current == 'd') {
			reader_extract_do(r, &active);
		}
	}
}

int main() {
	mul_it_over();
}


