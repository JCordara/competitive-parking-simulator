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
uniform sampler2D colourText;
uniform vec3 modelColour;

//Point Lights
#define MAX_NUMBER_POINT_LIGHTS 10
uniform vec3 lightPositions[MAX_NUMBER_POINT_LIGHTS];
uniform vec3 lightColours[MAX_NUMBER_POINT_LIGHTS];
uniform vec3 lightAttenuationConstaints[MAX_NUMBER_POINT_LIGHTS];
uniform float lightRadius[MAX_NUMBER_POINT_LIGHTS];
uniform int numberOfLights;

//Spot Lights
#define MAX_NUMBER_SPOT_LIGHTS 10
uniform vec3 spotLightPositions[MAX_NUMBER_SPOT_LIGHTS];
uniform vec3 spotLightColours[MAX_NUMBER_SPOT_LIGHTS];
uniform vec3 spotLightAttenuationConstaints[MAX_NUMBER_SPOT_LIGHTS];
uniform vec3 spotLightDirections[MAX_NUMBER_SPOT_LIGHTS];
uniform float spotLightRadius[MAX_NUMBER_SPOT_LIGHTS];
uniform float spotLightCosInnerAngles[MAX_NUMBER_SPOT_LIGHTS];
uniform float spotLightCosOuterAngles[MAX_NUMBER_SPOT_LIGHTS];
uniform int numberOfSpotLight;

//Directional Light
uniform vec3 directionalLightDirection;
uniform vec3 directionalLightColour;
uniform sampler2D shadowMap;
#define MAX_BIAS 0.002
#define MIN_BIAS 0.0005

//Ambient Light
uniform vec3 ambientLight;

//Camera Data
uniform vec3 renderCameraPosition;

//Outputs
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 color_unshaded;

//Cell Shading Constants
#define CELL_LAYER_COUNT_DIFFUSE 0
#define CELL_LAYER_COUNT_SPECULAR CELL_LAYER_COUNT_DIFFUSE + 0

float attenuate(vec3 constaints, float dis){
	return 1 / max(constaints[0] + constaints[1]*dis + constaints[1]*dis*dis,0.0001);
}

vec3 diffuse(vec3 lightDir, vec3 normal, vec3 k){
	return k * max( dot( lightDir, normal), 0.0);
}

vec3 specular(vec3 lightDir, vec3 normal, vec3 vDir, vec3 k, float a){
	if(a < 0.0000001) return vec3(0.0); 
	vec3 halfwayDir = normalize(lightDir + vDir);  
	return k * pow(max(dot(normal, halfwayDir), 0.0), a);
}

float spotLightEdgeFactor(float outer, float inner, float value){
	return max(min( (value - outer)/(inner - outer), 1.0), 0.0);
}

float directionalLightShadow(vec3 fragPositionInLight, float bias){
    vec3 projCoords = fragPositionInLight * 0.5 + 0.5;
	if(projCoords.z < 0.0 || projCoords.x < 0.0 || projCoords.y < 0.0
	||projCoords.z > 1.0 || projCoords.x > 1.0 || projCoords.y > 1.0)
		return 0.0;
    float currentDepth = projCoords.z;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
    return shadow;
}

vec3 cellShader(vec3 light, int bands){
	if(bands < 2) return light;
	vec3 nf = floor(light * vec3(bands));
	light = (nf)*(1.0 / vec3(bands));
	return light;
}
  
void main() {
	vec3 normal = normalize(fragNormal);
	vec3 vDir = normalize(renderCameraPosition - fragPos);
	//Point Light, specular and diffuse
	vec3 phonglightAccumulator = vec3(0.0,0.0,0.0), lightDir, diffuseFactor, specularFactor;
	float distanceToLight, attenuateFactor;
	for(int i = 0; i < min(numberOfLights,MAX_NUMBER_POINT_LIGHTS); i++){
		lightDir = lightPositions[i] - fragPos;
		distanceToLight = length(lightDir);
		if(distanceToLight < lightRadius[i]){
			attenuateFactor = attenuate(lightAttenuationConstaints[i], distanceToLight);
			lightDir = normalize(lightDir);
			diffuseFactor = cellShader(attenuateFactor * diffuse(lightDir, normal, uniformPhongDiffuse), CELL_LAYER_COUNT_DIFFUSE);
			specularFactor = cellShader(attenuateFactor * specular(lightDir, normal, vDir, uniformPhongSpecular, uniformPhongAlpha), CELL_LAYER_COUNT_SPECULAR);
			phonglightAccumulator += (diffuseFactor + specularFactor) * lightColours[i];
		}
	}
	//Spot Light, specular and diffuse
	float cosAngle, spotlightFactor;
	for(int i = 0; i < min(numberOfSpotLight, MAX_NUMBER_SPOT_LIGHTS); i++){
		lightDir = spotLightPositions[i] - fragPos;
		distanceToLight = length(lightDir);
		lightDir = normalize(lightDir);
		cosAngle = dot(-lightDir,spotLightDirections[i]);
		if( (distanceToLight < spotLightRadius[i]) && (cosAngle > spotLightCosOuterAngles[i])){
			attenuateFactor = attenuate(spotLightAttenuationConstaints[i], distanceToLight);
			spotlightFactor = spotLightEdgeFactor(spotLightCosOuterAngles[i], spotLightCosInnerAngles[i], cosAngle);
			diffuseFactor = cellShader(attenuateFactor * spotlightFactor * diffuse(lightDir, normal, uniformPhongDiffuse), CELL_LAYER_COUNT_DIFFUSE);
			specularFactor = cellShader(attenuateFactor * spotlightFactor * specular(lightDir, normal, vDir, uniformPhongSpecular, uniformPhongAlpha), CELL_LAYER_COUNT_SPECULAR);
			phonglightAccumulator += (diffuseFactor + specularFactor) * spotLightColours[i];
		}
	}
	//Directional Light, specular and diffuse + shadow mapping
	float shadowfactor;
	diffuseFactor = cellShader(diffuse(-directionalLightDirection, normal, uniformPhongDiffuse),CELL_LAYER_COUNT_DIFFUSE);
	specularFactor = cellShader(specular(-directionalLightDirection, normal, vDir, uniformPhongSpecular, uniformPhongAlpha), CELL_LAYER_COUNT_SPECULAR);
	shadowfactor = (1.0 - directionalLightShadow(FragPosLightSpace, max(MAX_BIAS * (1.0 - dot(normal, lightDir)), MIN_BIAS)));
	phonglightAccumulator +=  (diffuseFactor + specularFactor) * shadowfactor * directionalLightColour;
	//Ambient Light
	phonglightAccumulator += uniformPhongAmbient * ambientLight;
	//Final Result
	color = texture(colourText,fragTextureCoordinate) * vec4(phonglightAccumulator,1.0);
	color_unshaded = vec4(modelColour, 1.0);//For rendering edge detection
}
