#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef struct hash_table {
	int * values;
	int *indexes;
	int size;
} hash_table;

int hash (int target, int size) {
	return ((target* 37)>>2)%size;
}

hash_table * hash_table_create(int size) {
	hash_table * t = calloc(1, sizeof(hash_table));
	t->size = size;
	t->values = calloc(t->size, sizeof(int));
	t->indexes= calloc(t->size, sizeof(int));
	for (int i = 0; i < size; i++) {
		t->indexes[i] = -1;
	}
	return t;
}

void hash_table_print(hash_table * t) {
	for (int i = 0; i < t->size; i++) {
		if (t->indexes[i] != -1) {
			printf("%d:%d\n", t->indexes[i], t->values[i]);
		}
	}	
	printf("\n");
}

void hash_table_increment(hash_table * t, int target, int value) {
	int index = hash(target, t->size); 
	int index_cpy = index;

	while (1) {
		if( t->indexes[index] == target) {
			t->values[index] += value;
			return;
		} else if (t->indexes[index] == -1) {
			t->indexes[index] = target;
			t->values[index] += value;
			return;
		} 

		index = (index + 1)%t->size;
		if (index == index_cpy) {
			printf("Table too small\n");
			exit(0);
		}

	}
}

int hash_table_get(hash_table * t, int target) {
	int index = hash(target, t->size); 
	int index_cpy = index;

	while (1) {
		if( t->indexes[index] == target) {
			return t->values[index];
		} else if (t->indexes[index] == -1) {
			return -1;
		} 

		index = (index + 1)%t->size;
		if (index == index_cpy) {
			return -1;
		}

	}
}
typedef struct table {
	int current_val;
	int x;
	int y;
	int * data;
} table;

table * table_create(int x, int y) {
	table * t = calloc(1, sizeof(table));
	t->x = x;
	t->y = y; 
	t->current_val = 0;
	t->data = calloc(x*y, sizeof(int));
	return t;
}

int table_get(table * t, int x, int y) {
	if (x < 0 || y < 0) {
		return -1;
	}
	if (x >= t->x || y >= t->y) {
		return -1;
	}
	return t->data[x + y*t->x];	
}

void table_set(table * t, int x, int y, int val) {
	if (x < 0 || y < 0) {
		return;
	}
	if (x >= t->x || y >= t->y) {
		return;
	}
	t->data[x + y*t->x] = val;	
}

void table_print(table * t) {
	for (int i = 0; i < t->y; i++) {
		for (int j = 0; j < t->x; j++) {
			printf("%d ", table_get(t, j, i));
		}
		printf("\n");
	}
	printf("\n");
}

typedef struct grid {
	char ** data; 
	int x_len;
	int y_cur;
	int y_max;
} grid;

grid * grid_create() {
	grid * g = calloc(1, sizeof(grid));
	g->x_len = 0;
	g->y_cur = 0;
	g->y_max = 128;
	g->data = calloc(g->y_max, sizeof(char *));
	return g;
}

void grid_push(grid * g, char * buff) {
	g->data[g->y_cur] = buff;
	g->y_cur ++;
	if (g->y_cur == g->y_max) {
		g->y_max *= 2;
		g->data = realloc(g->data, g->y_max * sizeof(char *));
	}
}

void grid_set(grid * g, int x, int y, char val) {
	if (x < 0 || y< 0) {
		return;
	}
	if (x >= g->x_len || y >= g->y_cur) {
		return;
	}
	g->data[y][x] = val;
}

char grid_get(grid * g, int x, int y) {
	if (x < 0 || y< 0) {
		return '\0';
	}
	if (x >= g->x_len || y >= g->y_cur) {
		return '\0';
	}
	return g->data[y][x];
}

void grid_print(grid * g) {
	for (int j = 0; j < g->y_cur; j++) {
		for (int i = 0; i < g->x_len; i++) {
			printf("%c",grid_get(g, i, j));
		}
		printf("\n");
	}
	printf("\n");
}

grid * load_grid_from_file() {
	FILE * input = fopen("./input12.txt", "r");

	char * buff = NULL;
	unsigned long size;
	int len;
	grid * g = grid_create();

	while ((len = getline(&buff, &size, input)) > 0) {
		if (g->x_len == 0) {
			g->x_len = len -1;
		}
		grid_push(g, buff);
		buff = NULL;
	}		

	fclose(input);	
	return g;
}

void table_crawl(grid * g, table * t, int x, int y)  {
	if (table_get(t, x, y) != 0) {
		return;
	}

	table_set(t, x, y, t->current_val);
	char self_type = grid_get(g, x, y);
	char left_type = grid_get(g, x-1, y);
	char right_type = grid_get(g, x+1, y);
	char top_type = grid_get(g, x, y-1);
	char bottom_type = grid_get(g, x, y+1);

	if (self_type == left_type) {
		table_crawl(g, t, x-1, y);
	}
	
	if (self_type == right_type) {
		table_crawl(g, t, x+1, y);
	}
	
	if (self_type == top_type) {
		table_crawl(g, t, x, y-1);
	}
	
	if (self_type == bottom_type) {
		table_crawl(g, t, x, y+1);
	}

};

table * detect_zones(grid * g) {
	table * t = table_create(g->x_len, g->y_cur);
	for (int i = 0; i < g->y_cur; i++) {
		for (int j = 0; j < g->x_len; j++) {
		
			if (table_get(t, j, i) == 0) {
				t->current_val ++;
				table_crawl(g,t,j,i);
			}

		}	
	}	
	return t;
}

hash_table * table_get_areas(table * t) {
	printf("current size: %d\n", t->current_val);
	hash_table * areas = hash_table_create(t->current_val);

	for (int i = 0; i < t->y; i++) {
		for (int j = 0; j < t->x; j++) {
			hash_table_increment(areas, table_get(t, j, i), 1);	
		}	
	}	
	return areas;	
}

hash_table * table_get_perimeters(table * t) {
	hash_table * perimeters = hash_table_create(t->current_val);

	for (int i = 0; i < t->y; i++) {
		for (int j = 0; j < t->x; j++) {
			int perimeter = 4;
			
			int self_type = table_get(t, j, i);
			int left_type = table_get(t, j-1, i);
			int right_type = table_get(t, j+1, i);
			int top_type = table_get(t, j, i-1);
			int bottom_type = table_get(t, j, i+1);

			if (self_type == left_type) {
				perimeter --;
			}

			if (self_type == right_type) {
				perimeter --;
			}
			
			if (self_type == top_type) {
				perimeter --;
			}
			
			if (self_type == bottom_type) {
				perimeter --;
			}

			hash_table_increment(perimeters, table_get(t, j, i), perimeter);	
		}	
	}	
	return perimeters;	
}

hash_table * table_get_perimeters_discount(table * t) {
	hash_table * perimeters = hash_table_create(t->current_val);

	for (int i = 0; i < t->y; i++) {
		for (int j = 0; j < t->x; j++) {
			int perimeter = 0;
			
			int self_type = table_get(t, j, i);

			int left_type = table_get(t, j-1, i);
			int right_type = table_get(t, j+1, i);
			int top_type = table_get(t, j, i-1);
			int bottom_type = table_get(t, j, i+1);

			int top_left_type = table_get(t, j-1, i-1);
			int top_right_type = table_get(t, j+1, i-1);
			int bottom_left_type = table_get(t, j-1, i+1);
			int bottom_right_type = table_get(t, j+1, i+1);

			if (self_type != top_type && self_type != right_type) {
				perimeter ++;
			}
			
			if (self_type != bottom_type && self_type != right_type) {
				perimeter ++;
			}
			
			if (self_type != top_type && self_type != left_type) {
				perimeter ++;
			}
			
			if (self_type != bottom_type && self_type != left_type) {
				perimeter ++;
			}

			if (self_type == top_type && self_type == right_type && self_type != top_right_type) {
				perimeter ++;	
			}
			
			if (self_type == bottom_type && self_type == right_type && self_type != bottom_right_type) {
				perimeter ++;	
			}
			
			if (self_type == top_type && self_type == left_type && self_type != top_left_type) {
				perimeter ++;	
			}
			
			if (self_type == bottom_type && self_type == left_type && self_type != bottom_left_type) {
				perimeter ++;	
			}


			hash_table_increment(perimeters, table_get(t, j, i), perimeter);	
		}	
	}	
	return perimeters;	
}

int price (hash_table * area, hash_table* perimeter) {
	int sum = 0;
	for (int i = 0; i < area->size; i++) {
		if (area->indexes[i] != -1) {
			sum += area->values[i]*perimeter->values[i];
		}
	}	
	return sum;
}

int main () {
	grid * g = load_grid_from_file();
	grid_print(g);
	table * t = detect_zones(g);
	table_print(t);
	hash_table * areas = table_get_areas(t);
	hash_table_print(areas);
	hash_table * perimeters = table_get_perimeters_discount(t);
	hash_table_print(perimeters);
	printf("Final price: %d\n", price(areas, perimeters));
}
