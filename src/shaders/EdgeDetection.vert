#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexTextureCoordinate;

out vec2 fragUv2;

void main() {
	fragUv2 = vertexTextureCoordinate;
	gl_Position = vec4(vertexPos, 1.0);
}
