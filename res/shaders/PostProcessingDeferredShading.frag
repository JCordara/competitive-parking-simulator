#version 330 core
out vec4 color;
in vec2 fragUv2;

// All textures to read from for defered shading
uniform sampler2D textureColour;
uniform sampler2D textureClassification;
uniform sampler2D texturePosition;
uniform sampler2D textureNormal;
uniform sampler2D textureDiffuseConstant;
uniform sampler2D textureSpecularAndShinnyConstant;
uniform sampler2D textureAmbientConstant;
uniform sampler2D textureDepth;
	//Depth test texture of directional light Shading
uniform sampler2D textureDirectionalLightDepth;
uniform sampler2D textureDirectionalLightPosition;
	//Point Lighting Uniforms
#define MAX_NUMBER_POINT_LIGHTS 50
uniform vec3 pointLightPositions[MAX_NUMBER_POINT_LIGHTS];
uniform vec3 pointLightColours[MAX_NUMBER_POINT_LIGHTS];
uniform vec3 pointLightAttenuationConstaints[MAX_NUMBER_POINT_LIGHTS];
uniform float pointLightRadius[MAX_NUMBER_POINT_LIGHTS];
uniform int numberOfPointLights;
	//Spot Lighting Uniforms
#define MAX_NUMBER_SPOT_LIGHTS 70
uniform vec3 spotLightPositions[MAX_NUMBER_SPOT_LIGHTS];
uniform vec3 spotLightColours[MAX_NUMBER_SPOT_LIGHTS];
uniform vec3 spotLightAttenuationConstaints[MAX_NUMBER_SPOT_LIGHTS];
uniform float spotLightRadius[MAX_NUMBER_SPOT_LIGHTS];
uniform vec3 spotLightDirections[MAX_NUMBER_SPOT_LIGHTS];
uniform vec2 spotLightCosAngles[MAX_NUMBER_SPOT_LIGHTS];
uniform int numberOfSpotLights;
	//Directional Lighting Uniforms
uniform vec3 directionalLightDirection;
#define MAX_BIAS 0.005
#define MIN_BIAS 0.0005
uniform vec3 directionalLightColour;
//uniform vec2 directionalLightPixelSize;
	//Ambient Lighting Uniforms
uniform vec3 ambientLightColour;
	//Camera Uniforms
uniform vec3 cameraPosition;
uniform vec2 clipDistance;

const mat3 sx = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
const mat3 sy = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);

float attenuate(vec3 constaints, float dis){
	return 1 / max(constaints[0] + constaints[1]*dis + constaints[1]*dis*dis,0.0001);
}

vec3 diffuse(vec3 lightDir, vec3 normal, vec3 k){
	return k * max( dot( lightDir, normal), 0.0);
}

vec3 specular(vec3 lightDir, vec3 normal, vec3 vDir, vec3 k, float a){
	if(a < 0.0000001) return vec3(0.0); 
	return k * pow(max(dot(normal, normalize(lightDir + vDir)), 0.0), a);
}

float spotLightEdgeFactor(float outer, float inner, float value){
	return max(min( (value - outer)/(inner - outer), 1.0), 0.0);
}

float edgeDetection(){
    mat3 I;
	vec3 samples;
    for (int i=0; i<3; i++) for (int j=0; j<3; j++) {
		samples  = texelFetch(textureClassification, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
        I[i][j] = length(samples); 
	}
	float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
	float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);
	return sqrt(gx*gx + gy*gy);
}

float directionalLightShadow(vec3 fragPositionInLight, float bias){
    vec3 projCoords = fragPositionInLight * 0.5 + 0.5;
	if(projCoords.z < 0.0 || projCoords.x < 0.0 || projCoords.y < 0.0
	||projCoords.z > 1.0 || projCoords.x > 1.0 || projCoords.y > 1.0)
		return 0.0;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(textureDirectionalLightDepth, 0);
	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
			float pcfDepth = texture(textureDirectionalLightDepth, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
    return shadow * 0.1111111; //(/9)
}

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return 2.0 * clipDistance[0] * clipDistance[1] / (clipDistance[1] + clipDistance[0] - z * (clipDistance[1] - clipDistance[0]));	
}

#define TEST 0

void main() {
#if (TEST == 1)
	color = texture(textureNormal, fragUv2).rgba;
#else
	// All textures to read from for defered shading
	color = texture(textureColour, fragUv2);//Initial Data
	
	//Lighting Calculations
	vec3 pos = texture(texturePosition, fragUv2).rgb;
	vec3 norm = texture(textureNormal, fragUv2).rgb;
	vec3 diff = texture(textureDiffuseConstant, fragUv2).rgb;
	vec4 specShinny = texture(textureSpecularAndShinnyConstant, fragUv2);
	vec3 ambient = texture(textureAmbientConstant, fragUv2).rgb;
	vec3 directionalLightSpaceFragPos = texture(textureDirectionalLightPosition, fragUv2).rgb;

	vec3 vDir = normalize(cameraPosition - pos);
	//Point Light, specular and diffuse
	vec3 phonglightAccumulator = vec3(0.0,0.0,0.0), lightDir, diffuseFactor, specularFactor;
	float distanceToLight, attenuateFactor;
	for(int i = 0; i < min(numberOfPointLights ,MAX_NUMBER_POINT_LIGHTS); i++){
		lightDir = pointLightPositions[i] - pos;
		distanceToLight = length(lightDir);
		if(distanceToLight < pointLightRadius[i]){
			attenuateFactor = attenuate(pointLightAttenuationConstaints[i], distanceToLight);
			lightDir = normalize(lightDir);
			diffuseFactor = diffuse(lightDir, norm, diff);
			specularFactor = specular(lightDir, norm, vDir, specShinny.rgb, specShinny.a);
			phonglightAccumulator += attenuateFactor * (diffuseFactor + specularFactor) * pointLightColours[i];
		}
	}
	
	//Spot Light, specular and diffuse
	float cosAngle, spotlightFactor;
	for(int i = 0; i < min(numberOfSpotLights, MAX_NUMBER_SPOT_LIGHTS); i++){
		lightDir = spotLightPositions[i] - pos;
		distanceToLight = length(lightDir);
		lightDir = normalize(lightDir);
		cosAngle = dot(-lightDir, spotLightDirections[i]);
		if( (distanceToLight < spotLightRadius[i]) && (cosAngle > spotLightCosAngles[i][1])){
			attenuateFactor = attenuate(spotLightAttenuationConstaints[i], distanceToLight);
			spotlightFactor = spotLightEdgeFactor(spotLightCosAngles[i][1], spotLightCosAngles[i][0], cosAngle);
			diffuseFactor =  diffuse(lightDir, norm, diff);
			specularFactor = specular(lightDir, norm, vDir, specShinny.rgb, specShinny.a);
			phonglightAccumulator += spotlightFactor * attenuateFactor * (diffuseFactor + specularFactor) * spotLightColours[i];
		}
	}
	
	//Directional Light, specular and diffuse + shadow mapping
	float shadowfactor;
	diffuseFactor = diffuse(-directionalLightDirection, norm, diff);
	specularFactor = specular(-directionalLightDirection, norm, vDir, specShinny.rgb, specShinny.a);
	shadowfactor = (1.0 - directionalLightShadow(directionalLightSpaceFragPos, max(MAX_BIAS * (1.0 - dot(norm, -directionalLightDirection)), MIN_BIAS)));
	phonglightAccumulator +=  (diffuseFactor + specularFactor) * shadowfactor * directionalLightColour;
	//Ambient Light
	phonglightAccumulator += ambient * ambientLightColour;
	//Final Result
	color = color * vec4(phonglightAccumulator,1.0);//Lighting
	float edv = edgeDetection();
	color = (1.0 - edv) * color + edv * vec4(0.0,0.0,0.0,1.0);//Edge Detection
	//Depth Fade
	float depthfactor =  min(max(2.0*LinearizeDepth(texture(textureDepth,fragUv2).r) / clipDistance[1] - 0.98, 0.0), 1.0);
	float a = depthfactor * depthfactor;
	//night fog 0.025,0.025,0.064
	// fog 0.52, 0.80, 0.92
	color = (1.0 - a) * color + a * vec4(0.025,0.025,0.064,1.0);
#endif
	color[3] = 1.0; //Alpha Correction
} 
