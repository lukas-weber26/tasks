#include <GL/glew.h>
#include "./cglm/cglm.h"
#include <GLFW/glfw3.h>

typedef struct program {
	unsigned int gl_program;
	unsigned int shader_ids[5];
	unsigned int shader_active[5];
	unsigned int complete;
} program;

program program_create() {
	program new_program;
	new_program.gl_program = glCreateProgram();
	new_program.complete = 0;
	for (int i = 0; i < 5; i++) {
		new_program.shader_ids[i] = 0;
		new_program.shader_active[i] = 0;
	}
	return new_program;
}

void program_bind(program program) {
	glUseProgram(program.gl_program);
}

program program_add_shader(program input_program, char * shader_file, GLenum shader_type) {
	int target_index;
	switch(shader_type) {
		case GL_VERTEX_SHADER: target_index = 0; break;
		case GL_FRAGMENT_SHADER: target_index = 1; break;
		case GL_COMPUTE_SHADER: target_index = 2; break;
		case GL_TESS_CONTROL_SHADER: target_index = 3; break;
		case GL_TESS_EVALUATION_SHADER: target_index = 4; break;
		default: printf("Invalid shader type.\n"); exit(0);
	};

	if (input_program.shader_active[target_index] != 0) {
		printf("This shader type has already been bound.\n");
	}

	input_program.shader_active[target_index] = 1;
	input_program.shader_ids[target_index] = glCreateShader(shader_type);

	int size = 4024;
	FILE * shader_fp = fopen(shader_file, "r");
	char * shader_source = malloc(size * sizeof(char));
	int count = fread(shader_source, sizeof(char), size - 1, shader_fp);	
	shader_source[count] = '\0';

	while (count == size-1) {
		free(shader_source);
		size *= 2;
		shader_source = malloc(size * sizeof(char));
		count = fread(shader_source, sizeof(char), size - 1, shader_fp);	
		shader_source[count] = '\0';
	}

	fclose(shader_fp);
	const char * const_shader_source = shader_source; 
	glShaderSource(input_program.shader_ids[target_index], 1, &const_shader_source, NULL);
	glCompileShader(input_program.shader_ids[target_index]);

	int success;
	char log [1024];

	glGetShaderiv(input_program.shader_ids[target_index], GL_COMPILE_STATUS, &success);	
	if (!success)	 {
		glGetShaderInfoLog(input_program.shader_ids[target_index], 1024, &success, log);	
		printf("%s: %s\n", shader_file, log);
		exit(0);
	}

	free(shader_source);
	glAttachShader(input_program.gl_program, input_program.shader_ids[target_index]);
	return input_program;	
}

program program_complte(program input_program) { 
	glLinkProgram(input_program.gl_program);

	int success;
	glGetProgramiv(input_program.gl_program, GL_LINK_STATUS, &success);	
	char log[1024];
	if (!success)	 {
		glGetProgramInfoLog(input_program.gl_program, 1024, &success, log);	
		printf("%s\n", log);
		exit(0);
	}

	for (int i = 0; i < 5; i++) {
		if (input_program.shader_active[i] == 1) {
			glDeleteShader(input_program.shader_ids[i]);
		}
	}

	glUseProgram(input_program.gl_program);	
	input_program.complete= 1;
	return input_program;
}


