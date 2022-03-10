#version 330 core

//Fragment Input
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTextureCoordinate;
in vec3 FragPosLightSpace;//For directional light shadows

//Material Properties of Mesh
uniform vec3 uniformPhongDiffuse;
uniform vec3 uniformPhongSpecular;
uniform vec3 uniformPhongAmbient;
uniform float uniformPhongAlpha;
uniform sampler2D colourTexture;
uniform vec3 modelColour;

//Outputs
layout (location = 0) out vec3 textureColour;
layout (location = 1) out vec3 textureClassification;
layout (location = 2) out vec3 texturePosition;
layout (location = 3) out vec3 textureNormal;
layout (location = 4) out vec3 textureDiffuseConstant;
layout (location = 5) out vec4 textureSpecularAndShinnyConstant;
layout (location = 6) out vec3 textureAmbientConstant;
layout (location = 7) out vec3 textureDirectionalLightPosition;
  
void main() {
	textureColour = texture(colourTexture,fragTextureCoordinate).rgb;
	textureClassification = modelColour;
	texturePosition = fragPos;
	textureNormal = normalize(fragNormal);
	textureDiffuseConstant = uniformPhongDiffuse;
	textureSpecularAndShinnyConstant = vec4(uniformPhongSpecular, uniformPhongAlpha);
	textureAmbientConstant = uniformPhongAmbient;
	textureDirectionalLightPosition = FragPosLightSpace;
}
