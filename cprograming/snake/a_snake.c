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
	struct snake * next;
} snake;

snake * get_snake(int X, int Y) {
	snake * s = malloc(sizeof(snake));
	s->x = X; 
	s->y = Y;
	s->next = NULL;
	return s;
}

snake * snake_insert(int X, int Y, snake * head) {
	snake * s = malloc(sizeof(snake));
	s->x = X; 
	s->y = Y;
	s->next = head;
	return s;
}

void draw_snake(snake * head, int size) {
	int count = 0;
	while (true) {
		mvprintw(head->y, head->x, "X"); 
		count ++;
		if (count >= size) {
			break;	
		} else {
			head = head -> next;
		}
	}

	snake * temp = head -> next;
	head -> next = NULL;

	while (temp) {
		snake * save = temp;
		temp = temp -> next;
		save ->next = NULL;
		free(save);
	}

}

int check_snake_intersection(snake * head) {
	snake * body = head -> next;
	while (body) {
		if (body ->x == head ->x && body -> y == head ->y) {
			return 1;
		}
		body = body -> next;
	}

	return 0;
}

void loose(int X, int Y) {
	wclear(stdscr);
	mvprintw(Y/2,X/2 - 4, "YOU LOST.");
	wrefresh(stdscr);
	usleep(3000000);
}

int main() {

	FILE * fd = fopen("linked_list_times", "w");
	
	initscr();
	keypad(stdscr, TRUE);
	int MAXX = getmaxx(stdscr);
	int MAXY = getmaxy(stdscr);
	raw();
	noecho();
	refresh();
	nodelay(stdscr, TRUE);

	int c;
	int size = 1;

	int pos_x = MAXX/2;
	int pos_y = MAXY/2;
	

	snake * head = get_snake(pos_x, pos_y);

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

		head = snake_insert(pos_x, pos_y,head);
		draw_snake(head, size); 
		
		if (check_snake_intersection(head)) {
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

