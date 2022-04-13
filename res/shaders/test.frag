#version 330 core

in vec2 fragUV;

uniform sampler2D _texture;

out vec4 color;

void main() {
	color = texture(_texture, fragUV);
}
