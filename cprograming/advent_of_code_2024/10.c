#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef struct map {
	char ** data;
	int x;
	int max_y;
	int cur_y;
} map;

map * map_create() {
	map * m = calloc(1, sizeof(map));
	m->x = 0;
	m->cur_y = 0;
	m->max_y = 100;
	m->data = malloc(sizeof(char *) * m->max_y);
	for (int i = 0; i < m->cur_y; i++) {
		free(m->data[i]);
	}
	return m;
}

char map_get(map * m, int x, int y) {
	if (x < 0 || x >= m->x) {
		return '\0';
	} else if (y < 0 || y >= m->cur_y) {
		return '\0';
	} else {
		return m->data[y][x];
	}
}

void map_set(map * m, int x, int y, char val) {
	m->data[y][x] = val;
}

void map_delete(map * m) {
	for (int i = 0; i < m->cur_y; i++) {
		free(m->data[i]);
	}
	free(m->data);
	free(m);
}

void map_print(map * m) {
	for (int i = 0; i < m->cur_y; i++) {
		for (int j = 0; j < m->x; j++) {
			if (m->data[i][j] > '9') {
				printf("x");
			} else {
				printf("%c",m->data[i][j]);
			}
		}
		printf("\n");
	}
	printf("\n");
}

void map_insert(map * m, char * b) {
	m->data[m->cur_y] = b;
	m->cur_y ++;
	if (m->cur_y == m->max_y) {
		m->max_y *= 2;
		m->data = realloc(m->data, sizeof(char*) * m->max_y);
		for (int i = m->cur_y; i < m->max_y; m++) {
			m->data[i] = NULL;;
		}
	}
}

void get_trail(map * m, int x, int y, int * count) {
	char up, down, left, right, self;
	up = map_get(m, x, y-1);
	down = map_get(m, x, y+1);
	left = map_get(m, x-1, y);
	right = map_get(m, x+1, y);
	self = map_get(m, x, y);

	if (self == '9') {
		(*count) ++;		
		map_set(m, x, y, self + 10);
	} else if (self <= '9') {
		map_set(m, x, y, self + 10);

		if (up - self == 1) {
			get_trail(m, x, y-1, count);
		}
		
		if (down - self == 1) {
			get_trail(m, x, y+1, count);
		}
		
		if (left - self == 1) {
			get_trail(m, x-1, y, count);
		}
		
		if (right - self == 1) {
			get_trail(m, x+1, y, count);
		}
	}

}

map * map_from_file() {
	FILE * f = fopen("./input10.txt", "r");
	char * buff = NULL;
	unsigned long size;
	int len;
	map * m = map_create();

	while ((len = getline(&buff, &size, f)) > 0) {
		if (m->x == 0) {
			m->x = len - 1;
		}
		map_insert(m, buff);
		buff = NULL;
	}
	
	map_print(m);
	fclose(f);
	return m;
}

map * map_cpy(map * m) {
	map * new = calloc(1, sizeof(map));
	new->x = m->x;
	new->cur_y = m->cur_y;
	new->max_y = m->max_y;
	new->data = malloc(sizeof(char *) * m->max_y);
	for (int i = 0; i < new->cur_y; i++) {
		new->data[i] = malloc(sizeof(char)*new->x);
		for (int j = 0; j < new->x; j++) {
			new->data[i][j] = m->data[i][j];
		}	
	}
	return new;
}

void get_trail_type2(map * m, int x, int y, int * count) {
	char up, down, left, right, self;
	up = map_get(m, x, y-1);
	down = map_get(m, x, y+1);
	left = map_get(m, x-1, y);
	right = map_get(m, x+1, y);
	self = map_get(m, x, y);

	if (self == '9') {
		(*count)++;
		map_set(m, x, y, self + 10);
	} else if (self <= '9') {
		map_set(m, x, y, self + 10);

		if (up - self == 1) {
			get_trail(map_cpy(m), x, y-1, count);
		}
		
		if (down - self == 1) {
			get_trail(map_cpy(m), x, y+1, count);
		}
		
		if (left - self == 1) {
			get_trail(map_cpy(m), x-1, y, count);
		}
		
		if (right - self == 1) {
			get_trail(map_cpy(m), x+1, y, count);
		}
	} 

	map_delete(m);
}

int evaluate_trailheads(map * m) {
	int trails = 0, prev_trails = 0;
	for (int j = 0; j < m->cur_y; j++) {
		for (int i = 0; i < m->x; i++) {
			if (map_get(m, i,j) == '0') {

				map * new = map_cpy(m);
				get_trail_type2(new, i, j, &trails);
				printf("%d peaks found\n\n", trails - prev_trails);
				prev_trails = trails;
			}
		}
	}
	return trails;
}

int main () {
	map * m = map_from_file();
	printf("\n");
	int count = evaluate_trailheads(m);
	printf("%d trailheads found.\n",count);
}

