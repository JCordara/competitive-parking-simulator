#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexColour;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in vec2 vertexTextureCoordinate;

uniform mat4 Ms[10]; //Array so we can send instanced versions, use M[0] for a single value
uniform mat4 MsInverseTransposed[10]; //Array so we can send instanced versions, use M[0] for a single value
uniform mat4 V;
uniform mat4 P;
uniform mat4 lightSpaceMatrix;

out vec3 fragPos;
out vec3 fragColour;
out vec3 fragNormal;
out vec2 fragTextureCoordinate;
out vec3 FragPosLightSpace;

void main() {
	fragPos = vec3( Ms[gl_InstanceID] * vec4(vertexPos,1.0) );
	fragColour = vertexColour;
	fragNormal = mat3(MsInverseTransposed[gl_InstanceID])*vertexNormal;
	fragTextureCoordinate = vertexTextureCoordinate;
	vec4 FragPosLightSpace1 = lightSpaceMatrix * vec4(fragPos, 1.0);
	FragPosLightSpace = FragPosLightSpace1.xyz/FragPosLightSpace1.w;
	gl_Position =  P * V * Ms[gl_InstanceID] * vec4(vertexPos, 1.0);
}
