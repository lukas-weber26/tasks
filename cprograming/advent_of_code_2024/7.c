#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

int power(int a, int b) {
	if (b <= 0) {
		return 0;
	} else {
		int p = a;
		for (int i = 1; i < b; i ++) {
			p *= a;
		}
		return p;
	}
}

long long add(long long a, long long b) {
	return a + b;	
}

long long multiply(long long a, long long b) {
	return a * b;	
}

long long concat(long long a, long long b) {
	int i = 1;
	while (i <= b) {
		i = i*10;
	}
	return a * (i) + b;	
}

typedef struct operator_list {
	long long (**operators)(long long, long long);
	int max;
	int curr;
} operator_list; 

operator_list * operator_list_create(int n_items) {
	operator_list * new = calloc(1, sizeof(operator_list));
	new->max = n_items;
	new->curr = 0;
	new->operators = calloc(n_items, sizeof(long long * (long long, long long)));
	return new;
}

void operator_list_add(operator_list * list, long long(* fn) (long long, long long))  {
	assert(list->curr < list->max);
	list->operators[list->curr] = fn;
	list->curr ++;
}

void operator_list_free(operator_list * l) {
	free(l->operators);
	free(l);
	l = NULL;
}

operator_list * create_add_mul_list() {
	operator_list * l = operator_list_create(2);
	operator_list_add(l, add);
	operator_list_add(l, multiply);
	return l;
}

operator_list * create_add_mul_concat_list() {
	operator_list * l = operator_list_create(3);
	operator_list_add(l, add);
	operator_list_add(l, multiply);
	operator_list_add(l, concat);
	return l;
}

int operator_list_run(operator_list * l, int index, long long a, long long b) {
	assert(index >= 0 && index <= l->curr);	
	return (l->operators[index])(a,b);
}

void quick_test() {
	operator_list * l = create_add_mul_list();
	//printf("3,3:\n");
	//printf("Add %d\n", operator_list_run(l, 0, 3, 3));
	//printf("Mull %d\n", operator_list_run(l, 1, 3, 3));
	operator_list_free(l);
}

typedef struct calibration_list {
	long long target_value;
	long long * list;
	int max;
	int curr;
} calibration_list;

calibration_list * calibration_list_create(long long target, int size_hint) {
	calibration_list * l = calloc(1, sizeof(calibration_list));
	l->max = size_hint;
	l->curr = 0;
	l->list = calloc(l->max, sizeof(long long ));
	l->target_value = target;
	return l;
}

void calibration_list_free(calibration_list * l) {
	free(l->list);
	free(l);
	l = NULL;
} 

void calibration_list_print(calibration_list * l) {
	printf("%lld:", l->target_value);
	for (int i = 0; i < l->curr; i ++) {
		printf(" %lld", l->list[i]);
	}
	printf("\n");
} 

void calibration_list_push(calibration_list * l, long long val) {
	l->list[l->curr] = val;
	l->curr ++;
	if (l->curr >= l->max) {
		l->max *= 2;
		l->list = realloc(l->list, l->max*sizeof(long long));
	}
} 

typedef struct calibration_list_buff {
	calibration_list ** lists;
	int curr;
	int max;
} calibration_list_buff;

calibration_list_buff * calibration_list_buff_create(int size_hint) {
	calibration_list_buff * l = calloc(1, sizeof(calibration_list_buff));
	l->max = size_hint;
	l->curr = 0;
	l->lists = calloc(l->max, sizeof(calibration_list*));
	return l;
}

void calibration_list_buff_push(calibration_list_buff * b, calibration_list * l) {
	b->lists[b->curr] = l;
	b->curr ++;
	if (b->curr >= b->max) {
		b->max *= 2;
		b->lists = realloc(b->lists, b->max*sizeof(int));
	}
}

void calibration_list_buff_print(calibration_list_buff * b) {
	for (int i = 0; i < b->curr; i++) {
		calibration_list_print(b->lists[i]);	
	}
	printf("\n");
}

void calibration_list_buff_free(calibration_list_buff * b) {
	for (int i = 0; i < b->curr; i++) {
		calibration_list_free(b->lists[i]);	
	}
	free(b->lists);
	free(b);
	b = NULL;
}

long long evalutae_code(calibration_list * l, operator_list * o, int * op_indices) {
	long long result = (o->operators[op_indices[0]])(l->list[0],l->list[1]);

	for (long long i = 1; i < l->curr - 1; i ++) {
		result = (o->operators[op_indices[i]])(result,l->list[i+1]);
	}

	return (result == l->target_value);
}

long long calibration_list_validate(calibration_list * l, operator_list * o) {

	int base = o->curr;
	int combinations = power(base, (l->curr)-1);
	int * indices = alloca(sizeof(int) * l->curr - 1);

	//printf("combinations:%d\n", combinations);
	int c =0;	
	for (int j = 0; j < l->curr; j ++) {
		indices[j] = 0;
	}

	for (int i = 0; i < combinations; i ++) {
		c ++;
		int bin = i;
		for (int j = 0; j < l->curr; j ++) {
			indices[j] = bin % (base);
			//bin = bin >> 1;
			bin = (bin - (bin % (base)))/base; //either divide by 2, or 
		}

		for (int z = 0; z < l->curr; z++) {
			//printf("%d", indices[z]);
		}
		//printf("\n");

		long long r = evalutae_code(l, o, indices);	
		if (r == 1) {
			//printf("target found.\n");
			return l->target_value;
		}

	}
	
	//printf("Attempted %d combinations.\n", c);
	//exit(0);
	return 0;
};

calibration_list_buff * calibration_list_from_file(int size_hint) {
	FILE * f = fopen("./input7.txt", "r");
	char * buff = NULL, * working_buff = NULL;
	unsigned long size;
	int ret;	
	long long sum = 0;
	int i = 0;

	calibration_list_buff * b = calibration_list_buff_create(100);
	operator_list * o = create_add_mul_concat_list();

	while ((ret = getline(&buff, &size, f)) > 0) {
		working_buff = buff;
	
		long long v = atol(working_buff);
		calibration_list * c = calibration_list_create(v, size_hint);
		while (isdigit(*working_buff) || *working_buff == ':') {
			working_buff ++;
		}
		working_buff ++;

		while (*working_buff != '\n') {
			calibration_list_push(c,atoll(working_buff));
			while (isdigit(*working_buff)) {
				working_buff ++;
			} 
			if (*working_buff == ' ') {
				working_buff ++;
			}
		}
		
		sum += calibration_list_validate(c, o);
		if (sum < 0) {
			printf("Overflow!\n");
			exit(0);
		}
		
		free(buff);
		buff = NULL;
	}
	
	printf("Calibrated value: %lld\n", sum);
	fclose(f);
	return b;
}

int main() {
	calibration_list_from_file(1000);
	//printf("%lld, %lld: %lld\n", (long long int)100, (long long int)100, concat(100, 100));
	//calibration_list_buff * l = calibration_list_from_file(10);
	//calibration_list_buff_print(l);
	//calibration_list_buff_free(l);
}


