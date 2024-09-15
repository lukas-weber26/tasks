#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

typedef struct snake {
	int x;
	int y;
} snake;

void loose(int X, int Y) {
	wclear(stdscr);
	mvprintw(Y/2,X/2 - 4, "YOU LOST.");
	wrefresh(stdscr);
	usleep(3000000);
}

void draw_snake(snake * snake_array, int snake_head, int size, int max_size) {
	//mvprintw(snake_array[snake_head].y, snake_array[snake_head].x,"X");
	for (int i =0; i < size; i++) {
		int pos = (snake_head+i)%max_size;
		mvprintw(snake_array[pos].y, snake_array[pos].x,"X");
	}
}

int snake_intersection (snake * snake_array, int snake_head, int size, int max_size) {
	for (int i =1; i < size; i++) {
		int pos = (snake_head+i)%max_size;
		if (snake_array[pos].y == snake_array[snake_head].y && snake_array[pos].x == snake_array[snake_head].x) {
			return 1;
		}
	}
	return 0;
}

int main() {

	FILE * fd = fopen("array_times", "w");
	
	initscr();
	keypad(stdscr, TRUE);
	int MAXX = getmaxx(stdscr);
	int MAXY = getmaxy(stdscr);
	int MAXXMAXY = MAXX*MAXY;
	raw();
	noecho();
	refresh();
	nodelay(stdscr, TRUE);

	int c;
	int size = 1;

	int pos_x = MAXX/2;
	int pos_y = MAXY/2;
	
	snake * snake_array = malloc(sizeof(snake)*MAXX*MAXY);
	int snake_head = 0;

	snake_array[snake_head].x = pos_x;
	snake_array[snake_head].y = pos_y;

	int x_dir = 1;
	int y_dir = 0;
	
	int food_pos_x = rand()%MAXX;
	int food_pos_y = rand()%MAXY;

	while(true) {
		wclear(stdscr);
		refresh();

		c=getch();
		if (c == KEY_UP) {
			x_dir = 0; 
			y_dir = -1;
		} else if (c == KEY_DOWN) {
			x_dir = 0;
			y_dir = 1;
		} else if (c == KEY_LEFT) {
			x_dir = -1; 
			y_dir = 0;
		} else if (c == KEY_RIGHT) {
			x_dir = 1; 
			y_dir = 0; 
		} else if (c == 'X') {
			endwin(); 
			exit(0); 
		}
		
		pos_x += x_dir;
		pos_y += y_dir;

		if ((pos_x < 0) || (pos_x > MAXX) || (pos_y > MAXY) || (pos_y) < 0) {
			loose(MAXX, MAXY);
			break;
		}
		
		if ((pos_x == food_pos_x) && (pos_y == food_pos_y)) {
			food_pos_x = rand()%MAXX;
			food_pos_y = rand()%MAXY;
			size ++; }
		

		clock_t start, end; 
		double cpu_time_used;

		start = clock();

		snake_head -= 1;
		if (snake_head < 0) { 
			snake_head = (MAXXMAXY)-1;
		}
		
		snake_array[snake_head].x = pos_x;
		snake_array[snake_head].y = pos_y;

		draw_snake(snake_array, snake_head, size, MAXXMAXY);
		if (snake_intersection(snake_array, snake_head, size, MAXXMAXY)) {
			loose(MAXX, MAXY);	
			break;
		}

		end = clock();
		cpu_time_used = ((double) (end-start))/CLOCKS_PER_SEC;
	
		char buff [50];
		sprintf(buff, "%lf\n",cpu_time_used);
		fwrite(buff, strlen(buff), sizeof(char), fd);
	
		
		mvprintw(food_pos_y,food_pos_x,"0");  
		mvprintw(MAXY-1,1, "Player 1 current size: %d. Press X to exit.", size);

		wrefresh(stdscr);	
		if (x_dir != 0) {
			usleep(50000);
		} else {
			usleep(100000);
		}
	}

	fclose(fd);
	endwin();
}

