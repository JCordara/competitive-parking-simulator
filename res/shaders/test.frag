#version 330 core

in vec3 fragPos;
in vec3 fragColour;

out vec4 color;

void main() {
	color = vec4(fragColour,1.0);
}
