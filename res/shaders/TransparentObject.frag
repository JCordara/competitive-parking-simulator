#version 330 core

//Fragment Input
in vec2 fragTextureCoordinate;

//Material Properties of Mesh
uniform vec3 uniformPhongAmbient;
uniform sampler2D colourTexture;
uniform vec3 modelColour;

//Outputs
layout (location = 0) out vec4 col;
  
void main() {
	col = vec4(uniformPhongAmbient,1.0) * texture(colourTexture, fragTextureCoordinate);//vec4(texture(colourTexture, fragTextureCoordinate).rgb, 1.0 - fragTextureCoordinate.x);
}
