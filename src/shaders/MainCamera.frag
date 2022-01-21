#version 330 core

in vec3 fragPos;
in vec3 fragColour;
in vec3 fragNormal;
in vec2 fragTextureCoordinate;
in vec3 FragPosLightSpace;

uniform int useColour;
uniform sampler2D colourText;
uniform sampler2D shadowMap;

uniform vec3 lightPositions[10];
uniform vec3 lightColours[10];
uniform vec3 lightAttenuationConstaints[10];
uniform vec3 ambientLight;
uniform int numberOfLights;

uniform vec3 directionalLightDirection;
uniform vec3 directionalLightColour;

uniform vec4 uniformPhongConstaints;//  (K-Diff, K-Spec, Alpha, K-Amb)
uniform vec3 renderCameraPosition;

out vec4 color;

#define CELL_LAYER_COUNT_DIFFUSE 0
#define CELL_LAYER_COUNT_SPECULAR CELL_LAYER_COUNT_DIFFUSE + 0


float attenuate(vec3 constaints, float dis){
	return 1 / max(constaints[0] + constaints[1]*dis + constaints[1]*dis*dis,0.0001);
}

vec3 diffuse(vec3 col, vec3 lightDir, vec3 normal, float k){
	return k  *max( dot( lightDir, normal), 0.0) * col;
}

vec3 specular(vec3 col, vec3 lightDir, vec3 normal, vec3 vDir, float k, float a){
	float d = dot( lightDir, normal);
	float d2 = dot( 2 * d * normal - lightDir, vDir);
	if(d < 0.0 || d2 < 0.0) return 0.0 * col;
	return k * pow(d2,a) * col;
}
float directionalLightShadow(vec3 fragPositionInLight, float bias){

    // transform to [0,1] range
    vec3 projCoords = fragPositionInLight * 0.5 + 0.5;
	if(projCoords.z > 1.0) return 0.0;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
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
	vec4 baseColour;
	if(useColour != 0){ baseColour = vec4(fragColour, 1.0);}
	//else{ baseColour = texture(colourText,fragTextureCoordinate);}

	vec3 normal = normalize(fragNormal);
	vec3 vDir = normalize(renderCameraPosition - fragPos);

	//Point Light, specular and diffuse
	vec3 phonglightAccumulator = vec3(0.0,0.0,0.0), phonglight = vec3(0.0,0.0,0.0);
	vec3 lightDir;
	float distanceToLight, attenuateFactor;
	for(int i = 0; i < min(numberOfLights,10); i++){
		lightDir = lightPositions[i] - fragPos;
		distanceToLight = length(lightDir);
		attenuateFactor = attenuate(lightAttenuationConstaints[i], distanceToLight);
		lightDir = normalize(lightDir);
		phonglight = cellShader(attenuateFactor * diffuse(lightColours[i], lightDir, normal, uniformPhongConstaints[0]), CELL_LAYER_COUNT_DIFFUSE);
		phonglight += cellShader(attenuateFactor * specular(lightColours[i], lightDir, normal, vDir, uniformPhongConstaints[1], uniformPhongConstaints[2]), CELL_LAYER_COUNT_SPECULAR);
		phonglightAccumulator += phonglight;
	}
	//Directional Light

	phonglight =  cellShader(diffuse(directionalLightColour, -directionalLightDirection, normal, uniformPhongConstaints[0]),CELL_LAYER_COUNT_DIFFUSE);
	phonglight += cellShader(specular(directionalLightColour, -directionalLightDirection, normal, vDir, uniformPhongConstaints[1], uniformPhongConstaints[2]), CELL_LAYER_COUNT_SPECULAR);
	phonglightAccumulator += (1.0 - directionalLightShadow(FragPosLightSpace,max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005))) * phonglight;
	//cell shader
	//phonglightAccumulator = cellShader(phonglightAccumulator,0);//To turn on the cell shading, this needs to be given >1 band number
	//Ambient Light
	phonglightAccumulator += uniformPhongConstaints[3] * ambientLight;

	

	color = baseColour*vec4(phonglightAccumulator,1.0);
}
