#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <>

GLFWwindow * window_create() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(width, height, "test_window", NULL, NULL);
	assert(window);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, window_resize_function);
	glfwPollEvents();
	glViewport(0, 0, width, height);

	glewExperimental = true;
	glewInit();
	return window;
}
