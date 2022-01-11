#version 330 core

in vec3 fragPos;
in vec3 fragColour;
in vec3 fragNormal;
in vec2 fragTextureCoordinate;

uniform int useColour;
uniform sampler2D colourText;

uniform vec3 lightPositions[10];
uniform vec3 lightColours[10];
uniform vec3 lightAttenuationConstaints[10];
uniform vec3 ambientLight;
uniform int numberOfLights;

uniform vec4 uniformPhongConstaints;//  (K-Diff, K-Spec, Alpha, K-Amb)
uniform vec3 renderCameraPosition;

out vec4 color;

vec3 attenuate(vec3 col, vec3 constaints, float dis){
	return col / max(constaints[0] + constaints[1]*dis + constaints[1]*dis*dis,0.0001);
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

void main() {
	vec4 baseColour;
	if(useColour != 0){ baseColour = vec4(fragColour, 1.0);}
	else{ baseColour = texture(colourText,fragTextureCoordinate);}

	vec3 normal = normalize(fragNormal);
	vec3 vDir = normalize(renderCameraPosition - fragPos);

	vec3 phonglightAccumulator = vec3(0.0,0.0,0.0), phonglight = vec3(0.0,0.0,0.0);
	vec3 lightDir;
	float distanceToLight;
	for(int i = 0; i < min(numberOfLights,10); i++){
		lightDir = lightPositions[i] - fragPos;
		distanceToLight = length(lightDir);
		lightDir = normalize(lightDir);
		phonglight = diffuse(lightColours[i], lightDir, normal, uniformPhongConstaints[0]);
		phonglight += specular(lightColours[i], lightDir, normal, vDir, uniformPhongConstaints[1], uniformPhongConstaints[2]);
		phonglight = attenuate(phonglight, lightAttenuationConstaints[i], distanceToLight);
		phonglightAccumulator += phonglight;
	}
	phonglightAccumulator += uniformPhongConstaints[3] * ambientLight;
	color = baseColour*vec4(phonglightAccumulator,1.0);
}
