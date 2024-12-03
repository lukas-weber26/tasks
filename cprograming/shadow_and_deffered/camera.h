#include <stdlib.h>
#include "./cglm/cglm.h"
#include <GLFW/glfw3.h>

typedef struct super_camera {
	int * screen_width_ptr;	
	int * screen_height_ptr;	
	float FOV;
	float near_dist;
	float far_dist;
	vec3 pos;
	vec3 front;
	vec3 up;
	mat4 view;
	mat4 projection;
	mat4 projection_view;
	float last_frame;
	float delta_frame;
	float speed;
} super_camera;

static super_camera * global_camera_pointer;

void super_camera_free(super_camera * camera) {
	if (camera) {
		if (camera == global_camera_pointer) {
			global_camera_pointer = NULL;
		}
		free(camera);
	}
}

void super_camera_bind(super_camera * super_camera) {
	global_camera_pointer = super_camera;
} 

super_camera * super_camera_get_bound() {
	return global_camera_pointer;
} 

void super_camera_print(super_camera * camera) {
	if (!camera) {
		camera = super_camera_get_bound(); 
		printf("Bound camera:\n");
	}

	printf("Pos: %f, %f, %f\n", camera->pos[0], camera->pos[1], camera->pos[2]);
	printf("Front: %f, %f, %f\n", camera->front[0], camera->front[1], camera->front[2]);
	printf("Up : %f, %f, %f\n", camera->up[0], camera->up[1], camera->up[2]);

}

void super_camera_update(super_camera * camera) {
	if (!camera) {
		camera = super_camera_get_bound();
	}
	glm_perspective(glm_rad(camera->FOV), (float)(*camera->screen_width_ptr)/(float)(*camera->screen_height_ptr), camera->near_dist, camera->far_dist, camera->projection);	
	vec3 pos_front;
	glm_vec3_add(camera->pos, camera->front, pos_front);
	glm_lookat(camera->pos, pos_front, camera->up, camera->view);	
	glm_mat4_mul(camera->projection, camera->view, camera->projection_view);

	float current_frame = glfwGetTime();
	camera->delta_frame = current_frame - camera->last_frame;
	camera->last_frame = current_frame;
}

super_camera * super_camera_create (int * global_screen_width, int * global_screen_height, float FOV, float near, float far, float speed) {
	super_camera * new_camera = (super_camera *)calloc(1, sizeof(super_camera));

	new_camera->screen_height_ptr = global_screen_height;	
	new_camera->screen_width_ptr = global_screen_width;	

	new_camera->FOV = FOV;	
	new_camera->near_dist = near;	
	new_camera->far_dist = far;	
	new_camera->speed = speed;	

	glm_vec3_copy((vec3){0.0, 0.0, 1.0}, new_camera->pos);
	glm_vec3_copy((vec3){1.0, 0.0,0.0}, new_camera->front);
	glm_vec3_copy((vec3){0.0, 1.0,0.0}, new_camera->up);

	new_camera->last_frame = 0.0;
	new_camera->delta_frame = 0.0;

	super_camera_update(new_camera);
	return new_camera;
}

mat4 * super_camera_projection(super_camera * camera) {
	return &camera->projection;
}

mat4 * super_camera_view(super_camera * camera) {
	return &camera->view;
}

mat4 * super_camera_projection_view(super_camera * camera) {
	return &camera->projection_view;
}

void super_camera_keypress_update(GLFWwindow * window) {	
	super_camera * camera = super_camera_get_bound();
	float camera_speed = camera->speed * camera->delta_frame;
	printf("Camera speed: %f\n", camera_speed);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		vec3 diff;
		glm_vec3_scale(camera->front, 2*camera_speed, diff);
		glm_vec3_add(camera->pos, diff, camera->pos);	
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		vec3 diff;
		glm_vec3_scale(camera->front, -camera_speed, diff);
		glm_vec3_add(camera->pos, diff, camera->pos);	
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		vec3 diff;
		vec3 cross;
		glm_cross(camera->front, camera->up, cross);
		glm_normalize(cross);
		glm_vec3_scale(cross, -camera_speed, diff);
		glm_vec3_add(camera->pos, diff, camera->pos);	
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		vec3 diff;
		vec3 cross;
		glm_cross(camera->front, camera->up, cross);
		glm_normalize(cross);
		glm_vec3_scale(cross, camera_speed, diff);
		glm_vec3_add(camera->pos, diff, camera->pos);	
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
		vec3 diff;
		glm_vec3_scale(camera->up, camera_speed, diff);
		glm_vec3_add(camera->pos, diff, camera->pos);	
	}
	if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
		vec3 diff;
		glm_vec3_scale(camera->up, -camera_speed, diff);
		glm_vec3_add(camera->pos, diff, camera->pos);	
	}
}

void super_camera_mouse_handler(GLFWwindow * window, double xpos, double ypos) {
	super_camera * c = super_camera_get_bound();
	static int first_mouse = true;
	static float lastX = 400;
	static float lastY = 400;
	static float pitch = 0;
	static float yaw = 0;
	float xoffset = (xpos - lastX) *  c->delta_frame;
	float yoffset = (ypos - lastY) * c->delta_frame;

	if (first_mouse) {
		lastX = xpos;
		lastY = ypos;
		first_mouse = false;
	}

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0)
		pitch = 89.0;
	if (pitch < -89.0)
		pitch = -89.0;

	vec3 direction;
	direction[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
	direction[1] = -glm_rad(pitch);
	direction[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
	glm_normalize_to(direction, c->front);

	lastX = xpos;
	lastY = ypos;
}
