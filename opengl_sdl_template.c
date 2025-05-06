#include <glad/glad.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define MANUAL_TIME_STEP 100

#define NAME "SDL OpenGL Template"
#define SHADER "rot_grad.shader"

#define GL_IMPLEMENTATION
#include "load_shader.h"



bool running = true;
bool pause = false;
Uint32 time = 0U;	// ms

GLuint shader;

#define FPS 30
#define FRAME_TIME (1000/FPS)



#define return_defer(res) do { result = (res); goto defer; } while (0)
void handle_events(SDL_Event *e);



int main() {
	int result = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);



	SDL_Window *window = NULL;
	SDL_GLContext context = NULL;

	window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	if (!window) {
		fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
		return_defer(1);
	}

	context = SDL_GL_CreateContext(window);
	if (!context) {
		fprintf(stderr, "Failed to create SDL OpenGL context: %s\n", SDL_GetError());
		return_defer(1);
	}

	SDL_GL_SetSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
		fprintf(stderr, "Failed to initialize GLAD\n");
		return_defer(1);
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



	glClearColor(0.25f, 0.0f, 0.0f, 1.0f);



	time = SDL_GetTicks();
	Uint32 prev_time = 0U, curr_time, dt;

	SDL_Event e;
	while (running) {
		handle_events(&e);



		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		// resolution
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		glUniform2f(glGetUniformLocation(shader, "u_resolution"), (float) width*2.0f, (float) height*2.0f);
		// mouse position
		int x_pos, y_pos;
		SDL_GetMouseState(&x_pos, &y_pos);
		glUniform2f(glGetUniformLocation(shader, "u_mouse"), (float) x_pos*2.f, (float) (height-y_pos)*2.f);
		// time
		glUniform1f(glGetUniformLocation(shader, "u_time"), time * 1e-3f);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		SDL_GL_SwapWindow(window);



		curr_time = SDL_GetTicks();
		dt = curr_time - prev_time;
		if (!pause) time += dt;
		prev_time = curr_time;
		if (FRAME_TIME > dt) SDL_Delay(FRAME_TIME - dt);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteProgram(shader);
defer:
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return result;
}



void handle_events(SDL_Event *e) {
	while (SDL_PollEvent(e)) {
		switch (e->type) {
		case SDL_QUIT:
		case SDL_APP_TERMINATING:
			running = false;
			break;
		case SDL_KEYDOWN:
			switch (e->key.keysym.sym) {
			case SDLK_ESCAPE:
				running = false;
				break;
			case SDLK_SPACE:
				pause = !pause;
				break;
			case SDLK_r:
				glDeleteProgram(shader);
				shader = CreateShader(SHADER);
				break;
			}

			if (pause) {
				switch (e->key.keysym.sym) {
				case SDLK_LEFT:
					time -= MANUAL_TIME_STEP;
					break;
				case SDLK_RIGHT:
					time += MANUAL_TIME_STEP;
					break;
				}
			}
			break;
		}
	}
}