#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

typedef enum {UP, RIGHT, DOWN, LEFT} instruction_type;

typedef struct instruction_list {
	int cur;
	int max;
	instruction_type * instructions; 
} instruction_list; 

instruction_list * instruction_list_create() {
	instruction_list * new = calloc(1, sizeof(instruction_list));
	new->cur = 0;
	new->max = 64;
	new->instructions = calloc(new->max, sizeof(instruction_type));
	return new;
}

void instruction_list_print(instruction_list * list) {
	for (int i = 0; i < list->cur; i++) {
		switch (list->instructions[i]) {
			case UP: printf("UP\n"); break;
			case RIGHT: printf("RIGHT\n"); break;
			case DOWN: printf("DOWN\n"); break;
			case LEFT: printf("LEFT\n"); break;
			default: printf("Bad type.\n"); exit(0);
		}
	}
}

void instruction_list_push(instruction_list * list, instruction_type val) {
	list->instructions[list->cur++] = val;
	if (list->cur == list->max) {
		list->max *= 2;
		list->instructions = realloc(list->instructions, sizeof(instruction_type)*list->max);
	}
} 

instruction_list * instruction_list_from_file() {
	instruction_list * l = instruction_list_create();

	FILE * f = fopen("./input15dir.txt", "r");
	int input;

	while ((input = getc(f)) != EOF) {
		switch (input) {
			case '<': instruction_list_push(l, LEFT); break;	
			case '>': instruction_list_push(l, RIGHT); break;	
			case '^': instruction_list_push(l, UP); break;	
			case 'v': instruction_list_push(l, DOWN); break;	
			case ' ':
			case '\t':
			case '\n':
				break;
			default: printf("Bad instruction type\n"); exit(0);
		}	
	}

	return l;
}

typedef struct maze {
	char ** data;
	int len_x;
	int cur_y;
	int max_y;
	int robot_x;
	int robot_y;
} maze;

char maze_get(maze * m, int x, int y) {
	if (x < 0 || y < 0) {
		return '\0';
	}
	if (x >= m->len_x || y >= m->cur_y) {
		return '\0';
	}
	return m->data[y][x];
}

void maze_set(maze * m, int x, int y, char val) {
	if (x < 0 || y < 0) {
		return;
	}
	if (x >= m->len_x || y >= m->cur_y) {
		return;
	}
	m->data[y][x] = val;
}

void maze_find_robot(maze * m) {
	for (int i = 0; i < m->cur_y; i++) {
		for (int j = 0; j < m->len_x; j++) {
			if (m->data[i][j] == '@') {
				m->robot_x = j;
				m->robot_y = i;
				return;
			}
		}
	}
}

maze * maze_create() {
	maze * new = calloc(1, sizeof(maze));

	new->len_x = 0;
	new->cur_y = 0;
	new->max_y = 64;
	new->data = calloc(new->max_y, sizeof(char *));

	return new;
}

void maze_push(maze * m, char * b) {
	m->data[m->cur_y++] = b;
	if (m->cur_y == m->max_y) {
		m->max_y *= 2;
		m->data = realloc(m->data, m->max_y*sizeof(char *));
	}
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

void maze_fatten(maze * m) {
	for (int i = 0; i < m->cur_y; i++) {
		char * newline = malloc(m->len_x*sizeof(char)*2);

		int c = 0;
		for (int j = 0; j < m->len_x; j++) {
			if (m->data[i][j] == '#') {
				newline[c++] = '#';
				newline[c++] = '#';
			} else if (m->data[i][j] == 'O') {
				newline[c++] = '[';
				newline[c++] = ']';
			} else if (m->data[i][j] == '.') {
				newline[c++] = '.';
				newline[c++] = '.';
			} else if (m->data[i][j] == '@') {
				newline[c++] = '@';
				newline[c++] = '.';
			}
		}

		free(m->data[i]);
		m->len_x = c;
		m->data[i] = newline;
	}
}

maze * maze_from_file() {
	FILE * f = fopen("./input15maze.txt", "r");
	maze * m = maze_create();
	char * buff = NULL;
	unsigned long size;
	int len;

	while ((len = getline(&buff, &size, f)) != -1) {
		if (m->len_x == 0) {
			m->len_x = len -1;
		}
		maze_push(m, buff);
		buff = NULL;
	}


	maze_fatten(m);
	maze_find_robot(m);

	fclose(f);
	return m;
}

int next_y(instruction_type dir, int y) {
	if (dir == UP) {
		return y - 1;
	} else if (dir == DOWN) {
		return y + 1;
	} else {
		return y;	
	}
}

int next_x(instruction_type dir, int x) {
	if (dir == RIGHT) {
		return x+1;
	} else if (dir == LEFT) {
		return x-1;
	} else {
		return x;	
	}
}

bool move_if_valid(maze * m, int cur_x, int cur_y, instruction_type dir) {
	char c = maze_get(m, cur_x, cur_y);

	if (c == '.') {
		return true;
	} else if (c == '#'){
		return false;	
	} else {
		int xn = next_x(dir, cur_x), yn = next_y(dir, cur_y);
		bool can_move = move_if_valid(m, xn, yn, dir);
		if (can_move) {
			
			maze_set(m, xn, yn, c);
			if (c == '@') {
				m->robot_x = xn;
				m->robot_y = yn;
				maze_set(m, cur_x, cur_y, '.');
			}
		} 

		return can_move;
	};

	int new_x = next_x(dir, cur_x), new_y = next_y(dir, cur_y);
};

void maze_move(instruction_list * l, maze * m) {
	for (int i = 0; i < l->cur; i++) {
		instruction_type dir = l->instructions[i];
		int cur_x = m->robot_x, cur_y = m->robot_y;
		move_if_valid(m, cur_x, cur_y, dir);	
		maze_print(m);
	}
}

bool is_box(char c) {
	if (c == '[' || c == ']') {
		return true;
	}
	return false;
}

bool recursive_vibe_check(maze * m, int cur_x, int cur_y, instruction_type dir) {
	char c = maze_get(m, cur_x, cur_y);
	int nx = next_x(dir, cur_x), ny = next_y(dir, cur_y);

	char next = maze_get(m, nx, ny);
	if (!is_box(next)){
		if (next == '#') {
			return false;
		} else if (next == '.') {
			return true;
		}
	}

	if (maze_get(m, nx, ny) == '[') {
		bool a = recursive_vibe_check(m, nx, ny, dir);
		bool b = recursive_vibe_check(m, nx + 1, ny, dir);
		return a && b;
	}
	
	if (maze_get(m, nx, ny) == ']') {
		bool a = recursive_vibe_check(m, nx - 1, ny, dir);
		bool b = recursive_vibe_check(m, nx, ny, dir);
		return a && b;
	}

	printf("Error\n");
	exit(0);
};

bool move_if_valid_fat(maze * m, int cur_x, int cur_y, instruction_type dir) {
	char c = maze_get(m, cur_x, cur_y);

	if (c == '.') {
		return true;
	} else if (c == '#'){
		return false;	
	} else if (c == '@') {		
		int nx = next_x(dir, cur_x), ny = next_y(dir, cur_y);
		bool move = move_if_valid_fat(m, nx, ny, dir);
		if (move) {
			m->robot_x = nx;
			m->robot_y = ny;
			maze_set(m, nx, ny, '@'); 
			maze_set(m, cur_x, cur_y, '.');
		}
		return move;
	} else if (c == '[' ) {
		int nx = next_x(dir, cur_x), ny = next_y(dir, cur_y);
		bool move_l = move_if_valid_fat(m, nx, ny, dir);
		bool move_r = move_if_valid_fat(m, nx + 1, ny, dir);
		if (move_l && move_r) {
			maze_set(m, nx, ny, '['); 
			maze_set(m, nx+1, ny, ']'); 
			maze_set(m, cur_x, cur_y, '.');
			maze_set(m, cur_x + 1, cur_y, '.');
		}	
		return move_l && move_r;
	} else if (c == ']') {
		int nx = next_x(dir, cur_x), ny = next_y(dir, cur_y);
		bool move_l = move_if_valid_fat(m, nx - 1, ny, dir);
		bool move_r = move_if_valid_fat(m, nx, ny, dir);
		if (move_l && move_r) {
			maze_set(m, nx-1, ny, '['); 
			maze_set(m, nx, ny, ']'); 
			maze_set(m, cur_x - 1, cur_y, '.');
			maze_set(m, cur_x, cur_y, '.');
		}	
		return move_l && move_r;
	} else {
		printf("Error\n");
		exit(0);
	}

};

void maze_move_fat(instruction_list * l, maze * m) {
	for (int i = 0; i < l->cur; i++) {
		instruction_type dir = l->instructions[i];
		int cur_x = m->robot_x, cur_y = m->robot_y; 
		if (dir == LEFT || dir == RIGHT) {
			move_if_valid(m, cur_x, cur_y, dir);	
		} else {
			if (recursive_vibe_check(m, cur_x, cur_y, dir)) {
				move_if_valid_fat(m, cur_x, cur_y, dir);	
			}
		}
		//maze_print(m);
	}
}

int maze_gps_fat(maze * m) {
	int sum = 0;
	maze_print(m);
	for (int i = 0; i < m->cur_y; i++) {
		for (int j = 0; j < m->len_x; j++) {
			if (maze_get(m, j, i) == '[') {
				int s = (100 * i + j);		
				//printf("val: %d\n", s);
				sum += s;
			}
		}
	}
	return sum;
} 

int maze_gps(maze * m) {
	int sum = 0;
	maze_print(m);
	for (int i = 0; i < m->cur_y; i++) {
		for (int j = 0; j < m->len_x; j++) {
			if (maze_get(m, j, i) == 'O') {
				int s = (100 * i + j);		
				//printf("val: %d\n", s);
				sum += s;
			}
		}
	}
	return sum;
} 

int main() {
	instruction_list * l = instruction_list_from_file();
	//instruction_list_print(l);
	maze * m = maze_from_file();
	//maze_print(m);
	maze_move_fat(l, m);
	printf("Final GPS: %d\n", maze_gps_fat(m));
}











