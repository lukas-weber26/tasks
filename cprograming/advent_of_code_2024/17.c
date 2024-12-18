#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <setjmp.h>

jmp_buf buf;
int logtier = 0;
long long int n_iterations = 0;

void logger(char * c) {
	if (logtier)
		printf("%s", c);
}

typedef struct table {
	int * keys;
	int * counts;
	int max_items;
	int n_items;
	int n_unique;
} table;

//table * tables[8];

table * table_create() {
	table * t = calloc(1, sizeof(table));
	t->max_items = 2048;
	t->n_items = 0;
	t->n_unique = 0;
	t->keys = malloc(2040 * t->max_items);
	t->counts = malloc(2040 * t->max_items);
	for (int i = 0; i < t->max_items; i++) {
		t->keys[i] = -1;
		t->counts[i] = 0;
	}
	return t;
}

void table_insert(table * t, int key) {
	int index = ((key*37 >> 2) % t->max_items);
	int index_cpy = index;
	while (1) {
		if (t->keys[index] == key) {
			t->counts[index] ++;
			t->n_items ++;
			return;
		} else if (t->keys[index] == -1) {
			t->keys[index] = key;
			t->counts[index] ++;
			t->n_unique ++;
			t->n_items ++;
			return;
		} 
		index ++;
		if (index == index_cpy) {
			printf("ERROR, SMALL TABLE\n");
			exit(0);
		}
	}
}

void table_print(table * t) {
	for (int i = 0; i < t->max_items; i++) {
		if (t->keys[i] != -1) {
			printf("%d:%d\n", t->keys[i], t->counts[i]);
		}
	}
}

typedef struct state {
	int A;
	int B;
	int C;
	int * program;
	int cur_program_len;
	int max_program_len;
	int * out;
	int cur_out_len;
	int max_out_len;
	int instruction_pointer;
} state;

void state_free(state ** s) {
	free((*s)->program);
	free((*s)->out);
	free(*s);
	s = NULL;
}

state * state_create(int A, int B, int C) {
	state * s = calloc(1, sizeof(state));
	s->A = A;
	s->B = B;
	s->C = C;
	s->instruction_pointer = 0;
	s->program = calloc(64, sizeof(int));
	s->cur_program_len = 0;
	s->max_program_len = 64;
	s->out= calloc(64, sizeof(int));
	s->cur_out_len = 0;
	s->max_out_len = 64;
	return s;
}

void state_print(state * s) {
	printf("A,B,C = %d,%d,%d\n", s->A, s->B, s->C);
	//printf("IP, len, max = %d, %d, %d\n", s->instruction_pointer, s->cur_program_len, s->max_program_len);
}

void state_print_program(state * s) {
	for (int i = 0; i < s->cur_program_len; i++) {
		printf("%d,", s->program[i]);
	}
	printf("\n");
}

void state_print_output(state * s) {
	//printf("OUTPUT LENGTH: %d\n", s->cur_out_len);
	for (int i = 0; i < s->cur_out_len; i++) {
		printf("%d,", s->out[i]);
	}
	printf("\n");
}

void state_add_output(state * s, int n) {
	s->out[s->cur_out_len++] = n;
	if (s->max_out_len == s->cur_out_len) {
		s->max_out_len*= 2;
		s->out= realloc(s->out, s->max_out_len* sizeof(int));
	}
}

void state_add_instructions(state * s, int n) {
	s->program[s->cur_program_len++] = n;
	if (s->cur_program_len == s->max_program_len) {
		s->max_program_len *= 2;
		s->program = realloc(s->program, s->max_program_len * sizeof(int));
	}
}

state * state_from_file(int A, int B, int C) {
	state * s = state_create(A, B, C);
	FILE * f = fopen("./input17.txt", "r");
	int c;
	while ((c = getc(f)) != EOF) {
		if (isdigit(c)) {
			state_add_instructions(s, c - '0');	
		}
	}
	fclose(f);
	return s;
}

int get_combo_op(state * s, int n) {
	if (n < 4) {
		return n;
	} else if (n == 4) {
		return s->A;
	} else if (n == 5) {
		return s->B;	
	} else if (n == 6) {
		return s->C;	
	} else {
		printf("invalid\n");
		longjmp(buf, 1);
	}
}

long long int pow8(long long int v) {
	if (v == 0) {
		return 1;
	} else {
		long long int val = 8;
		for (long long int i = 1; i < v; i++) {
			val *= 8;	
		}
		return val;
	}
}

int mpow(int v) {
	if (v == 0) {
		return 1;
	} else {
		int val = 2;
		for (int i = 1; i < v; i++) {
			val *= 2;	
		}
		return val;
	}
}

void run_adv(state * s){
	logger("run_adv\n");
	int denominator = mpow(get_combo_op(s, s->program[s->instruction_pointer++]));
	int result = s->A/(denominator);
	s-> A = result;
	//table_insert(tables[0], result);
}

void run_bxl(state * s){
	logger("run_bxl\n");
	int val = s->program[s->instruction_pointer++];
	int result = s->B ^ val;
	s->B = result;
	//table_insert(tables[1], result);
}

void run_bst(state * s){
	logger("run_bst\n");
	int val = get_combo_op(s, s->program[s->instruction_pointer++]) % 8;
	s->B = val;
	//table_insert(tables[2], val);
}

void run_jnz(state * s){
	logger("run_jnz\n");
	int val = 1;
	if (s->A != 0) {
		val = s->program[s->instruction_pointer];
		s->instruction_pointer = val;
	} else {
		s->instruction_pointer++;
	}
	//table_insert(tables[3], val);
	//state_print(s);
	//state_print_output(s);
}

void run_bxc(state * s){
	logger("run_bxc\n");
	s->B = s->B ^ s->C;	
	s->instruction_pointer++;
	//table_insert(tables[4], s->B);
}

void run_out(state * s){
	logger("run_out\n");
	//printf("Output running\n");
	int val = get_combo_op(s, s->program[s->instruction_pointer++]) % 8;
	state_add_output(s, val);
	//table_insert(tables[5], val);
}

void run_bdv(state * s){
	logger("run_bdv\n");
	int denominator = mpow(get_combo_op(s, s->program[s->instruction_pointer++]));
	s-> B = s->A/(denominator);
	//table_insert(tables[6], s->B);
}

void run_cdv(state * s){
	logger("run_cdv\n");
	int denominator = mpow(get_combo_op(s, s->program[s->instruction_pointer++]));
	s-> C = s->A/(denominator);
	//table_insert(tables[7], s->C);
}

void state_run(state * s) {
	while (s->instruction_pointer < s->cur_program_len) {
		switch (s->program[s->instruction_pointer++]) {
			case 0: run_adv(s); break;//adv	
			case 1: run_bxl(s); break;//bxl
			case 2: run_bst(s); break;//bst	
			case 3: run_jnz(s); break;//jnz	
			case 4: run_bxc(s); break;//bxc	
			case 5: run_out(s); break;//out	
			case 6: run_bdv(s); break;//bdv	
			case 7: run_cdv(s); break;//cdv	
		}
		n_iterations ++;
		//state_print(s);	 //demonstrates that a decreases and program ends at a=0, and b and c don't
	}
} 

bool io_cmp(state * s) {
	if (s->cur_out_len != s->cur_program_len) {
		return false;
	}
	for (int i = 0; i < s->cur_out_len; i ++) {
		if (s->program[i] != s->out[i]) {
			return false;
		}
	}
	return true;
}

state * state_copy(state * s) {
	state * c = state_create(s->A, s->B, s->C);
	for (int i = 0; i < s->cur_program_len; i++) {
		state_add_instructions(c, s->program[i]);
	}
	return c;
}

void get_crazy(state * s) {
	int i = 0;
	while (1) {
		state * s_cpy = state_copy(s);
		s->A = 117440;
		state_run(s_cpy);		
		if (io_cmp(s_cpy)) {
			printf("Done! %d\n",i);
			exit(0);
		} else {
			printf("Fail!\n");
			exit(0);
			//printf("%d\n",i);
		}
		setjmp(buf);
		//state_free(&s);
		i++;
	}
}

bool list_cmp(int target[], int cmp [], int len) {
	for (int i = 0; i < len; i++) {
		if (target[i] != cmp[i]) {
			return false;
		}
	}
	return true;
}

int find_list_input(int target[], int target_len, state * initial_state) {
	for (int i = 0; i < 100000000; i ++) {
		//printf("%d\n", i);

		state * cpy = state_copy(initial_state);
		cpy->A = i;
		state_run(cpy);
		//state_print_output(cpy);

		if (cpy->cur_out_len < target_len) {
			state_free(&cpy); 
			continue;
		}

		if (list_cmp(target, cpy->out, target_len)) {
			//printf("Match\n");
			state_free(&cpy);
			return i;
		}
		
		state_free(&cpy);

	}
	return  -1;
}

void quick_state_run(state * s, int A) {
	state * scpy = state_copy(s);
	scpy->A = A;
	state_run(scpy);
	state_print_output(scpy);
	state_free(&scpy);
}

void print_octal(int input) {
	int c = 0;
	printf("0");
	while (input > 0) {
		printf(" + %lld*%d", pow8(c), (input % 8));	
		input = input >> 3;
		c++;
	}
}

void produce_n(state * s, int len, int max_len) {
	int l [6] = {0,3,5,4,3,0};
	int input = find_list_input(l, len, s);
	//printf("Input %d produces: ", input);
	printf("Input ");
	print_octal(input);
	printf(", produces: \n");
	for (int j = 0; j < max_len; j++) {
		printf("%d,", l[j]);
	}
	printf("\n");
	quick_state_run(s, input);
}

void experiment_1(state * s) {
	for (int i = 1; i < 7; i++) {
		produce_n(s, i, 6);
		printf("\n\n");
	}
}


int octals_to_int(int octals[]) {
	int sum = 0;
	for (int i = 0; i < 7; i++) {
		sum += pow8(i) * octals[i];
	}
	return sum;
}


state * state_run_with_input(state * s, int octal_value) {
	state * s2 = state_copy(s);
	s2->A = octal_value;
	state_run(s2);
	return s2;
}

void experiment_2(state * s) {
	//goal: same as experiment one but backwards
	int target [6] = {0,3,5,4,3,0};
	int octals [6] = {-1,0,3,5,4,3}; //5th power is 3
	//so in short, the first one was not needed
	//and the 5th power is actually used to choose the 5th digit, despite there being 6 total (the final 0 is a given)

	for (int i = 0; i < 8; i++) { 
		state * complete = state_run_with_input(s, pow8(5)*octals[5] + pow8(4)*octals[4] + pow8(3) * octals[3] + pow8(2) * octals[2] + pow8(1) * 0);
		printf("%d: ", i);
		state_print_output(complete);
		state_free(&complete);
	}

}

int main() {
	//think I get it
	//how does a impact the number of iterations
	state * s = state_from_file(0, 0, 0);
	//experiment_1(s);
	experiment_2(s);


	//for (int i = 1; i < 7; i++) {
	//	printf("Input %d produces: ", find_list_input(l, i, s));
	//	for (int j = 0; j < i; j++) {
	//		printf("%d, ", l[j]);
	//	}
	//	printf("\n");
	//}

}
