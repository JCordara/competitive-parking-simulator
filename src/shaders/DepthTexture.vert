#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 Ms[10]; //Array so we can send instanced versions, use M[0] for a single value
uniform mat4 V;
uniform mat4 P;

void main() {
	gl_Position =  P * V * Ms[gl_InstanceID] * vec4(pos, 1.0);
}
