#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef enum {EAST = 0, SOUTH = 1, WEST = 2, NORTH = 3, NA} dir;

void dir_print(dir d) {
	switch (d) {
		case EAST: printf("E\n"); break;	
		case SOUTH: printf("S\n"); break;	
		case WEST: printf("W\n"); break;	
		case NORTH: printf("N\n"); break;	
		case NA: printf("error\n"); exit(0);
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
	if (x < 0 || y < 0) {
		return;
	} else if (x >= m->len_x || y >= m->cur_y) {
		return;	
	}
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

typedef struct node {
	int previous_nodes_x[3];	
	int previous_nodes_y[3];	
	dir previous_nodes_d[3];	
	int costs[3];
} node;

typedef struct grid {
	int x;
	int y;
	node * data;
	int start_x;
	int start_y;
	int end_x;
	int end_y;
} grid;

node * grid_get_node(grid * g, int x, int y, dir d) {
	if (x < 0 || y < 0) {
		return NULL;
	} else if (x >= g->x || y >= g->y) {
		return NULL;
	}
	return &g->data[d + x*4 + y*g->x*4];
}

grid * grid_create (maze * m) {
	int n_elements = m->len_x * m->cur_y * 4;
	grid * g = calloc(1, sizeof(grid));
	if (g == NULL) {
		printf("Not good\n");exit(0);
	}
	g->x = m->len_x;
	g->y = m->cur_y;
	g->data = malloc(sizeof(node) * n_elements);
	if (g->data == NULL) {
		printf("Not good\n");exit(0);
	}
	for (int y = 0; y < g->y; y++) {
		for (int x = 0; x < g->x; x++) {
			for (int d = 0; d < 4; d++) {
				node * local = grid_get_node(g, x, y, d);
				for (int i = 0; i < 3; i++) {
					local->costs[i] = 100000000;
					local->previous_nodes_x[i] = -1;
					local->previous_nodes_y[i] = -1;
					local->previous_nodes_d[i] = NA;
				}
				if (maze_get(m, x, y) == 'S') {
					if (d == EAST) {
						local->costs[0] = 0;
					}
					g->start_x = x;	
					g->start_y = y;	
				}
				if (maze_get(m, x, y) == 'E') {
					g->end_x = x;		
					g->end_y = y;	
				}
			}
		}
	}

	return g;
}

void grid_print(grid * g) {
	for (int y = 0; y < g->y; y++) {
		for (int x = 0; x < g->x; x++) {
			for (int d = 0; d < 4; d++) {
				node * local = grid_get_node(g, x, y, d);
				printf("---");
			}
			printf("-");
		}
		printf("\n");
		for (int x = 0; x < g->x; x++) {
			for (int d = 0; d < 4; d++) {
				node * local = grid_get_node(g, x, y, d);
				if (local->costs[0] > 10000) {
					printf("   ");
				} else {
					printf("%02d,", local->costs[0]/100);
				}
			}
			printf("|");
		}
		printf("\n");
		for (int x = 0; x < g->x; x++) {
			for (int d = 0; d < 4; d++) {
				node * local = grid_get_node(g, x, y, d);
				if (local->costs[0] > 10000) {
					printf("   ");
				} else {
					printf("%02d,", local->costs[0]%100);
				}
			}
			printf("|");
		}
		printf("\n");
	}
	printf("\n");
}

void previous_nodes_replace(node * n, int x, int y, dir d) {
	for (int i = 0; i< 3; i++) {
		n->previous_nodes_x[i] = -1;	
		n->previous_nodes_y[i] = -1;	
		n->previous_nodes_d[i] = NA;
	}
	n->previous_nodes_x[0] = x;
	n->previous_nodes_y[0] = y;
	n->previous_nodes_d[0] = d;
}

void previous_nodes_add(node * n, int x, int y, dir d) {
	int i = 0; 
	while (n->previous_nodes_x[i] == -1 && i < 2) {i ++;};
	n->previous_nodes_x[i] = x;
	n->previous_nodes_y[i] = y;
	n->previous_nodes_d[i] = d;
}
		
void path_add(node * n, int x, int y, dir d, int c) {
	//for (int i = 0; i < 3; i++) {
	//	printf("(%d,%d,%d,%d), ", n->costs[i], n->previous_nodes_x[i], n->previous_nodes_y[i], n->previous_nodes_d[i]);
	//}
	//printf("\t");

	    for (int i = 2; i > 0; i--) {
		n->costs[i] = n->costs[i - 1];
		n->previous_nodes_y[i] = n->previous_nodes_y[i - 1];
		n->previous_nodes_x[i] = n->previous_nodes_x[i - 1];
		n->previous_nodes_d[i] = n->previous_nodes_d[i - 1];
	    }

	    // Now set the new values at index 0
	    n->costs[0] = c;
	    n->previous_nodes_y[0] = y;
	    n->previous_nodes_x[0] = x;
	    n->previous_nodes_d[0] = d;
	
	//for (int i = 0; i < 3; i++) {
	//	printf("(%d,%d,%d,%d), ", n->costs[i], n->previous_nodes_x[i], n->previous_nodes_y[i], n->previous_nodes_d[i]);
	//}
	//printf("\n");
}
		
bool step_to(grid * g, maze * m, int prev_x, int prev_y, dir prev_d, int next_x, int next_y, dir next_d, int cost) {
	node * local = grid_get_node(g, next_x, next_y, next_d);

	if (cost < local->costs[0]) {
		//previous_nodes_replace(local, prev_x, prev_y, prev_d);
		path_add(local, prev_x, prev_y, prev_d, cost);
		return true;
	} else if (cost == local->costs[0]) {
		//printf("Should add. ");
		path_add(local, prev_x, prev_y, prev_d, cost);
		//previous_nodes_add(local, prev_x, prev_y, prev_d);
		return false;	
	}

	return false;
};

void grid_step(grid * g, maze * m, int cur_x, int cur_y, dir cur_d) {
	//grid_print(g);
	//getchar();
	int x_ahead = cur_x, y_ahead = cur_y;
	dir next_dir, prev_dir;	

	switch (cur_d) {
		case NORTH: y_ahead --; next_dir = EAST; prev_dir = WEST; break;	
		case SOUTH: y_ahead ++; next_dir = WEST; prev_dir = EAST; break;	
		case EAST: x_ahead ++; next_dir = NORTH; prev_dir = SOUTH; break;	
		case WEST: x_ahead --; next_dir = SOUTH; prev_dir = NORTH; break;	
		case NA: printf("error\n"); exit(0);
	}

	char c_ahead = maze_get(m, x_ahead, y_ahead);
	int current_cost = grid_get_node(g, cur_x, cur_y, cur_d)->costs[0];
	//printf("Current: %d, %d, Local cost: %d, Ahead: %c\n", cur_x, cur_y, current_cost, c_ahead);

	//always start by going ahdead since this is the cheapest
	if (c_ahead != '#') {
		if (step_to(g, m, cur_x, cur_y, cur_d, x_ahead, y_ahead, cur_d, current_cost + 1)) {
			grid_step(g, m, x_ahead, y_ahead, cur_d);
		}
	}

	//if this is not viable turn
	if (step_to(g, m, cur_x, cur_y, cur_d, cur_x, cur_y, next_dir, current_cost + 1000)) {
		grid_step(g, m, cur_x, cur_y, next_dir);
	}
		
	if (step_to(g, m, cur_x, cur_y, cur_d, cur_x, cur_y, prev_dir, current_cost + 1000)) {
		grid_step(g, m, cur_x, cur_y, prev_dir);
	}
	
}

int grid_get_final_cost(grid * g) {
	int min = 1000000000;
	for (int i =0; i < 4;i++) {
		int c = grid_get_node(g, g->end_x, g->end_y, i)->costs[0];
		if (c < min) {
			min = c;
		}
	}
	return min;
}

void mark_best_path(grid * g, maze * m, int cur_x, int cur_y, int cur_d, int * count) {
	//maze_print(m);
	//getchar();
	printf("%d\n", *count);

	char cur = maze_get(m, cur_x, cur_y);
	//printf("Count: %d\n", *count);

	if (cur_x == g->start_x && cur_y == g->start_y) {
		if (cur == 'S') {
			printf("START DETECTED\n");
			maze_set(m, cur_x, cur_y, '!');
			(*count)++;
		} else {
			printf("START DETECTED BAD\n");
		}
		return;
	} else if (cur != '!') {
		(*count)++;
		maze_set(m, cur_x, cur_y, '!');
	}


	node * n = grid_get_node(g, cur_x, cur_y, cur_d);
	if (n == NULL) {
		printf("Error\n");
		exit(0);
	}

	int min_cost = n->costs[0];

	//for (int i = 0; i < 3; i++) {
	//	if (n->costs[i] == min_cost) {
	//		printf("M");
	//	}
	//	printf("(%d,%d,%d,%d)", n->costs[i], n->previous_nodes_x[i], n->previous_nodes_y[i], n->previous_nodes_d[i]);
	//}
	//printf("\n");

	for (int i = 0; i < 3; i++) {
		if (n->costs[i] != min_cost && min_cost != 100000000 && min_cost != 0) {
			n->previous_nodes_x[i] = -1; 
			n->previous_nodes_y[i] = -1;
			n->previous_nodes_d[i] = -1;
			n->costs[i] = 100000000;
		}
	}

	for (int i = 0; i < 3; i++) {
		if (n->costs[i] == min_cost && min_cost != 100000000 && min_cost != 0) {
			//getchar();
			int px = n->previous_nodes_x[i];
			int py = n->previous_nodes_y[i];
			int di = n->previous_nodes_d[i];

			n->previous_nodes_x[i] = -1;
			n->previous_nodes_y[i] = -1;
			n->previous_nodes_d[i] = -1;
			n->costs[i] = 100000000;

			mark_best_path(g, m, px, py, di, count);
			//kill the path once it has been visited? (and if its the cheapest make sure to kill all others)
			//should get rid of endless cycle
		}
	}
}

int main() {
	maze * m = maze_from_file();
	maze_print(m);
	grid * g = grid_create(m);
	grid_step(g, m, g->start_x, g->start_y, EAST);
	int min_cost = grid_get_final_cost(g);
	printf("Min cost: %d\n", min_cost);
	//grid_print(g);
	int count = 0;

	//4 directions
	for (int i = 0; i < 4; i++) {
		node * n = grid_get_node(g, g->end_x, g->end_y, i);
		if (n->costs[0] == min_cost) {
			printf("N starts\n");
			mark_best_path(g, m, g->end_x, g->end_y, i, &count);
		}
	}
	
	printf("Cound: %d\n", count);
	//maze_print(m);
}
