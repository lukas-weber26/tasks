#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INF 99999999

typedef enum {T = 0, R = 1, B = 2, L = 3} direction;

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
	m->costs = calloc(1, sizeof(cost*)*x*y);
	for (int i = 0; i < m->y; i++) {
		for (int j = 0; j < m->x; j++) {
			maze_set(m, j, i, '.');
			m->costs[j + i*m->x] = NULL;
		}
	}
	m->costs[0] = cost_create(0, 0, 0);
	m->start_x = 0;
	m->start_y = 0;
	m->end_x= x-1;
	m->end_y= y-1;
	return m;
}

cost * maze_get_cost_full(maze * m, int x, int y, bool print) {
	if (print)
		printf("Requested val at: %d,%d. ", x,y);

	if (x < 0 || y < 0) {
		printf("Bad position\n");
		exit(0);
	}
	if (x >= m->x || y >= m->y) {
		printf("Bad position\n");
		exit(0);
	}
	cost * c = m->costs[x + y*m->x]; 
	if (c == NULL) {
		if (print)
			printf("Returing inf.");

		return NULL;
	} else {
		if (print)
			printf("Returing %d.\n", c->val);

		return c; 
	}
}

int maze_get_cost_val(maze * m, int x, int y, bool print) {
	if (print)
		printf("Requested val at: %d,%d. ", x,y);

	if (x < 0 || y < 0) {
		printf("Bad position\n");
		exit(0);
	}
	if (x >= m->x || y >= m->y) {
		printf("Bad position\n");
		exit(0);
	}
	cost * c = m->costs[x + y*m->x]; 
	if (c == NULL) {
		if (print)
			printf("Returing inf.");

		return INF;
	} else {
		if (print)
			printf("Returing %d.\n", c->val);

		return c->val; 
	}
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

void costs_print(maze * m) {
	for (int i = 0; i < m->y; i++) {
		for (int j = 0; j < m->x; j++) {
			int val = maze_get_cost_val(m, j, i, false);
			if (val == INF) {
				printf("%3d",-1);
			} else {
				printf("%3d",val);
			}
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

int get_x(int cur_x, direction d) {
	if (d == T || d == B) {
		return cur_x;
	} else if (d == R) {
		return cur_x + 1;
	} else if (d == L) {
		return cur_x - 1;
	}
	printf("Bad direction\n");
	exit(1);
}

int get_y(int cur_y, direction d) {
	if (d == R || d == L) {
		return cur_y;
	} else if (d == T) {
		return cur_y - 1;
	} else if (d == B) {
		return cur_y + 1;
	}
	printf("Bad direction\n");
	exit(1);
}



void maze_push_cost(maze * m, int push_loc_x, int push_loc_y, int new_cost, int source_x, int source_y) {
	if (push_loc_x < 0 || push_loc_y < 0) {
		printf("Bad position\n");
		exit(0);
	}
	if (push_loc_x  >= m->x || push_loc_y >= m->y) {
		printf("Bad position\n");
		exit(0);
	}

	cost * new = cost_create(source_x, source_y, new_cost);
	new->next = m->costs[push_loc_x + push_loc_y*m->x]; 
	m->costs[push_loc_x + push_loc_y*m->x] = new;
}

bool step_to(maze * m, int prev_x, int prev_y, int next_x, int next_y, int prev_cost) {
	//get the cost of the new are
	
	//printf("(step to) Requested val at: %d,%d. ", next_y,y);
	int new_cost = maze_get_cost_val(m, next_x, next_y, false);
	//printf("Old coords: %d,%d, new coords: %d,%d, old cost: %d, new cost: %d\n", prev_x, prev_y, next_x, next_y, prev_cost, new_cost);
	if (prev_cost < new_cost) {
		maze_push_cost(m, next_x, next_y, prev_cost, prev_x, prev_y);
		return true;
	} else if (prev_cost == new_cost) {
		maze_push_cost(m, next_x, next_y, prev_cost, prev_x, prev_y);
		return false;
	} else {
		return false;
	}	
}

void step(maze * m, int cur_x, int cur_y) {
	if (cur_x == m->end_x && cur_y == m->end_y) {
		return;
	}	

	char local_chars[4];
	int local_xcoords[4];
	int local_ycoords[4];

	//get the cost of the current area
	int local_cost = maze_get_cost_val(m, cur_x, cur_y, false);

	//figure out what items are in the area
	for (int i = 0; i < 4; i++) {
		local_xcoords[i] = get_x(cur_x, i);
		local_ycoords[i] = get_y(cur_y, i);
		local_chars[i] = maze_get(m, local_xcoords[i], local_ycoords[i]);
		//printf("(%d,%d),%c\n", local_xcoords[i], local_ycoords[i], local_chars[i]);
	}

	//if the item is valid, go there
	for (int i = 0; i < 4; i++) {
		if (local_chars[i] == '.' || local_chars[i] == 'O') {
			//printf("(%d,%d),%c\n", local_xcoords[i], local_ycoords[i], local_chars[i]);
			if (step_to(m, cur_x, cur_y, local_xcoords[i], local_ycoords[i], local_cost + 1)) {
				step(m, local_xcoords[i], local_ycoords[i]);
			}
			//costs_print(m);
		}
	}
}

//time to backtrack
void backtrack(maze * m, int cur_x, int cur_y, int *count) {
	if (cur_x == m->start_x && cur_y == m->start_y) {
		return;
	}	

	//printf("%d,%d\n", cur_x, cur_y);
	cost * local_cost = maze_get_cost_full(m, cur_x, cur_y, false);
	if (local_cost == NULL) {
		printf("NULL cost error\n");
		exit(0);
	}
	
	(*count) ++;
	backtrack(m, local_cost->x, local_cost->y, count);
}

int main () {
	coord_list * l = coords_from_file();
	maze * m = maze_create(71,71);
	add_n_from_list_to_maze(m, l, 1024);
	//maze_print(m);
	costs_print(m);
	step(m, m->start_x, m->start_y);
	int count = 0;
	backtrack(m, m->end_x, m->end_y, &count);
	printf("Count: %d\n", count);
}
