#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct coord_list {
	int * x;
	int * y;
	int cur_len;
	int max_len;
} coord_list;

void coord_list_print(coord_list * l) {
	for (int i = 0; i < l->cur_len; i++) {
		printf("(%d,%d)\n", l->x[i], l->y[i]);
	}
}

coord_list * coord_list_create() {
	coord_list * c = calloc(1, sizeof(coord_list));
	c->cur_len = 0;
	c->max_len = 64;
	c->x = calloc(64, sizeof(int));
	c->y = calloc(64, sizeof(int));
	return c;
}

void coord_list_push(coord_list * c, int x, int y) {
	c->x[c->cur_len] = x;
	c->y[c->cur_len] = y;
	if (++c->cur_len == c->max_len) {
		c->max_len *= 2;
		c->x = realloc(c->x, c->max_len*sizeof(int));
		c->y = realloc(c->y, c->max_len*sizeof(int));
	}
}

typedef struct cost {
	int x;
	int y; 
	int val;
	struct cost * next;
} cost;

cost * cost_create(int x, int y, int val) {
	cost * c = calloc(1, sizeof(cost));
	c->next = NULL;
	c->x = x;
	c->y = y;
	c->val= val;
	return c;
}

typedef struct maze {
	int x;
	int y;
	char * data;
	cost ** costs;
	int start_x;
	int start_y;
	int end_x;
	int end_y;
} maze;

void maze_set(maze * m, int x, int y, char val) {
	if (x < 0 || y < 0) {
		return;
	}
	if (x >= m->x || y >= m->y) {
		return;
	}
	m->data[x + y*m->x] = val;
}

char maze_get(maze * m, int x, int y) {
	if (x < 0 || y < 0) {
		return '#';
	}
	if (x >= m->x || y >= m->y) {
		return '#';
	}
	return m->data[x + y*m->x];
}

maze * maze_create(int x, int y) {
	maze * m = calloc(1, sizeof(maze));
	m->x = x;
	m->y = y;
	m->data = calloc(1, sizeof(char)*x*y);
	for (int i = 0; i < m->y; i++) {
		for (int j = 0; j < m->x; j++) {
			maze_set(m, j, i, '.');
		}
	}
	m->costs = calloc(1, sizeof(cost*)*x*y);
	m->start_x = 0;
	m->start_y = 0;
	m->end_x= x;
	m->end_y= y;
	return m;
}

void maze_print(maze * m) {
	for (int i = 0; i < m->y; i++) {
		for (int j = 0; j < m->x; j++) {
			printf("%c",maze_get(m, j, i));
		}
		printf("\n");
	}
	printf("\n");
}

coord_list * coords_from_file() {
	coord_list * c = coord_list_create();

	FILE * f = fopen("./input18.txt", "r");
	char * buff = NULL, *buff_cpy = NULL;
	unsigned long size;
	int len;

	while ((len = getline(&buff, &size, f)) != -1) {
		buff_cpy = buff;
		int x = atoi(buff_cpy);
		while (isdigit(*buff_cpy++));
		int y = atoi(buff_cpy);
		coord_list_push(c, x, y);
		free(buff);
		buff = NULL;
	}
	
	//coord_list_print(c);
	return c;
}

void add_n_from_list_to_maze(maze * m, coord_list * l, int n){
	for (int i = 0; i < n; i++) {
		maze_set(m, l->x[i], l->y[i], '#');
	}
}

int main () {
	coord_list * l = coords_from_file();
	maze * m = maze_create(7,7);
	add_n_from_list_to_maze(m, l, 12);
	maze_print(m);
}
