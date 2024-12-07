#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <alloca.h>
#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/cglm.h>
#include <cglm/affine-mat.h>
#include <cglm/affine-pre.h>
#include <cglm/mat4.h>
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef enum {TRIANGLE, PANE} shape;
int v_toggle = 0;
int width = 1920, height = 1080;

void process_input(GLFWwindow * window) {
	static int block = 0;
	block = block - 1;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		if (block < 0) {
			v_toggle  = (v_toggle + 1) %2;
			block = 10;
		}

	}
	super_camera_keypress_update(window);
}

void window_resize_function(GLFWwindow * window, int w, int h) {
	width = w; 
	height = h;
	//glViewport(0, 0, w, h);
}

GLFWwindow * window_create() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(width, height, "test_window", NULL, NULL);
	assert(window);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, window_resize_function);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, super_camera_mouse_handler);
	glfwPollEvents();
	glViewport(0, 0, width, height);

	glewExperimental = true;
	glewInit();
	
	glClearColor(1.0, 1.0, 1.0, 1.0);	
	glPointSize(4);
	glEnable(GL_DEPTH_TEST);

	return window;
}

unsigned int get_pane_vao() {
	float data [] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
	};	

	unsigned int VAO, VBO;

	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glCreateBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void *)(sizeof(float)*0));	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void *)(sizeof(float)*2));	
	glEnableVertexAttribArray(0);	
	glEnableVertexAttribArray(1);	

	return VAO;
}

void draw_pane_vao(unsigned int VAO) {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

unsigned int get_cube_vao() {
	float data [] = {
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f,  
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  
	};	

	unsigned int VAO, VBO;

	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glCreateBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void *)(sizeof(float)*0));	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void *)(sizeof(float)*3));	
	glEnableVertexAttribArray(0);	
	glEnableVertexAttribArray(1);	

	return VAO;
}

void draw_cube_vao(unsigned int VAO) {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

unsigned int get_simple_texture() {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int w,h,c;	
	unsigned char * tex_data = stbi_load("./texture.jpg", &w, &h, &c,0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);	
	stbi_image_free(tex_data);

	return texture;
}

int main() {
	GLFWwindow * window = window_create();
	super_camera * camera = super_camera_create(&width, &height, 45.0, 0.1, 1000, 10);
	super_camera_bind(camera);

	////setting up the pane
	unsigned int pane_VAO = get_pane_vao();
	unsigned int texture = get_simple_texture();
	
	program pane_program = program_create();
	pane_program = program_add_shader(pane_program, "./pane_vertex.glsl", GL_VERTEX_SHADER);
	pane_program = program_add_shader(pane_program, "./pane_fragment.glsl", GL_FRAGMENT_SHADER);
	pane_program = program_complte(pane_program);
	program_bind(pane_program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	unsigned int texture_loc = glGetUniformLocation(pane_program.gl_program, "./tex_image");
	glUniform1i(texture_loc, 0);
	////end of pane setup	
	
	////setting up the cube 
	unsigned int cube_VAO = get_cube_vao();
	
	program cube_program = program_create();
	cube_program = program_add_shader(cube_program, "./cube_vertex.glsl", GL_VERTEX_SHADER);
	cube_program = program_add_shader(cube_program, "./cube_fragment.glsl", GL_FRAGMENT_SHADER);
	cube_program = program_complte(cube_program);
	program_bind(cube_program);

	unsigned int cube_projection_view_loc = glGetUniformLocation(cube_program.gl_program, "projection_view");
	unsigned int cube_model_loc = glGetUniformLocation(cube_program.gl_program, "model");

	program shadow_cube_program = program_create();
	shadow_cube_program = program_add_shader(shadow_cube_program, "./shadow_cube_vertex.glsl", GL_VERTEX_SHADER);
	shadow_cube_program = program_add_shader(shadow_cube_program, "./shadow_cube_fragment.glsl", GL_FRAGMENT_SHADER);
	shadow_cube_program = program_complte(shadow_cube_program);
	program_bind(shadow_cube_program);

	unsigned int shadow_cube_projection_view_loc = glGetUniformLocation(shadow_cube_program.gl_program, "projection_view");
	unsigned int shadow_cube_model_loc = glGetUniformLocation(shadow_cube_program.gl_program, "model");
	unsigned int shadow_cube_light_view_loc = glGetUniformLocation(shadow_cube_program.gl_program, "light_view");
	unsigned int shadow_texture_loc = glGetUniformLocation(shadow_cube_program.gl_program, "tex_image");

	mat4 cube_1_model;
	glm_mat4_identity(cube_1_model);
	//end of cube setup	
	
	//framebuffer 
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);	
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int fbo_texture;
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture,0);	
	
	unsigned int depth_texture;
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1920, 1080, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0); 

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer borked\n");
		exit(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	

	glEnable(GL_CULL_FACE);
	while(!glfwWindowShouldClose(window)) {
		process_input(window);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		program_bind(cube_program);
		//glUniformMatrix4fv(cube_projection_view_loc, 1, GL_FALSE, (float *)*super_camera_projection_view(camera));
		
		mat4 ortho_cam;
		mat4 ortho_proj;
		mat4 ortho_view;
		glm_ortho(-30, 30, -30, 30, 1.0, 200.0, ortho_proj);	
		glm_lookat((vec3){20.0, 20.0, 20.0}, (vec3){0,0,0}, (vec3){0.0, 1.0, 0.0}, ortho_view);
		glm_mat4_mul(ortho_proj, ortho_view, ortho_cam);

		glUniformMatrix4fv(cube_projection_view_loc, 1, GL_FALSE, (float *) ortho_cam);

		glCullFace(GL_FRONT);
		mat4 local_model;
		for (int i = -5; i < 5; i++) {
			for (int j = -5; j < 5; j++) {
				glm_translate_to(cube_1_model, (vec3){(float)i*3.0,0.0, (float)j*3.0}, local_model);	
				glUniformMatrix4fv(cube_model_loc, 1, GL_FALSE, (float *)local_model);
				draw_cube_vao(cube_VAO);	
			}
		}
				
		glm_scale_to(cube_1_model, (vec3){25.0,1.0,25.0}, local_model);	
		glm_translate_y(local_model, -5.0);
		glUniformMatrix4fv(cube_model_loc, 1, GL_FALSE, (float *)local_model);
		draw_cube_vao(cube_VAO);	

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);

		glCullFace(GL_BACK);
		if (v_toggle == 1) {
			program_bind(shadow_cube_program);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depth_texture);
			glUniform1i(shadow_texture_loc, 0);

			glUniformMatrix4fv(cube_projection_view_loc, 1, GL_FALSE, (float *)super_camera_projection_view(camera));
			glUniformMatrix4fv(shadow_cube_light_view_loc, 1, GL_FALSE, (float *)ortho_cam);

			for (int i = -5; i < 5; i++) {
				for (int j = -5; j < 5; j++) {
					glm_translate_to(cube_1_model, (vec3){(float)i*3.0,0.0, (float)j*3.0}, local_model);	
					glUniformMatrix4fv(cube_model_loc, 1, GL_FALSE, (float *)local_model);
					draw_cube_vao(cube_VAO);	
				}
			}
					
			glm_scale_to(cube_1_model, (vec3){25.0,1.0,25.0}, local_model);	
			glm_translate_y(local_model, -5.0);
			glUniformMatrix4fv(cube_model_loc, 1, GL_FALSE, (float *)local_model);
			draw_cube_vao(cube_VAO);	
		} else {
			program_bind(pane_program);	
			glActiveTexture(GL_TEXTURE0);	
			glBindTexture(GL_TEXTURE_2D, depth_texture);
			glUniform1i(texture_loc, 0);
			draw_pane_vao(pane_VAO);
		}

		super_camera_update(camera);
		super_camera_print(camera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

