#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INF 999999999
typedef enum {T = 0,R = 1,B = 2,L = 3} direction;

typedef struct cost {
	int x; 
	int y;
	int count;
	struct cost * next;
} cost;

cost * cost_create(int origin_x, int origin_y, int val) {
	cost * c = calloc(1, sizeof(cost));
	c->x = origin_x;
	c->y = origin_y;
	c->count = val;
	c->next = NULL;
	return c;
}

typedef struct maze {
	char ** data;
	cost ** costs;
	int x;
	int y;
	int y_max;
	int start_x;
	int start_y;
	int end_x;
	int end_y;
} maze;

maze * maze_create() {
	maze * new = calloc(1, sizeof(maze));
	new->x = 0;
	new->y = 0;
	new->y_max = 64;
	new->data = calloc(new->y_max, sizeof(char*));
	new->costs =NULL;
	return new;
}

void maze_push_line(maze * m, char * buff) {
	m->data[m->y++] = buff;
	if (m->y == m->y_max) {
		m->y_max *= 2;
		m->data = realloc(m->data, m->y_max*sizeof(char*));
	}	
}

void maze_finalize(maze * m) {
	m->costs = calloc(m->x*m->y, sizeof(cost *));
	for (int y = 0; y < m->y; y++) {
		for (int x = 0; x < m->x; x++) {
			if (m->data[y][x] == 'S') {
				m->start_x = x;
				m->start_y = y;
				m->costs[x + y*m->x] = cost_create(x, y, 0);
			} else if (m->data[y][x] == 'E') {
				m->end_x = x;
				m->end_y = y;
			}
		}
	}		
}

int next_x(direction d, int x) {
	switch (d) {
		case T: return x;
		case B: return x;
		case R: return x+1;
		case L: return x-1;
		default: printf("Bad dir\n"); exit(0);
	}
}

int next_y(direction d, int y) {
	switch (d) {
		case T: return y-1;
		case B: return y+1;
		case R: return y;
		case L: return y;
		default: printf("Bad dir\n"); exit(0);
	}
}

char maze_get_data(maze * m, int x, int y) {
	if (x < 0 || y < 0) {
		return '\0';
	} else if (x >= m->x || y >= m->y) {
		return '\0';
	} else {
		return m->data[y][x];
	}
}

void maze_set_data(maze * m, int x, int y, char data) {
	if (x < 0 || y < 0) {
		return;
	} else if (x >= m->x || y >= m->y) {
		return;
	} else {
		m->data[y][x] = data;
	}
}

void maze_print_data(maze * m) {
	for (int y = 0; y < m->y; y++) {
		for (int x = 0; x < m->x; x++) {
			printf("%c", m->data[y][x]);
		}
		printf("\n");
	}		
	printf("\n");
}

int maze_get_cost_val(maze * m, int x, int y) {
	if (x < 0 || y < 0) {
		return INF;
	} else if (x >= m->x || y >= m->y) {
		return INF;
	} else {
		cost * c = m->costs[x + y * m->x];
		if (c != NULL) {
			return c->count;
		} else {
			return INF;
		}
	}
}

cost * maze_get_cost(maze * m, int x, int y) {
	if (x < 0 || y < 0) {
		return NULL;
	} else if (x >= m->x || y >= m->y) {
		return NULL;
	} else {
		cost * c = m->costs[x + y * m->x];
		return c;
	}
}

void maze_set_cost(maze * m, int x, int y, int val, int sx, int sy) {
	if (x < 0 || y < 0) {
		return;
	} else if (x >= m->x || y >= m->y) {
		return;
	} else {
		cost * new = cost_create(sx, sy, val);
		new->next = m->costs[x + y * m->x];
		m->costs[x + y * m->x] = new;
	}
}

void maze_print_cost(maze * m) {
	for (int y = 0; y < m->y; y++) {
		for (int x = 0; x < m->x; x++) {
			int c = maze_get_cost_val(m, x, y);
			int type = maze_get_data(m, x, y);
			if (c != INF && type == '#') {
				printf("XX ");	
			} else if (c != INF) {
				printf("%02d ", c);
			} else if (type == '#') {
				printf("## ");	
			} else {
				printf("-- ");	
			}
		}
		printf("\n");
	}		
	printf("\n");
}

maze * maze_from_file() {
	FILE * f = fopen("./input20.txt", "r");
	char * buff = NULL;
	unsigned long size;
	int len;
		
	maze * m = maze_create();	

	while ((len = getline(&buff, &size, f)) != EOF) {
		if (m->x == 0) {
			m->x = len-1;
		}
		maze_push_line(m, buff);
		buff = NULL;
	}
	
	maze_finalize(m);
	fclose(f);
	return m;
}

bool step_to(maze * m, int prev_x, int prev_y, int next_x, int next_y, int prev_cost) {
	int cost = maze_get_cost_val(m, next_x, next_y);

	if (prev_cost < cost) {
		maze_set_cost(m, next_x, next_y, prev_cost, prev_x, prev_y);	
		return true;
	} else if (prev_cost == cost) {
		maze_set_cost(m, next_x, next_y, prev_cost, prev_x, prev_y);	
		return false;
	} else {
		return false;
	}
}

void step(maze * m, int cur_x, int cur_y) {
	int surrounding_x[4];
	int surrounding_y[4];
	char surrounding_chars[4];

	for (int i = 0; i < 4; i++) {
		surrounding_x[i] = next_x(i, cur_x);
		surrounding_y[i] = next_y(i, cur_y);
		surrounding_chars[i] = maze_get_data(m, surrounding_x[i], surrounding_y[i]);
	}

	int local_cost = maze_get_cost_val(m, cur_x, cur_y);

	for (int i = 0; i < 4; i++) {
		if (surrounding_chars[i] != '#' && surrounding_chars[i] != '\0') {
			if (step_to(m, cur_x, cur_y, surrounding_x[i], surrounding_y[i], local_cost + 1)) {
				step(m, surrounding_x[i], surrounding_y[i]);
			}
		}
	}
}

int next_x_r(direction d, int x, int depth) {
	for (int i = 0; i < depth; i++) {
		x = next_x(d, x);
	}
	return x;
}

int next_y_r(direction d, int y, int depth) {
	for (int i = 0; i < depth; i++) {
		y = next_x(d, y);
	}
	return y;
}

bool can_step(char c) {
	if (c == '.' || c == 'S' || c == 'E') {
		return true;
	}
	return false;
}

//don't think backtracking is necessary right now
int count_shortcuts(maze * m) {
	int count = 0;
	for (int y = 0; y < m->y; y++) {
		for (int x = 0; x < m->x; x++) {
			//getchar();
			int type = maze_get_data(m, x, y) ;
			if (type == 'E' || type == '.' || type == 'S') {

				int local_cost = maze_get_cost_val(m, x, y);
				printf("Point: %d,%d\n", x,y);
				int x_neighbours[4];
				int y_neighbours[4];
				char n_type[4];
				for (int i = 0; i < 4; i++) {
					x_neighbours[i] = next_x(i, x);
					y_neighbours[i] = next_y(i, y);
					n_type[i] = maze_get_data(m, x_neighbours[i], y_neighbours[i]);
					printf("\tSub-Point: %d,%d,%c\n", x_neighbours[i],y_neighbours[i], n_type[i]);

					if (n_type[i] == '#') {
						int superx = next_x(i, x_neighbours[i]);
						int supery = next_y(i, y_neighbours[i]);
						char superc = maze_get_data(m, superx, supery);
						//printf("\t\tpoint is interesting. Next point: %d,%d,%c\n", superx, supery, superc);
						if (superc == '.' || superc == 'E' || superc == 'S') {
							int cost = maze_get_cost_val(m, superx, supery);
							//printf("\t\t\tPoint is a shortcut. Origional cost: %d -> %d\n", cost-local_cost, 2);
							if (cost-local_cost > 0) {
								//add the shortcut
								if (cost - local_cost >= 102) {
									count ++;
								}
							}
						}
					}
				}

					
			}
		}
	}		

	return count;
}

int count_shortcuts_advanced(maze * m) {
	int count = 0;
	for (int y = 0; y < m->y; y++) {
		for (int x = 0; x < m->x; x++) {
			//getchar();
			int type = maze_get_data(m, x, y) ;
			if (type == 'E' || type == '.' || type == 'S') {

				int local_cost = maze_get_cost_val(m, x, y);
				printf("Point: %d,%d\n", x,y);

				for (int xp = 0; xp <= 40; xp ++) {
					for (int yp = 0; yp <= 40; yp ++) {

						int cheat_cost = abs(xp -20) + abs(yp-20);

						if (cheat_cost <= 20) {
							int new_cost = maze_get_cost_val(m, x + xp - 20, y + yp - 20);

							if (new_cost != INF) {
								//printf("\t checking: %d, %d. Cost: %d\n", x + xp-10, y + yp-10, new_cost);
								int old_cost = new_cost - local_cost;

								if ( old_cost > 0 && cheat_cost < old_cost) {
									//valid cheat
									if (old_cost - cheat_cost >= 100) {
										count ++;
									}
								}
							}

						}

					}
				}

					
			}
		}
	}		

	return count;
}

int main() {
	maze * m = maze_from_file();
	maze_print_data(m);
	maze_print_cost(m);
	step(m, m->start_x, m->start_y);
	maze_print_cost(m);
	int c = count_shortcuts_advanced(m);
	printf("Final count: %d\n", c);
}

//part two looks really straight forward. only some things to condier:
//the 20 picosecond thing is of course fucking weird
//may need to consider if something is usefull for getting to the optimal path and not just the highest possible cost (try to ignore this first)
