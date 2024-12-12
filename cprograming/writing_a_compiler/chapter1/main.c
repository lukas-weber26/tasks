#include "./project.h"

int main(int argc, char * argv[]) {

	compiler_mode active_compiler_mode = FULL;

	if (argc == 1) {
		printf("Too few arguments. Usage: compiler <path-to-file> --flag\n");
		exit(0);
	} else if (argc == 3) {
		if (strncmp(argv[2] + 2, "lex", 3) == 0) {
			active_compiler_mode = LEX;	
		} else if (strncmp(argv[2] + 2, "parse", 5) == 0) {
			active_compiler_mode = PARSE;	
		} else if (strncmp(argv[2] + 2, "codegen", 7) == 0) {
			active_compiler_mode = CODEGEN;	
		} else if (strncmp(argv[2] + 2, "s", 1) == 0) {
			 active_compiler_mode = ASSEMBLY;	
		} else {
			printf("Invalid flag: %s. Options: lex, parse, codegen, s.\n", argv[2]);
			exit(0);
		}
	} else if (argc > 3){
		printf("Too many arguments. Usage: compiler <path-to-file> --flag\n");
		exit(0);
	}

	FILE * input_file = fopen(argv[1], "r");
	if (!input_file) {
		printf("Input file %s not found. Exiting\n", argv[1]);
		exit(0);
	}

	tokenize(input_file);	

}
