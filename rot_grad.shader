#shader vertex
#version 330 core

layout (location = 0) in vec2 inPos;

void main() {
	gl_Position = vec4(inPos.x, inPos.y, 0.0f, 1.0f);
}



#shader fragment
#version 330 core

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

out vec4 FragColor;

#define R 500.0

void main() {
	vec2 coord_uv = gl_FragCoord.xy / u_resolution;
	vec2 mouse_uv = u_mouse / u_resolution;

	float t = 1.0 - min(length(coord_uv - mouse_uv), R) / R;

	FragColor = vec4(
		(sin(t * (coord_uv.x + u_time)) + 1.0) / 2.0,
		(cos(t * (coord_uv.y + u_time)) + 1.0) / 2.0,
		(cos(t * (coord_uv.x + coord_uv.y + u_time)) + 1.0) / 2.0,
		1.0);
}