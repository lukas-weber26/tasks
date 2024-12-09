#include <stdlib.h>
#include <stdio.h>
#include <alloca.h>
#include <assert.h>
#include <stdbool.h>

typedef struct location {
	int x;
	int y;
} location;

void location_print(location l) {
	printf("(%d, %d) ", l.x, l.y);
} 

typedef struct location_list {
	location * locs;
	int cur;
	int max;
} location_list;

location_list * location_list_create(int n) {
	location_list * l = calloc(1, sizeof(location_list));
	l->max = n;
	l->cur = 0;
	l->locs = calloc(l->max, sizeof(location));
	return l;
}

void location_list_free(location_list * l) {
	free(l->locs);
	free(l);
	l = NULL;
} 

void location_list_print(location_list * l) {
	for (int i = 0; i < l->cur; i ++) {
		location_print(l->locs[i]);
	}
	printf("\n");
} 

void location_list_push(location_list * list, location loc) {
	list->locs[list->cur ++] = loc;
	if (list->cur >= list->max) {
		list->max *= 2;
		list->locs = realloc(list->locs, list->max*sizeof(location));
	}
} 

typedef struct antena_table {
	int * type;
	location_list ** lists;	
	int size;
} antena_table;

char hash(char type, antena_table * t) {
	//return ((37*type)>>2) % t->size;	
	return type % t->size;
}

antena_table * antena_table_create(int size) {
	antena_table * t = calloc(1, sizeof(antena_table));
	t->type = calloc(size, sizeof(int));

	for (int i = 0; i < size; i ++) {
		t->type[i] = -1;
	}

	t->lists = calloc(size, sizeof(location_list *));
	t->size = size;
	return t;
}

void antena_table_free(antena_table * t) {
	for (int i = 0; i < t->size; i++) {
		if (t->type[i] != -1) {
			location_list_free(t->lists[i]);
		}
	}
	free(t->lists);
	free(t->type);
	free(t);
	t = NULL;
}

void antena_table_print(antena_table * t) {
	for (int i = 0; i < t->size; i++) {
		if (t->type[i] != -1) {
			//if (t->type[i] < 256) {
				printf("%c: ", t->type[i]);
			//} else {
			//	printf("%d: ", t->type[i]);
			//}
			location_list_print(t->lists[i]);
		}
	}
} 

void antenta_table_insert(antena_table * t, char type, location l) {
	int index = hash(type, t);
	int index_cpy = index;

	while (t->type[index] != type && t->type[index] != -1) {
		index ++;
		if (index == index_cpy) {
			printf("Insert error.\n");
			exit(0);
		}
	}

	if (t->type[index] == -1) {
		t->type[index] = type;
		t->lists[index] = location_list_create(10);
	} 

	location_list_push(t->lists[index], l);
	
}

location_list * antenta_table_get(antena_table * t, char type, location l) {
	int index = hash(type, t);
	int index_cpy = index;

	while (t->type[index] != type) {
		index ++;
		if (index == index_cpy || t->type[index] == -1) {
			printf("Warning, no list found\n");
			return NULL;
		}
	}

	return t->lists[index];
}

typedef struct antinode_table {
	location size;
	bool * data;
	int count;
} antinode_table;

antinode_table * antinode_table_create(location size) {
	antinode_table * t = calloc(1, sizeof(antinode_table));
	t->size = size;
	t->data = calloc(size.x*size.y, sizeof(bool));
	t->count = 0;
	return t;
}

void antinode_table_push(antinode_table * t, location target_loc) {
	if (target_loc.x < 0 || target_loc.y < 0) {
		return;
	}
	if (target_loc.x >= t->size.x || target_loc.y >= t->size.y) {
		return;
	}

	bool *used = &(t->data[t->size.x*target_loc.y + target_loc.x]);
	if (!(*used)) {
		*used = true;	
		t->count ++;
	}
}

void antinode_table_free(antinode_table * t) {
	free(t->data);
	free(t);
	t = NULL;
}

void antinode_table_print(antinode_table * t) {
	for (int i = 0; i < t->size.y; i ++) {
		for (int j = 0; j < t->size.x; j ++) {
			if (t->data[j+i*t->size.x]) {
				printf("#");
			} else {
				printf(".");
			}
		}
		printf("\n");
	}
	printf("\n");
}

void load_data(antena_table ** antenas, antinode_table ** antinodes) {
	FILE * f = fopen("./input8.txt", "r");
	unsigned long size;
	int len;
	char * buff = NULL;

	*antenas = antena_table_create(256);
	int y = 0;
	int x = 0;

	while ((len = getline(&buff, &size, f)) > 0) {
		x = 0;
		for (int i = 0; i < len -1; i ++) {
			if (buff[i] != '.') {
				location loc = {x,y};
				antenta_table_insert(*antenas, buff[i], loc);	
			}
			x ++;
		}
		y ++;
		free(buff);
		buff = NULL;
	}

	location table_size = {x,y};
	*antinodes = antinode_table_create(table_size);


	fclose(f);
}

void location_list_calcualte_antinodes(location_list * locations, antinode_table * antinodes) {
	for (int i = 0; i < locations->cur - 1; i ++) {
		for (int j = i + 1; j < locations->cur; j ++) {
			//should now have all combinations of antinodes	
			//location_print(locations->locs[i]);
			//location_print(locations->locs[j]);
			//printf("\n");

			location a = locations->locs[i];
			location b = locations->locs[j];

			location c = {a.x - 2*(a.x-b.x),a.y - 2*(a.y-b.y)};	
			location d = {a.x + (a.x-b.x), a.y + (a.y-b.y)};	

			antinode_table_push(antinodes, c);
			antinode_table_push(antinodes, d);
		}
	}
}

void antena_table_resolve_antinodes(antena_table * antenas, antinode_table * antinodes) {
	for (int i = 0; i < antenas->size; i++) {
		if (antenas->type[i] != -1) {
			//printf("Resoving antinodes:\n");
			location_list_calcualte_antinodes(antenas->lists[i], antinodes);
			//printf("\n");
		}
	}
} 

int main() {
	antena_table * antenas;
	antinode_table * antinodes;

	load_data(&antenas, &antinodes);
	
	antena_table_print(antenas);
	antinode_table_print(antinodes);

	antena_table_resolve_antinodes(antenas, antinodes);
	
	antinode_table_print(antinodes);
	printf("There are %d unique antinodes\n", antinodes->count);

}
