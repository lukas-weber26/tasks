#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

int global_x = 101;
int global_y = 103;

typedef struct robot {
	int xpos;
	int ypos;
	int xvel;
	int yvel;
} robot;

void robot_print(robot * r) {
	printf("Pos: %d,%d Vel: %d,%d\n", r->xpos, r->ypos, r->xvel, r->yvel);
}

robot * robot_create(int x, int y, int xv, int yv) {
	robot * r = calloc(1, sizeof(robot));
	r->xpos = x;
	r->ypos = y;
	r->xvel= xv;
	r->yvel= yv;
	return r;
}

void robot_move(robot * r) {
	r->xpos += r->xvel;
	r->ypos += r->yvel;

	while (r->ypos < 0) {
		r->ypos += global_y;	
	}
	
	while (r->ypos >= global_y) {
		r->ypos -= global_y;	
	}
	
	while (r->xpos < 0) {
		r->xpos += global_x;	
	}
	
	while (r->xpos >= global_x) {
		r->xpos -= global_x;	
	}

}

typedef struct list {
	void ** data;
	int cur;
	int max;
} list;

list * list_create() {
	list * l = calloc(1, sizeof(list));
	l->cur = 0;
	l->max = 64;
	l->data = calloc(64, sizeof(void *));
	return l;
}

void list_push(list * list, void * data) {
	list->data[list->cur++] = data;
	if (list->cur == list->max) {
		list->max *= 2;
		list->data = realloc(list->data, list->max*sizeof(void *));
	}
}

int list_calculate_safety(list * l) {
	int quadrant[4] = {0,0,0,0};

	for (int i = 0; i < l->cur; i++) {
		int x = ((robot*)(l->data[i]))->xpos;
		int y = ((robot*)(l->data[i]))->ypos;

		if (y < global_y/2 && x < global_x/2) {
			quadrant[0] ++;	
		}
		
		if (y > global_y/2 && x < global_x/2) {
			quadrant[1] ++;	
		}
		
		if (y < global_y/2 && x > global_x/2) {
			quadrant[2] ++;	
		}
		
		if (y > global_y/2 && x > global_x/2) {
			quadrant[3] ++;	
		}

	}	

	return quadrant[0]*quadrant[1]*quadrant[2]*quadrant[3];
}

char * buff_skip_number(char * b) {
	while (isdigit(*(b)) || *b == '-') {
		b ++;
	}
	return b;
}

char * buff_skip_to_number(char * b) {
	while (!isdigit(*(b)) && *b != '-') {
		b ++;
	}
	return b;
}

list * list_from_file() {
	FILE * f = fopen("./input14.txt", "r");
	list * l = list_create();

	char * buff = NULL, *buff_cpy = NULL;
	unsigned long size;
	int len;
	
	while ((len = getline(&buff, &size, f)) != -1) {

		buff_cpy = buff_skip_to_number(buff);
		int x = atoi(buff_cpy);
		buff_cpy = buff_skip_number(buff_cpy);
		buff_cpy = buff_skip_to_number(buff_cpy);
		int y = atoi(buff_cpy);
		buff_cpy = buff_skip_number(buff_cpy);
		buff_cpy = buff_skip_to_number(buff_cpy);
		int xv = atoi(buff_cpy);
		buff_cpy = buff_skip_number(buff_cpy);
		buff_cpy = buff_skip_to_number(buff_cpy);
		int yv = atoi(buff_cpy);

		robot * r = robot_create(x,y,xv,yv);
		list_push(l, (void *) r);

		free(buff);
		buff = NULL;
	}

	fclose(f);
	return l;
}

void robots_print(list * l) {
	for (int i = 0; i < l->cur; i++) {
		robot_print(l->data[i]);
	}
}

void list_robots_move(list * l, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < l->cur; j++) {
			robot_move(l->data[j]);
		}
	}
}

void print_tree(list * l) {
	int size = global_x * global_y;
	char * data = calloc(size, sizeof(char));
	
	for (int i = 0; i < l->cur; i++) {
		robot * r = l->data[i];
		data[r->xpos + r->ypos * global_x] = 'x';
	}

	for (int i = 0; i < global_y; i++) {
		for (int j = 0; j < global_x; j++) {
			char c = data[j + i * global_x];
			if (c == 'x') {
				putchar('X');
			} else {
				putchar(' ');
			}
		}
		printf("\n");
	}
	printf("\n");
}

int list_x_avg(list * l) {
	int sum = 0;
	for (int i = 0; i < l->cur; i++) {
		sum += ((robot *)(l->data[i]))->xpos;
	}
	return sum/l->cur;
}

int list_x_variance(list * l) {
	int avg = list_x_avg(l);
	int var = 0;
	int total = 0;
	for (int i = 0; i < l->cur; i++) {
		int x = ((robot *)(l->data[i]))->xpos;
		var += (x - avg)*(x-avg);
		total ++;
	}

	return var/total;
}

int main () {
	list * l = list_from_file();
	int i = 0;
	
	int avg = list_x_variance(l);


	while (1) {
		list_robots_move(l, 1); 
		i++;
		//print_tree(l);
		int local_avg = list_x_variance(l);
		if (abs(avg - local_avg) > 300) {
			printf("moved %d times. Avg: %d\n", i, local_avg);
			print_tree(l);
			getchar();
		}
	}

	//printf("Final score: %d\n", list_calculate_safety(l));
}
