#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

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
	printf("IP, len, max = %d, %d, %d\n", s->instruction_pointer, s->cur_program_len, s->max_program_len);
}

void state_print_program(state * s) {
	for (int i = 0; i < s->cur_program_len; i++) {
		printf("%d,", s->program[i]);
	}
	printf("\n");
}

void state_print_output(state * s) {
	printf("OUTPUT LENGTH: %d\n", s->cur_out_len);
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
		exit(0);
	}
}


void run_adv(state * s){
	int denominator = roundl(pow(2.0, get_combo_op(s, s->program[s->instruction_pointer++])));
	s-> A = s->A/(denominator);
}

void run_bxl(state * s){
	int val = s->program[s->instruction_pointer++];
	s->B ^= val;
}

void run_bst(state * s){
	int val = get_combo_op(s, s->program[s->instruction_pointer++]) % 8;
	s->B = val;
}

void run_jnz(state * s){
	printf("Jump?\n");
	if (s->A != 0) {
		printf("Jump!\n");
		int val = s->program[s->instruction_pointer];
		s->instruction_pointer = val;
		printf("%d: instruction pointer\n", s->instruction_pointer);
	} else {
		s->instruction_pointer++;
	}
}

void run_bxc(state * s){
	s->B = s->B ^ s->C;	
	s->instruction_pointer++;
}

void run_out(state * s){
	printf("Output running\n");
	int val = get_combo_op(s, s->program[s->instruction_pointer++]) % 8;
	state_add_output(s, val);
}

void run_bdv(state * s){
	int denominator = roundl(pow(2.0, get_combo_op(s, s->program[s->instruction_pointer++])));
	s-> B = s->A/(denominator);
}

void run_cdv(state * s){
	int denominator = roundl(pow(2.0, get_combo_op(s, s->program[s->instruction_pointer++])));
	s-> C = s->A/(denominator);
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
			
	}
	
} 

int main() {
	state * s = state_from_file(50230824, 0, 0);
	state_print(s);
	state_print_program(s);
	state_print_output(s);
	state_run(s);
	state_print(s);
	state_print_program(s);
	state_print_output(s);
}
