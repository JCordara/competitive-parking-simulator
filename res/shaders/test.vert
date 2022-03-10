#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 colour;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 fragPos;
out vec3 fragColour;

void main() {
	fragColour = colour;
	fragPos = vec3(M*vec4(pos,1.0));
	//fragNormal = mat3(transpose(inverse(M)))*normal;
	gl_Position =  P * V * M * vec4(pos, 1.0);
}
