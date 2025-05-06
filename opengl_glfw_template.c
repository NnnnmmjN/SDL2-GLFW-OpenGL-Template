#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define MANUAL_TIME_STEP 0.1

#define NAME "GLFW OpenGL Template"
#define SHADER "rot_grad.shader"

#define GL_IMPLEMENTATION
#include "load_shader.h"



bool running = true;
bool pause = false;
double time = 0.0;

GLuint shader;



void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);



int main() {
	if (!glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, NAME, NULL, NULL);
	if (!window) {
		fprintf(stderr, "Failed to create GLFW window\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		glfwTerminate();
		return 1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	shader = CreateShader(SHADER);
	glUseProgram(shader);



	GLfloat vertices[] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f,
	};

	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);



	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glfwSetKeyCallback(window, key_callback);

	time = glfwGetTime();
	double prev_time = 0.;

	// glfwMakeContextCurrent(win2);

#if 1
	while (!glfwWindowShouldClose(window)) {
		glfwMakeContextCurrent(window);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		// resolution
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glUniform2f(glGetUniformLocation(shader, "u_resolution"), (float) width*2.0f, (float) height*2.0f);
		// mouse position
		double x_pos, y_pos;
		glfwGetCursorPos(window, &x_pos, &y_pos);
		glUniform2f(glGetUniformLocation(shader, "u_mouse"), (float) x_pos*2.f, (float) (height-y_pos)*2.f);
		// time
		glUniform1f(glGetUniformLocation(shader, "u_time"), time);

		glBindVertexArray(VAO);
		// glDrawArrays(GL_TRIANGLES, 0, 4*2);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		glfwSwapBuffers(window);

		// glfwMakeContextCurrent(win2);
		// glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		// glClear(GL_COLOR_BUFFER_BIT);
		// glfwSwapBuffers(win2);

		glfwPollEvents();
		
		double curr_time = glfwGetTime();
		if (!pause) time += curr_time - prev_time;
		prev_time = curr_time;
	}
#endif

	// glfwMakeContextCurrent(window);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}



void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	(void) scancode;
	(void) mods;

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			running = false;
			break;
		case GLFW_KEY_SPACE:
			pause = !pause;
			break;
		case GLFW_KEY_R:
			glDeleteProgram(shader);
			shader = CreateShader(SHADER);
			break;
		}

		if (pause) {
			switch (key) {
			case GLFW_KEY_LEFT:
				time -= MANUAL_TIME_STEP;
				break;
			case GLFW_KEY_RIGHT:
				time += MANUAL_TIME_STEP;
				break;
			}
		}
	}

	if (!running) glfwSetWindowShouldClose(window, GL_TRUE);
}