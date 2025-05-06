# C OpenGL templates for GLFW3 and SDL2 (for MacOS and Linux)

## Requirements
- SDL2 - website or package manager
- GLFW3 - https://www.glfw.org/download
- glad - https://glad.dav1d.de

## Building
```
make
```

Last section in Makefile (building libglad.a) exists in case there is no already built one, and for convenience. 
Though initial setup (changing some paths inside Makefile) may bring some inconveniences...
