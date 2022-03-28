#version 330 core

//Fragment Input
in vec2 fragTextureCoordinate;

//Material Properties of Mesh
uniform vec3 uniformPhongAmbient;
uniform sampler2D colourTexture;
uniform sampler2D depthTexture;
uniform vec3 translucentIntensity;

//Outputs
layout (location = 0) out vec4 col;

uniform vec2 pixelSize;

  
void main() {
	vec2 screenCoord = gl_FragCoord.xy * pixelSize;//vec2(gl_FragCoord.x / 1200.0, gl_FragCoord.y / 800.0);
	if( texture(depthTexture, screenCoord).r > gl_FragCoord.z)
		col = vec4(uniformPhongAmbient,1.0) * vec4(translucentIntensity,1.0) * texture(colourTexture, fragTextureCoordinate);//vec4(texture(colourTexture, fragTextureCoordinate).rgb, 1.0 - fragTextureCoordinate.x);
	else
		col = vec4( screenCoord, 0.0,0.0);
}
