#version 330 core
out vec4 color;

uniform sampler2D in_tex2;
uniform sampler2D in_tex2_no_shading;
in vec2 fragUv2;
//in vec3 C;

mat3 sx = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
mat3 sy = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);

void main() {
   vec3 diffuse = texture(in_tex2, fragUv2).rgb;
    mat3 I;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            vec3 samples  = texelFetch(in_tex2_no_shading, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            I[i][j] = length(samples); 
		}
	}

	float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
	float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

	float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));
	color = vec4(diffuse - vec3(g), 1.0);

} 

/*   vec3 diffuse = texture(in_tex2, fragUv2).rgb;
    mat3 I, J;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            vec3 samples  = texelFetch(in_tex2_no_shading, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            I[i][j] = length(samples);
			samples  = texelFetch(in_tex2, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            J[i][j] = length(samples); 
		}
	}

	float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
	float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

	float gx2 = dot(sx[0], J[0]) + dot(sx[1], J[1]) + dot(sx[2], J[2]); 
	float gy2 = dot(sy[0], J[0]) + dot(sy[1], J[1]) + dot(sy[2], J[2]);

	float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));
	float g2 = sqrt(pow(gx2, 2.0)+pow(gy2, 2.0));
	color = vec4(0.0, max(g,g2), 0.0, 1.0);*/
