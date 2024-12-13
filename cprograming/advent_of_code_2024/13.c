#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

typedef struct coord {
	long long int x;
	long long int y;
} coord;

typedef struct machine {
	coord a;
	coord b;
	coord prize;
	long long int min_cost;
} machine;

long long int calculate_min_2(machine m) {

	long double A,B,C,D;
	A = (long double)m.a.x;
	B = (long double)m.b.x;
	C = (long double)m.a.y;
	D = (long double)m.b.y;
	long double determinant = A *D -B *C;
	//printf("Determinant: %Lf\n", determinant);
	if (roundl(determinant) == 0) {
		printf("No unique solution\n");
		exit(0);
	}
	
	//check linear independence. This is not working
	long long int axy = (long long int) round((long double)m.a.x/(long double)m.a.y);		
	long long int bxy = (long long int) round((long double)m.b.x/(long double)m.b.y);		
	long long int pxy = (long long int) round((long double)m.prize.x/(long double)m.prize.y);

	if (axy == bxy && axy == pxy) {
		printf("Dependence detected\n");
		exit(0);
	}

	//given linear independence, solve
	long double e1a	= (long double)m.a.x/(long double)m.a.x;	
	long double e1b = (long double)m.b.x/(long double)m.a.x;	
	long double e1p = (long double)m.prize.x/(long double)m.a.x;	
	
	long double e2a	= (long double)m.a.y/(long double)m.a.y;	
	long double e2b = (long double)m.b.y/(long double)m.a.y;	
	long double e2p = (long double)m.prize.y/(long double)m.a.y;	
	
	long double e3b = e2b - e1b;
	long double e3p = e2p - e1p;
	
	long double b_f = e3p/e3b;
	long double a_f = e1p - b_f * e1b;

	long long int b_fi = (long long int)roundl(b_f);
	long long int a_fi = (long long int)roundl(a_f);
	
	if (a_fi >= 0.0 && b_fi >= 0.0) {
		if (a_fi * m.a.x + b_fi * m.b.x == m.prize.x) {
			if (a_fi * m.a.y + b_fi * m.b.y == m.prize.y) {
			//printf("VALID\n");
			long long int cost = a_fi*3 + 1*b_fi;
			return cost;
			}
		}
	}

	return 0;	

}

long long int calculate_min(machine m) {
	for (long long int i = 0; (i*m.a.x <= m.prize.x) && (i*m.a.y <= m.prize.y); i++) {
		long long int cur_x = i*m.a.x; 
		long long int cur_y = i*m.a.y;
		for (long long int j = 0; (j*m.b.x + cur_x <= m.prize.x) && (j*m.b.y + cur_y <= m.prize.y); j++) {
			long long int total_x = cur_x + j*m.b.x; 
			long long int total_y = cur_y + j*m.b.y;
			//printf("%lld,%lld\n",i,j);
			if (total_x == m.prize.x && total_y == m.prize.y) {
				long long int cost = i*3 + 1*j;
				if (m.min_cost < 0) {
					m.min_cost = cost;
				} else if (m.min_cost > cost) {
					m.min_cost = cost;
				}
			}
		}
	}
	m.min_cost = m.min_cost > 0 ? m.min_cost : 0;
	return m.min_cost;
}

typedef struct list {
	long long int * data;
	int cur;
	int max;
} list;

list * list_create() {
	list * new = calloc(1, sizeof(list));
	new->max = 64;
	new->cur = 0;
	new->data = calloc(new->max, sizeof(long long int));
	return new;
}

void list_push(list * l, long long int val) {
	l->data[l->cur] = val;
	l->cur++;
	if (l->cur == l->max) {
		l->max *= 2;
		l->data = realloc(l->data, l->max*sizeof(long long int));
	}
}

long long int list_sum(list * l) {
	long long int sum = 0;
	for (int i =0; i < l->cur; i++) {
		if (l->data[i] > 0) {
			sum += l->data[i];	
		}
	}
	return sum;
}

long long int part1() {
	list * costs = list_create();

	FILE * f = fopen("./input13.txt", "r");
	char * buff = NULL, *buff_cpy;
	unsigned long size;
	int len, mode = 0;
	long long int x, y;
	machine m;
	long long int total_cost = 0;
	long long int total_cost2 = 0;

	FILE * outfile = fopen("./day13out.txt", "rw");
	
	while ((len = getline(&buff, &size, f)) != -1) {
		buff_cpy = buff;
		switch (mode) {
			case 0: 
				while (!isdigit(*buff_cpy)) {buff_cpy ++;};
				x = atoll(buff_cpy);
				while (isdigit(*buff_cpy)) {buff_cpy ++;};
				while (!isdigit(*buff_cpy)) {buff_cpy ++;};
				y = atoll(buff_cpy);
				m.a.x = x;
				m.a.y = y;
				break;	
			case 1: 
				while (!isdigit(*buff_cpy)) {buff_cpy ++;};
				x = atoll(buff_cpy);
				while (isdigit(*buff_cpy)) {buff_cpy ++;};
				while (!isdigit(*buff_cpy)) {buff_cpy ++;};
				y = atoll(buff_cpy);
				m.b.x = x;
				m.b.y = y;
				break;	
			case 2: 
				while (!isdigit(*buff_cpy)) {buff_cpy ++;};
				x = atoll(buff_cpy) + 10000000000000;
				while (isdigit(*buff_cpy)) {buff_cpy ++;};
				while (!isdigit(*buff_cpy)) {buff_cpy ++;};
				y = atoll(buff_cpy) + 10000000000000;
				m.prize.x = x;
				m.prize.y = y;
				m.min_cost = - 1;
				long long int local_cost = 0;//calculate_min(m);	//calculate_min_2(m);	
				long long int local_cost2 = calculate_min_2(m);	
				total_cost += local_cost;
				total_cost2 += local_cost2;
				printf("Type 1, 2: %lld, %lld\n", local_cost, local_cost2);
				//fprintf(outfile, "Type 1: %lld, Type 2 %lld\n", local_cost, local_cost2);
			case 3: break;	
		}

		mode = (mode + 1)%4;
		free(buff);
		buff = NULL;
	}
	
	printf("Cost1,2: %lld, %lld\n", total_cost, total_cost2);

	exit(0);
	fclose(outfile);
	fclose(f);
	return total_cost2;
}

int main () {
	long long int c = part1();

}

