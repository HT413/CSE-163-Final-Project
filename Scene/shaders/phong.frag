// Fragment shader for Phong shading
#version 410

// Definitions if any
const int maxLights = 8;

// Inputs
in vec3 position;
in vec3 normal;

// Uniform variables
uniform int numLights;
uniform vec3 camPos;
uniform vec3 lightCols[maxLights];
uniform vec4 lights[maxLights];

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

// Outputs
out vec4 fragColor;

void main(){
	vec3 output = ambient;
	vec3 eyeDir = camPos - position;
	for(int i = 0; i < numLights; i++){
		vec4 lightPos = lights[i];
		vec3 lightCol = lightCols[i];
		vec3 lightDir;
		if(lightPos.w < 0.0001){ // Directional light
			lightDir = normalize((-lightPos).xyz);
		}
		else{ // Point light
			lightDir = lightPos.xyz - position;
		}
		vec3 halfVec = normalize(eyeDir + lightDir);
		// Diffuse component
		output += (diffuse * lightCol * max(dot(normal, lightDir), 0.0));
		// Specular component
		output += (specular * lightCol * pow(max(dot(normal, halfVec), 0.0), shininess));
	}
	fragColor = vec4(output, 1.0);
}