#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <alloca.h>
#include "cglm/cglm.h"
#include <cglm/affine-pre.h>
#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/io.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <cglm/vec3.h>
#include <cglm/vec4.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <assert.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./camera.h"
#include "./program.h"

//need a camera. Needs to be reusable. Needs to be able to provide a view matrix and projection matrix. 
//should take global width & height pointers and stay updated

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct framebuffer {
	unsigned int fbo;	
	unsigned int rbo;	
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	unsigned int texture;
	unsigned int texture_loc;
	program program;
} framebuffer;

framebuffer framebuffer_create(char * vertex_shader, char * fragment_shader, char * texture_name) {
	framebuffer new;
	glGenFramebuffers(1, &new.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, new.fbo);

	glGenTextures(1, &new.texture);
	glBindTexture(GL_TEXTURE_2D, new.texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, new.texture, 0);

	glGenRenderbuffers(1, &new.rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, new.rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, new.rbo);		

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer incomplete\n");
		exit(0);
	}
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float pane_data [] = {
		     1.0f,  1.0f, 1.0f, 1.0f,   // top right
		     1.0f, -1.0f, 1.0f, 0.0f,   // bottom right
		    -1.0f, -1.0f, 0.0f, 0.0f,   // bottom left
		    -1.0f,  1.0f, 0.0f, 1.0f    // top left 
	};

	unsigned int indices [] = {0,1,3,1,2,3};

	glGenVertexArrays(1, &new.vao);
	glBindVertexArray(new.vao);

	glGenBuffers(1, &new.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, new.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pane_data), pane_data, GL_STATIC_DRAW);	
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)(0*sizeof(float)));	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void *)(2*sizeof(float)));	

	glEnableVertexAttribArray(0);	
	glEnableVertexAttribArray(1);	
	
	glGenBuffers(1, &new.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);	

	new.program = program_create();
	new.program = program_add_shader(new.program, vertex_shader, GL_VERTEX_SHADER);
	new.program = program_add_shader(new.program, fragment_shader, GL_FRAGMENT_SHADER);
	new.program = program_complte(new.program);
	program_bind(new.program);

	new.texture_loc = glGetUniformLocation(new.program.gl_program, texture_name);	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, new.texture);
	glUniform1i(new.texture_loc, 0);	

	return new;	
}

void framebuffer_draw(framebuffer f) {
	//don't bother with framebuffer shit yet, just make it draw
	program_bind(f.program);
	glBindVertexArray(f.vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, f.texture);
	glUniform1i(f.texture_loc, 0);	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void bind_framebuffer(framebuffer f) {
	glBindFramebuffer(GL_FRAMEBUFFER, f.fbo);
}

void bind_default_framebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int screen_width = 1920, screen_height = 1080;

void process_input(GLFWwindow * window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		exit(0);	
	}
	super_camera_keypress_update(window);
}

void window_resize_function(GLFWwindow * window, int w, int h) {
	screen_width = w; 
	screen_height = h;
	//glViewport(0, 0, w, h);
}

GLFWwindow * window_create() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(screen_width, screen_height, "test_window", NULL, NULL);
	assert(window);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, window_resize_function);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, super_camera_mouse_handler);
	glfwPollEvents();
	glViewport(0, 0, screen_width, screen_height);

	glewExperimental = true;
	glewInit();
	return window;
}

void draw_start(GLFWwindow * window) {
	process_input(window);
	glClearColor(0.0, 0.0, 0.0, 1.0);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void draw_end(GLFWwindow * window) {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main() {
	GLFWwindow * window = window_create();

	framebuffer f = framebuffer_create("./frame_vertex.glsl", "./frame_fragment.glsl", "frame_texture");
	//simple_renderable pane = get_shape(PANE); 

	super_camera * camera = super_camera_create(&screen_width, &screen_height, 45.0, 0.1, 10000, 3.0);
	super_camera_bind(camera);	

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float data [] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	
	unsigned int cube_vao, cube_vbo;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);

	glGenBuffers(1, &cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);	

	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void *)(sizeof(float)*0));	
	glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void *)(sizeof(float)*3));	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	unsigned int plane_vao, plane_vbo;
	glGenVertexArrays(1, &plane_vao);
	glBindVertexArray(plane_vao);


	glPointSize(4);

	program p = program_create();
	p = program_add_shader(p, "./vertex.glsl", GL_VERTEX_SHADER);
	p = program_add_shader(p, "./fragment.glsl", GL_FRAGMENT_SHADER);
	p = program_complte(p);
	program_bind(p);

	unsigned int view = glGetUniformLocation(p.gl_program, "view");
	unsigned int view_pos = glGetUniformLocation(p.gl_program, "view_loc");
	unsigned int projection = glGetUniformLocation(p.gl_program, "projection");
	unsigned int model = glGetUniformLocation(p.gl_program, "model");

	mat4 cube_model_matrix;
	glm_mat4_identity(cube_model_matrix);	
	glm_translate_y(cube_model_matrix, 1.0);	

	mat4 plane_model_matrix;
	glm_mat4_identity(plane_model_matrix);	
	glm_scale(plane_model_matrix, (vec3){100.0,0.1,100.0});

	while(!glfwWindowShouldClose(window)) {
	
		bind_framebuffer(f);
		draw_start(window);
		program_bind(p);
		glBindVertexArray(cube_vao);
		glUniformMatrix4fv(view, 1, GL_FALSE, (float*)super_camera_view(camera));
		glUniformMatrix4fv(projection, 1, GL_FALSE, (float*)super_camera_projection(camera));
		glUniformMatrix4fv(model, 1, GL_FALSE, (float*)cube_model_matrix);
		glUniform3fv(view_pos, 1, camera->pos);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glUniformMatrix4fv(view, 1, GL_FALSE, (float*)super_camera_view(camera));
		glUniformMatrix4fv(projection, 1, GL_FALSE, (float*)super_camera_projection(camera));
		glUniformMatrix4fv(model, 1, GL_FALSE, (float*)plane_model_matrix);
		glUniform3fv(view_pos, 1, camera->pos);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	
		bind_default_framebuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		framebuffer_draw(f);

		super_camera_print(camera);
		super_camera_update(camera);
		draw_end(window);
	
	}
	
	glfwTerminate();
}


