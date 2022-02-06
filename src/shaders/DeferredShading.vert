#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vertexTextureCoordinate;

#define MAX_NUMBER_INSTANCES 100
uniform mat4 Ms[MAX_NUMBER_INSTANCES]; //Array so we can send instanced versions, use M[0] for a single value
uniform mat4 MsInverseTransposed[MAX_NUMBER_INSTANCES]; //Array so we can send instanced versions, use M[0] for a single value
uniform mat4 cameraV;
uniform mat4 cameraP;
uniform mat4 directionalLightCameraV;
uniform mat4 directionalLightCameraP;

out vec3 fragPos;
out vec3 fragColour;
out vec3 fragNormal;
out vec2 fragTextureCoordinate;
out vec3 FragPosLightSpace;

void main() {
	fragPos = vec3( Ms[gl_InstanceID] * vec4(vertexPos,1.0) );
	fragNormal = mat3(MsInverseTransposed[gl_InstanceID])*vertexNormal;
	fragTextureCoordinate = vertexTextureCoordinate;
	vec4 FragPosLightSpace1 = directionalLightCameraP * directionalLightCameraV * vec4(fragPos, 1.0);
	FragPosLightSpace = FragPosLightSpace1.xyz/FragPosLightSpace1.w;
	gl_Position =  cameraP * cameraV * Ms[gl_InstanceID] * vec4(vertexPos, 1.0);
}
