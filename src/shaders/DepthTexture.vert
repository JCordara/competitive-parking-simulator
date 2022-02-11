#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoordinate;

uniform mat4 Ms[10]; //Array so we can send instanced versions, use M[0] for a single value
uniform mat4 V;
uniform mat4 P;

void main() {
	gl_Position =  P * V * Ms[gl_InstanceID] * vec4(vertexPos, 1.0);
}
