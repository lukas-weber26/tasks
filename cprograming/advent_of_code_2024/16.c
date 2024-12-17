#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef enum {EAST = 0, SOUTH = 1, WEST = 2, NORTH = 3} dir;

void dir_print(dir d) {
	switch (d) {
		case EAST: printf("E\n"); break;	
		case SOUTH: printf("S\n"); break;	
		case WEST: printf("W\n"); break;	
		case NORTH: printf("N\n"); break;	
	}
}

typedef struct maze {
	char ** data;
	int len_x;
	int cur_y;
	int max_y;
} maze;

maze * maze_create() {
	maze * m = calloc(1, sizeof(maze));
	m->len_x = 0;
	m->cur_y = 0;
	m->max_y = 64;
	m->data = calloc(m->max_y, sizeof(char*));
	return m;
}

void maze_push(maze * m, char * data) {
	m->data[m->cur_y++] = data;
	if (m->cur_y == m->max_y) {
		m->max_y *= 2;
		m->data = realloc(m->data, sizeof(char *) * m->max_y);
	}
}

char maze_get(maze * m, int x, int y) {
	if (x < 0 || y < 0) {
		return '#';
	} else if (x >= m->len_x || y >= m->cur_y) {
		return '#';
	}
	return m->data[y][x];	
}

void maze_set(maze * m, int x, int y, char val) {
	m->data[y][x] = val;	
}

void maze_print(maze * m) {
	for (int i = 0; i < m->cur_y; i++) {
		for (int j = 0; j < m->len_x; j++) {
			printf("%c", m->data[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

maze * maze_from_file() {
	maze * m = maze_create();
	FILE * f = fopen("./input16.txt", "r");
	char * buff = NULL;
	unsigned long size;
	int len;	

	while ((len = getline(&buff, &size, f)) != -1) {
		if (m->len_x == 0) {
			m->len_x = len-1;	
		}
		maze_push(m, buff);
		buff = NULL;
	}

	fclose(f);
	return m;
}

typedef struct costs {
	int N;
	int E;
	int S;
	int W;
} costs;

int fun1 (int n) {
	return (n/1000)%10;
}
int fun2 (int n) {
	return (n %1000)%10;
}

void costs_print(costs c) {
	if (c.E == -1) {
		printf("00,00,00,00|");
	} else {
		printf("%d%d,%d%d,%d%d,%d%d|", fun1(c.E), fun2(c.E), fun1(c.S), fun2(c.S), fun1(c.W),fun2(c.W), fun1(c.N), fun2(c.N));
	}
	//printf("%d,%d,%d,%d|", c.E, c.S, c.W, c.N);
	//printf("(%d, %d) ", c.E, c.N);
}

void costs_print_thousands(costs c) {
	if (c.E == -1) {
		printf("0,0,0,0|");
	} else {
		printf("%d,%d,%d,%d|", c.E/1000, c.S/1000, c.W/1000, c.N/1000);
	}
}

void costs_print_ones(costs c) {
	if (c.E == -1) {
		printf("00,00,00,00|");
	} else {
		printf("%02d,%02d,%02d,%02d|", c.E%1000, c.S%1000, c.W%1000, c.N%1000);
	}
}

typedef struct cost_table {
	costs * data;
	int x;
	int y;
} cost_table;

cost_table * cost_table_create(maze * m, int * cur_x, int * cur_y) {
	cost_table * t = calloc(1, sizeof(cost_table));
	t->x = m->len_x;
	t->y = m->cur_y;
	int xy = t->x * t->y;
	t->data = malloc(sizeof(costs) * xy);
	for (int i = 0; i < xy; i++) {
		t->data[i].E = -1;
		t->data[i].W = -1;
		t->data[i].N = -1;
		t->data[i].S = -1;
	}

	for (int i = 0; i < m->cur_y; i++) {
		for (int j = 0; j < m->len_x; j++) {
			if (m->data[i][j] == 'S') {
				t->data[i*t->x + j].E = 0;
				t->data[i*t->x + j].W = 2000;
				t->data[i*t->x + j].N = 1000;
				t->data[i*t->x + j].S = 1000;
				*cur_x = j;
				*cur_y = i;
				break;
			}
		}
	}
	
	return t;
}

void cost_table_print(cost_table * t) {
	for (int i = 0; i < t->y; i++) {
		for (int j = 0; j < t->x; j++) {
			costs_print(t->data[j + i * t->x]);
		}
		printf("\n");
	}
	printf("\n");
}

void cost_table_print_alt(cost_table * t) {
	for (int i = 0; i < t->y; i++) {
		for (int j = 0; j < t->x; j++) {
			costs_print_ones(t->data[j + i * t->x]);
		}
		printf("\n");
	}
	printf("\n");
	for (int i = 0; i < t->y; i++) {
		for (int j = 0; j < t->x; j++) {
			costs_print_thousands(t->data[j + i * t->x]);
		}
		printf("\n");
	}
	printf("\n");
}

costs * table_get(cost_table * t, int x, int y) {
	return &t->data[x + y*t->x];
}

bool update (cost_table * t, int cur_x, int cur_y, int next_cost, dir direction) {
	int east_offset = 0, west_offset = 0, north_offset = 0, south_offset = 0;	
	
	if (direction == NORTH) {
		cur_y -= 1;
		east_offset = 1000;
		west_offset = 1000;
		north_offset = 0;
		south_offset = 2000;
	} else if (direction == SOUTH) {
		cur_y += 1;
		east_offset = 1000;
		west_offset = 1000;
		north_offset = 2000;
		south_offset = 0;
	} else if (direction == EAST) {
		cur_x += 1;
		east_offset = 0;
		west_offset = 2000;
		north_offset = 1000;
		south_offset = 1000;
	} else if (direction == WEST) {
		cur_x -= 1;
		east_offset = 2000;
		west_offset = 0;
		north_offset = 1000;
		south_offset = 1000;
	}

	costs * cost = table_get(t, cur_x, cur_y);
	bool update = false;
	if ((next_cost + east_offset < cost->E) || (cost->E == -1)) { 
		cost->E = next_cost + east_offset;
		update = true;
	} 
	if ((next_cost + west_offset < cost->W) || (cost->W == -1)) { 
		cost->W = next_cost + west_offset;
		update = true;
	} 
	if ((next_cost + north_offset < cost->N) || (cost->N == -1)) { 
		cost->N = next_cost + north_offset;
		update = true;
	} 
	if ((next_cost + south_offset < cost->S) || (cost->S == -1)) { 
		cost->S = next_cost + south_offset;
		update = true;
	} 

	return update;
}

void step(maze * m,cost_table * t, int cur_x, int cur_y, dir blockdir) {
	cost_table_print_alt(t);
	//getchar();
	char left = maze_get(m, cur_x-1, cur_y);
	char right = maze_get(m, cur_x+1, cur_y);
	char top = maze_get(m, cur_x, cur_y - 1);
	char bottom = maze_get(m, cur_x, cur_y + 1);

	if (left != '#' && blockdir != WEST) {
		int left_cost = t->data[cur_x + cur_y *t->x].W;
		int next_cost = left_cost + 1;
		printf("Left is valid. %d, %d\n", left, next_cost);
		if (update(t, cur_x, cur_y, next_cost, WEST)) {
			step(m, t, cur_x - 1, cur_y, EAST);
		}
	}
	if (right != '#' && blockdir != EAST) {
		int right_cost = t->data[cur_x + cur_y *t->x].E;
		int next_cost = right_cost + 1;
		printf("Right is valid. %d, %d\n", right_cost, next_cost);
		if (update(t, cur_x, cur_y, next_cost, EAST)) {
			step(m, t, cur_x + 1, cur_y, WEST);
		}
	}
	if (top != '#' && blockdir != NORTH) {
		int top_cost = t->data[cur_x + cur_y *t->x].N;
		int next_cost = top_cost + 1;
		printf("Top is valid. %d, %d\n", top_cost, next_cost);
		if (update(t, cur_x, cur_y, next_cost, NORTH)) {
			step(m, t, cur_x, cur_y-1, SOUTH);
		}
	}
	if (bottom != '#' && blockdir != SOUTH) {
		int bottom_cost = t->data[cur_x + cur_y *t->x].S;
		int next_cost = bottom_cost + 1;
		printf("Bottom is valid. %d, %d\n", bottom_cost, next_cost);
		if (update(t, cur_x, cur_y, next_cost, SOUTH)) {
			step(m, t, cur_x, cur_y+1, NORTH);
		}
	}
}

int get_final_cost(maze * m, cost_table * t) {
	for (int i = 0; i < m->cur_y; i ++) {
		for (int j = 0; j < m->len_x; j ++) {
			if (maze_get(m, j, i) == 'E') {
				costs * c = table_get(t, j, i);
				int l [4] = {c->E, c->N, c->S, c->W};
				int min = l[0];
				for (int z = 1; z < 4; z++) {
					if (l[z] < min) {
						min = l[z];
					}
				}
				return min;
			}
		}
	}
	return -1;
}

int main() {
	maze * m = maze_from_file();
	maze_print(m);
	int cur_x, cur_y;
	cost_table * t = cost_table_create(m, &cur_x, &cur_y);
	//cost_table_print(t);
	printf("Cur x, y: %d,%d\n", cur_x, cur_y);
	step(m, t, cur_x, cur_y, -1);
	cost_table_print_alt(t);
	printf("Final cost: %d\n",get_final_cost(m, t));
}
