#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

typedef enum {UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3} direction;
typedef enum {WALL, EMPTY, USED, OUT_OF_BOUNDS} point_type;

typedef bool used_dirs[4];

typedef struct maze_dict {
	used_dirs * dirs;	
	int x_size;
	int y_size;
} maze_dict;

maze_dict * maze_dict_create(int x, int y) {
	maze_dict * m = calloc(1, sizeof(maze_dict));
	m->x_size = x;
	m->y_size = y;
	m->dirs = calloc(x*y, sizeof(used_dirs)); 
	return m;
}

void maze_dict_free(maze_dict * md) {
	free(md->dirs);
	free(md);
	md = NULL;
} 

int maze_dict_check_and_add(maze_dict * md, int x, int y, direction d) {
	if (md->dirs[x + md->x_size*y][d]) {
		return 1;
	} else {
		md->dirs[x + md->x_size*y][d] = 1;
		return 0;
	}
}

typedef struct maze {
	int x_loc;
	int y_loc;
	direction dir;
	point_type ** maze;
	int line_length;
	int n_lines;
	int max_lines;
	int distinct_hits;
} maze;

void maze_print(maze * m) {
	printf("Printing maze. Nunique: %d\n", m->distinct_hits);
	for (int j = 0; j < m->n_lines; j++) {
		for (int i = 0; i < m->line_length; i ++) {
			if (m->y_loc == j && m->x_loc == i) {
				switch (m->dir) {
					case UP: printf("^"); break;
					case RIGHT: printf(">"); break;
					case DOWN: printf("v"); break;
					case LEFT: printf("<"); break;
				}
			} else {
				switch (m->maze[j][i]) {
					case WALL: printf("#"); break;
					case EMPTY: printf("."); break;
					case USED: printf("X"); break;
					default:
						printf("Invalid case\n");
						exit(0);
				}
			}
		}
		printf("\n");
	}	
	printf("\n");
}

direction turn(direction d) {
	return (d+1)%4;
}

point_type get_next_type(maze * m, int next_x, int next_y) {
	//if (m->x_loc >= m->line_length - 1 || next_x < 0) {
	//	return OUT_OF_BOUNDS;	
	//} else if (m->y_loc >= m->n_lines - 1 || next_y < 0) {
	//	return OUT_OF_BOUNDS;	
	//} 
	if (next_x >= m->line_length || next_x < 0) {
		return OUT_OF_BOUNDS;	
	} else if (next_y >= m->n_lines || next_y < 0) {
		return OUT_OF_BOUNDS;	
	} 
	return m->maze[next_y][next_x];
}

int maze_move_loop(maze * m, maze_dict * md, bool print, int rcount) {

	if (print) {
		maze_print(m);
	}

	if (rcount-- == 0) {
		return 0;	
	}

	int next_x = m->x_loc, next_y = m->y_loc;
	
	switch (m->dir) {
		case UP:
			next_y -= 1;
			break;
		case RIGHT:
			next_x += 1;
			break;
		case DOWN:
			next_y += 1;
			break;
		case LEFT:
			next_x -= 1;
			break;
		default:
			printf("Invalid case\n");
			exit(0);
	}

	point_type next_type = get_next_type(m, next_x, next_y);

	switch (next_type) {
		case WALL:
			m->dir = turn(m->dir);
			return maze_move_loop(m, md, print, rcount);
		case EMPTY:
			m->distinct_hits ++;
		case USED:
			m->x_loc = next_x;
			m->y_loc = next_y;
			m->maze[next_y][next_x] = USED;
			int loop = maze_dict_check_and_add(md, next_x, next_y, m->dir); //don't like this m->dir
			if (loop == 1) {
				return 1;
			}
			return maze_move_loop(m, md, print, rcount);
		case OUT_OF_BOUNDS: 
			return 0;
		default:
			printf("Invalid case\n");
			exit(0);
	}
	
};

void maze_move(maze * m, bool allow_recursion) {

	int next_x = m->x_loc, next_y = m->y_loc;
	
	switch (m->dir) {
		case UP:
			next_y -= 1;
			break;
		case RIGHT:
			next_x += 1;
			break;
		case DOWN:
			next_y += 1;
			break;
		case LEFT:
			next_x -= 1;
			break;
		default:
			printf("Invalid case\n");
			exit(0);
	}

	point_type next_type = get_next_type(m, next_x, next_y);

	switch (next_type) {
		case WALL:
			m->dir = turn(m->dir);
			if (allow_recursion) {
				maze_move(m, allow_recursion);
			}
			return;
		case EMPTY:
			m->distinct_hits ++;
		case USED:
			m->x_loc = next_x;
			m->y_loc = next_y;
			m->maze[next_y][next_x] = USED;
			if (allow_recursion) {
				maze_move(m, allow_recursion);
			}
			return;
		case OUT_OF_BOUNDS: 
			return;
		default:
			printf("Invalid case\n");
			exit(0);
	}
	
};

void maze_add_row(maze * m, char * line) {

	point_type * new_row = malloc(sizeof(point_type)*m->line_length);

	for (int i = 0; i < m->line_length; i++) {
		switch (line[i]) {
			case '#':
				new_row[i] = WALL;
				break;
			case '.':
				new_row[i] = EMPTY;
				break;
			case '^':
				new_row[i] = USED;
				m->x_loc = i;
				m->y_loc = m->n_lines;
				m->dir = UP;
				break;
			default: 
				printf("Unexpected character: %c\n", line[i]);
				exit(0);
		}
	}

	m->maze[m->n_lines] = new_row;

	m->n_lines ++;
	if (m->n_lines >= m->max_lines) {
		m->max_lines *= 2;
		m->maze = realloc(m->maze, sizeof(point_type *) * m->max_lines);
	}
}

maze * maze_create() {
	maze * m = calloc(1,sizeof(maze));
	FILE * f = fopen("./input6.txt", "r");

	m->max_lines = 100;
	m->n_lines = 0;
	m->distinct_hits = 1;
	m->maze = malloc(sizeof(point_type *) * m->max_lines);

	char * buff = NULL;
	unsigned long size;
	int len;

	while ((len = getline(&buff, &size, f)) > 0) {

		if (m->line_length == 0) {
			m->line_length = len-1;
		}

		maze_add_row(m, buff);

		free(buff);
		buff = NULL;
	}

	fclose(f);
	return m;
}

void maze_free(maze * m) {
	for (int j = 0; j < m->n_lines; j++) {
		free(m->maze[j]);
	}	
	free(m->maze);
	free(m);
	m = NULL;
}

//systematically adding a random obstacle is easy
// you are guranteed to be in a loop if you are at a location where you have 1: been before, 2: are facing the same direction

int run_maze(maze * m) {
	maze_dict * md = maze_dict_create(m->line_length, m->n_lines);

	int c = maze_move_loop(m, md, false, -1);

	maze_free(m);
	maze_dict_free(md);
	return c;
}

void part2() {
	int c = 0; 
	maze * m = maze_create();
	int J = m->n_lines;
	int I = m->line_length;
	maze_free(m);

	for (int j = 0; j < J; j++) {
		for (int i = 0; i < I; i ++) {	
	//for (int j = 6; j < J; j++) {
	//	for (int i = 3; i < I; i ++) {	
			m = maze_create();
			if (m->maze[j][i] == EMPTY && !(i == m->x_loc && j == m->y_loc)) {
				//printf("Running\n");
				m->maze[j][i] = WALL;
				int r =  run_maze(m);
				if (r)
					printf("%d, %d\n", i, j);
					
				c += r;
			}
			//printf("one down. %d.\n", c);
		}	
	}

	printf("Number of loops: %d\n", c);
}

int main () {
	part2();
}
