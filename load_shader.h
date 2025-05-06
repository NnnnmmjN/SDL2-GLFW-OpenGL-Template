/*
//	INCLUDING + DEFINITION:
//		#define GL_IMPLEMENTATION
//		#include "load_shader.h"

//	USAGE:
//		GLuint shader = CreateShader("SHADER_PATH");
//		glUseProgram(shader);
//
//		...
//		
//		glDeleteProgram(shader);
*/

#ifndef LOAD_SHADER_H
#define LOAD_SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <glad/glad.h>

GLuint CreateShader(const char *file_path);

#endif // LOAD_SHADER_H



#ifdef GL_IMPLEMENTATION

struct string {
	char *str;
	size_t len;
};

struct ShaderProgramSource {
	struct string VertexSource;
	struct string FragmentSource;
};

struct ShaderProgramSource _ParseShader(const char *file_path) {
	enum ShaderType { VERTEX = 0, FRAGMENT };
	struct string source_str[2] = {0};
	struct ShaderProgramSource source = {0};

	FILE *stream = fopen(file_path, "r");
	if (stream == NULL) {
		fprintf(stderr, "Could not open shader source file: %s\n", file_path);
		return source;
	}

	fseek(stream, 0L, SEEK_END);
	const size_t file_len = ftell(stream);
	rewind(stream);

	char shader_source[file_len];
	int64_t vertex_pos = 0LL, fragment_pos = 0LL;
	fpos_t pos;

	while (!feof(stream)) {
		fscanf(stream, "%s", shader_source);
		if (strcmp(shader_source, "#shader") == 0) {
			fscanf(stream, "%s", shader_source);
			if (strcmp(shader_source, "vertex") == 0) {
				fgetc(stream);
				fgetpos(stream, &pos);
				vertex_pos = *(int64_t *) &pos;
			} else if (strcmp(shader_source, "fragment") == 0) {
				fgetc(stream);
				fgetpos(stream, &pos);
				fragment_pos = *(int64_t *) &pos;
			}
		}
	}
	
	source_str[VERTEX].len = fragment_pos - vertex_pos - 17 + 1;
	source_str[VERTEX].str = (char *) malloc(source_str[VERTEX].len * sizeof(char));
	if (source_str[VERTEX].str == NULL) {
		fprintf(stderr, "Could not allocate memory for Vertex Shader\n");
		goto defer;
	}
	pos = *(fpos_t *) &vertex_pos;
	fsetpos(stream, &pos);
	for (size_t i = 0; i < source_str[VERTEX].len; i++) source_str[VERTEX].str[i] = (char) fgetc(stream);
	source_str[VERTEX].str[source_str[VERTEX].len - 1] = '\0';

	source_str[FRAGMENT].len = file_len - fragment_pos + 1;
	source_str[FRAGMENT].str = (char *) malloc(source_str[FRAGMENT].len * sizeof(char));
	if (source_str[FRAGMENT].str == NULL) {
		fprintf(stderr, "Could not allocate memory for Fragment Shader\n");
		free(source_str[VERTEX].str);
		goto defer;
	}
	pos = *(fpos_t *) &fragment_pos;
	fsetpos(stream, &pos);
	for (size_t i = 0; i < source_str[FRAGMENT].len; i++) source_str[FRAGMENT].str[i] = (char) fgetc(stream);
	source_str[FRAGMENT].str[source_str[FRAGMENT].len - 1] = '\0';

	source = (struct ShaderProgramSource) {
		.VertexSource = source_str[VERTEX],
		.FragmentSource = source_str[FRAGMENT]
	};
	
defer:
	fclose(stream);
	return source;
}

GLuint _CompileShader(GLuint type, const struct string *source) {
	GLuint id = glCreateShader(type);
	glShaderSource(id, 1, (const GLchar *const *) &source->str, NULL);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char message[length];
		glGetShaderInfoLog(id, length, &length, message);
		const char *type_str = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
		printf("Failed to compile %s shader: %s\n", type_str, message);
		glDeleteShader(id);
		return 0;
	}

	return id;
}

GLuint CreateShader(const char *file_path) {
	struct ShaderProgramSource source = _ParseShader(file_path);
	if (source.VertexSource.len == 0 || source.FragmentSource.len == 0) goto err;

	GLuint program = glCreateProgram();
	GLuint vs = _CompileShader(GL_VERTEX_SHADER, &source.VertexSource);
	GLuint fs = _CompileShader(GL_FRAGMENT_SHADER, &source.FragmentSource);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	if (source.VertexSource.str != NULL) free(source.VertexSource.str);
	if (source.FragmentSource.str != NULL) free(source.FragmentSource.str);

	return program;
err:
	if (source.VertexSource.str != NULL) free(source.VertexSource.str);
	if (source.FragmentSource.str != NULL) free(source.FragmentSource.str);
	
	return 0;
}

#undef GL_IMPLEMENTATION
#endif // GL_IMPLEMENTATION