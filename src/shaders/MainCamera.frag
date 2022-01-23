#version 330 core

in vec3 fragPos;
in vec3 fragColour;
in vec3 fragNormal;
in vec2 fragTextureCoordinate;
in vec3 FragPosLightSpace;

uniform int useColour;
uniform sampler2D colourText;

#define MAX_NUMBER_POINT_LIGHTS 10
uniform vec3 lightPositions[MAX_NUMBER_POINT_LIGHTS];
uniform vec3 lightColours[MAX_NUMBER_POINT_LIGHTS];
uniform vec3 lightAttenuationConstaints[MAX_NUMBER_POINT_LIGHTS];
uniform float lightRadius[MAX_NUMBER_POINT_LIGHTS];
uniform int numberOfLights;

#define MAX_NUMBER_SPOT_LIGHTS 10
uniform vec3 spotLightPositions[MAX_NUMBER_SPOT_LIGHTS];
uniform vec3 spotLightColours[MAX_NUMBER_SPOT_LIGHTS];
uniform vec3 spotLightAttenuationConstaints[MAX_NUMBER_SPOT_LIGHTS];
uniform vec3 spotLightDirections[MAX_NUMBER_SPOT_LIGHTS];
uniform float spotLightRadius[MAX_NUMBER_SPOT_LIGHTS];
uniform float spotLightCosInnerAngles[MAX_NUMBER_SPOT_LIGHTS];
uniform float spotLightCosOuterAngles[MAX_NUMBER_SPOT_LIGHTS];
uniform int numberOfSpotLight;

uniform vec3 directionalLightDirection;
uniform vec3 directionalLightColour;
uniform int useShadowMap;
uniform sampler2D shadowMap;

uniform vec3 ambientLight;

uniform vec4 uniformPhongConstaints;//  (K-Diff, K-Spec, Alpha, K-Amb)
uniform vec3 renderCameraPosition;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 color_unshaded;



#define CELL_LAYER_COUNT_DIFFUSE 0
#define CELL_LAYER_COUNT_SPECULAR CELL_LAYER_COUNT_DIFFUSE + 0


float attenuate(vec3 constaints, float dis){
	return 1 / max(constaints[0] + constaints[1]*dis + constaints[1]*dis*dis,0.0001);
}

vec3 diffuse(vec3 col, vec3 lightDir, vec3 normal, float k){
	return k  *max( dot( lightDir, normal), 0.0) * col;
}

vec3 specular(vec3 col, vec3 lightDir, vec3 normal, vec3 vDir, float k, float a){
	if(a < 0.00001) return k * col; 
	vec3 halfwayDir = normalize(lightDir + vDir);  
	return k * pow(max(dot(normal, halfwayDir), 0.0), a) * col;
}

vec3 spotLightEdgeFactor(float outer, float inner, float value ){
	return max(min( (value - outer)/(inner - outer), 1.0), 0.0);
}

float directionalLightShadow(vec3 fragPositionInLight, float bias){
    vec3 projCoords = fragPositionInLight * 0.5 + 0.5;
	if(projCoords.z > 1.0) return 0.0;
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

vec4 sampleTexture(){
	return texture(colourText,fragTextureCoordinate);
}
  
void main() {
	vec4 baseColour;
	if(useColour != 0){ baseColour = vec4(fragColour, 1.0);}
	//else{ baseColour = sampleTexture();}

	color_unshaded = baseColour;//FOr rendering edge detection

	vec3 normal = normalize(fragNormal);
	vec3 vDir = normalize(renderCameraPosition - fragPos);

	//Point Light, specular and diffuse
	vec3 phonglightAccumulator = vec3(0.0,0.0,0.0), phonglight, lightDir;
	float distanceToLight, attenuateFactor;
	for(int i = 0; i < min(numberOfLights,MAX_NUMBER_POINT_LIGHTS); i++){
		lightDir = lightPositions[i] - fragPos;
		distanceToLight = length(lightDir);
		if(distanceToLight < lightRadius[i]){
			attenuateFactor = attenuate(lightAttenuationConstaints[i], distanceToLight);
			lightDir = normalize(lightDir);
			phonglight = cellShader(attenuateFactor * diffuse(lightColours[i], lightDir, normal, uniformPhongConstaints[0]), CELL_LAYER_COUNT_DIFFUSE);
			phonglight += cellShader(attenuateFactor * specular(lightColours[i], lightDir, normal, vDir, uniformPhongConstaints[1], uniformPhongConstaints[2]), CELL_LAYER_COUNT_SPECULAR);
			phonglightAccumulator += phonglight;
		}
	}
	float cosAngle;
	//Spot Light, specular and diffuse
	for(int i = 0; i < min(numberOfSpotLight, MAX_NUMBER_SPOT_LIGHTS); i++){
		lightDir = spotLightPositions[i] - fragPos;
		distanceToLight = length(lightDir);
		lightDir = normalize(lightDir);
		cosAngle = dot(-lightDir,spotLightDirections[i]);
		if( (distanceToLight < spotLightRadius[i]) && (cosAngle > spotLightCosOuterAngles[i])){
			attenuateFactor = attenuate(spotLightAttenuationConstaints[i], distanceToLight);
			phonglight = cellShader(attenuateFactor * diffuse(spotLightColours[i], lightDir, normal, uniformPhongConstaints[0]), CELL_LAYER_COUNT_DIFFUSE);
			phonglight += cellShader(attenuateFactor * specular(spotLightColours[i], lightDir, normal, vDir, uniformPhongConstaints[1], uniformPhongConstaints[2]), CELL_LAYER_COUNT_SPECULAR);
			phonglightAccumulator += spotLightEdgeFactor(spotLightCosOuterAngles[i], spotLightCosInnerAngles[i], cosAngle) * phonglight;
		}
	}
	//Directional Light, specular and diffuse + shadow mapping
	phonglight =  cellShader(diffuse(directionalLightColour, -directionalLightDirection, normal, uniformPhongConstaints[0]),CELL_LAYER_COUNT_DIFFUSE);
	phonglight += cellShader(specular(directionalLightColour, -directionalLightDirection, normal, vDir, uniformPhongConstaints[1], uniformPhongConstaints[2]), CELL_LAYER_COUNT_SPECULAR);
	if(useShadowMap == 1){
		phonglightAccumulator += (1.0 - directionalLightShadow(FragPosLightSpace, max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005))) * phonglight;
	}else{
		phonglightAccumulator += phonglight;
	}
	//Ambient Light
	phonglightAccumulator += uniformPhongConstaints[3] * ambientLight;
	//Final Result
	color = baseColour * vec4(phonglightAccumulator,1.0);
}
