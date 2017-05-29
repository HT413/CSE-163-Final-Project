// Fragment shader for Ashikhmin direct lighting
#version 410

// Definitions if any
const float M_PI = 3.14159265359f;
const int maxLights = 8;

// Inputs
in vec3 position;
in vec3 normal;

// Uniform variables
uniform int numLights;
uniform vec3 camPos;
uniform vec3 lightCols[maxLights];
uniform vec4 lights[maxLights];

uniform float n_u;
uniform float n_v;
uniform float rs;
uniform float rd;
uniform vec3 diffuse;
uniform vec3 specular;

// Outputs
out vec4 fragColor;

void main(){
	vec3 k1 = normalize(camPos - position);
	vec3 output = vec3(0, 0, 0);
	vec3 uVec = normalize(cross(vec3(0, 1, 0), normal));
	if(length(uVec) < 0.0001) 
		uVec = normalize(cross(vec3(1, 0, 0), normal));
	vec3 vVec = normalize(cross(normal, uVec));
	for(int i = 0; i < numLights; i++){
		vec4 lightPos = lights[i];
		vec3 lightCol = lightCols[i];
		vec3 k2;
		if(lightPos.w < 0.001){
			k2 = normalize(vec3(-lightPos));
		} else{
			k2 = normalize(vec3(lightPos) - position);
		}
		vec3 halfVec = normalize(k1 + k2);
		float fresnel = rs + (1.0 - rs) * pow(1.0 - dot(k1, halfVec), 5);
		float spec = (sqrt((n_u + 1.0) * (n_v + 1.0)) / (8.0 * M_PI)) * fresnel *
			pow(dot(normal, halfVec), ((n_u * pow(dot(halfVec, uVec), 2) + n_v *
			pow(dot(halfVec, vVec), 2)) / (1.0 - pow(dot(halfVec, normal), 2)))) /
			(dot(halfVec, k1) * max(dot(normal, k1), dot(normal, k2)));
		float diff = ((28.0 * rd) / (23.0 * M_PI)) * (1.0 - rs) *		(1.0 - pow(1.0 - dot(normal, k1) / 2.f, 5)) *		(1.0 - pow(1.0 - dot(normal, k2) / 2.f, 5));
		output += diff * diffuse * lightCol + spec * specular * lightCol;
	}
	fragColor = vec4(output, 1.0);
}