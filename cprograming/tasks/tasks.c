#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_tasks(char ** names, char * types, int length) {
	FILE * fp = fopen("kanban_file.txt", "w");

	for (int i = 0; i < length; i ++) {
		fprintf(fp, "%c:%s", types[i], names[i]);
	}

	fclose(fp);
}

//maybe make this a bit more sophisticated as time goes on
void print_tasks(char ** names, char * types, int length) {

	printf("Active tasks:\n");
	for (int i = 0; i < length; i++) {
		if (types[i] == 'a') {
			printf("%d: %s",i,names[i]);	
		}
	}

	printf("\nInactive tasks:\n");
	for (int i = 0; i < length; i++) {
		if (types[i] == 'w') {
			printf("%d: %s",i,names[i]);	
		}
	}
	
	printf("\nCompleted tasks:\n");
	for (int i = 0; i < length; i++) {
		if (types[i] == 'c') {
			printf("%d: %s",i,names[i]);	
		}
	}
}

void add_task(char ** names, char * types, int *length, char * task_name, char task_type) {
	if (!(task_type == 'a' || task_type == 'w' || task_type == 'c')) {
		printf("Invalid task type. Please use a|w|c\n");
		return;	
	} 

	//never have to realloc anything because realloc accounts for one item allways being added in reading step.
	types[*length] = task_type;

	int size = strlen(task_name);

	names[*length] = malloc(size + 2);
	strcpy(names[*length], task_name);
	names[*length][size] = '\n';
	names[*length][size+1] = '\0';

	*length+=1;

	write_tasks(names, types, *length);

}

void move_task(char ** names, char * types, int length, int task_number, char task_type) {
	if ((task_type == 'a' || task_type == 'w' || task_type == 'c') && (task_number >= 0 && task_number < length)) {
		types[task_number] = task_type;
	} else {
		printf("Invalid task number of destination. No changes have been made.\n");
	}

	write_tasks(names, types, length);
}

void help() {
	printf("Usage: kanban <comand> <args>\n");
	printf("Possible commnds:\n");
	printf("add <task name> <a|w|c>\n");
	printf("move <task number> <a|w|c>\n");
	printf("Note that a = active, w = waiting, c = complete\n");
	printf("Please be carefull about using space in task names!\n");
}

int main(int argc, char * argv[]) {
	
	FILE * fp = fopen("kanban_file.txt", "r");
	unsigned long line_size = 256;
	unsigned long num_read;
	char *line_buffer;
	
	int number_of_lines= 256;
	int number_of_used_lines = 0;
	char **names = malloc(sizeof(char *) * number_of_lines);
	char *types = malloc(sizeof(char) * number_of_lines);

	while ((num_read = getline(&line_buffer, &line_size, fp)) != -1) {

	
		char *new_line = malloc(sizeof(char) * num_read-1);
		memcpy(new_line,line_buffer+2,num_read-1);
		types[number_of_used_lines] = line_buffer[0];
		names[number_of_used_lines] = new_line;

		number_of_used_lines++;

		if (number_of_used_lines == number_of_lines-1) {
			number_of_lines *= 2;
			*names = realloc(*names,(sizeof(char *) * number_of_lines));
			types = realloc(types,(sizeof(char) * number_of_lines));
		}
	}

	free(line_buffer);
	fclose(fp);

	//check if anything should be moved/added/helped
	if (argc == 4) {
		if (strcmp(argv[1], "add") == 0) {
			add_task(names,types,&number_of_used_lines,argv[2], argv[3][0]);
		} if (strcmp(argv[1], "move") == 0) {
			move_task(names,types,number_of_used_lines,atoi(argv[2]), argv[3][0]);
		}
	} else if (argc != 1) {
		help();
	} 
	//always actually want to print the tasks
	print_tasks(names,types,number_of_used_lines);

	//should free all of the datastructures 
	for (int i = 0; i < number_of_used_lines; i++) {
		free(names[i]);
	}

	free(names);
	free(types);
	
}

//PROBLEM: NEED TO ADD A \N TO THE NAME ARGUMENT. SHOULD NOT BE THE END OF THE WORLD!	
