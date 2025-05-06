UNAME := $(shell uname)

CC=gcc
COMMON_FLAGS=-fdiagnostics-color=always -Wall -Wextra -Wno-deprecated

GLFW=-lglfw3
SDL=""
OPENGL=""

ifeq ($(UNAME), Darwin)
	OPENGL=-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -framework CoreFoundation
	SDL=-Wl,-rpath,/Library/Frameworks -F/Library/Frameworks -framework SDL2
endif
ifeq ($(UNAME), Linux)
	OPENGL=-pthread -lm -ldl
	SDL=-lSDL2
endif



all: main_glfw_gl main_sdl_gl main_sdl

main_glfw_gl: opengl_glfw_template.c
	$(CC) $(COMMON_FLAGS) opengl_glfw_template.c -o main_glfw_gl $(GLFW) $(OPENGL) -lglad

main_sdl_gl: opengl_sdl_template.c
	$(CC) $(COMMON_FLAGS) opengl_sdl_template.c -o main_sdl_gl $(SDL) $(OPENGL) -lglad

main_sdl: sdl_template.c
	$(CC) $(COMMON_FLAGS) sdl_template.c -o main_sdl $(SDL)



clean:
	rm libglad.a
	rm main_glfw_gl
	rm main_sdl_gl
	rm main_sdl



libglad.a: glad/glad.h glad/glad.c
	gcc -c glad/glad.c && ar cr libglad.a glad.o && rm glad.o