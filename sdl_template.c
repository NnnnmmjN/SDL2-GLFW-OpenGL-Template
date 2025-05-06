#include <stdbool.h>

#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define MANUAL_TIME_STEP 100

#define NAME "SDL Template"



bool running = true;
bool pause = false;
Uint32 time = 0U;	// ms

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

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	window = SDL_CreateWindow(NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window) {
		fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
		return_defer(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		fprintf(stderr, "Failed to create SDL renderer: %s\n", SDL_GetError());
		return_defer(1);
	}



	time = SDL_GetTicks();
	Uint32 prev_time = 0U, curr_time, dt;

	SDL_Event e;
	while (running) {
		handle_events(&e);



		SDL_SetRenderDrawColor(renderer, 24, 24, 24, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &(SDL_Rect) {200, 200, 100, 200});

		SDL_RenderPresent(renderer);



		curr_time = SDL_GetTicks();
		dt = curr_time - prev_time;
		if (!pause) time += dt;
		prev_time = curr_time;
		if (FRAME_TIME > dt) SDL_Delay(FRAME_TIME - dt);
	}

defer:
	SDL_DestroyRenderer(renderer);
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