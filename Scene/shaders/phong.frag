// Fragment shader for Phong shading
#version 410

// Definitions if any
const int maxLights = 8;

// Inputs
in vec3 position;
in vec3 normal;
in vec3 origPosition;

// Uniform variables
const float SHADOW_BIAS = 0.005;
uniform sampler2D lightMap;
uniform mat4 lightProject;
uniform mat4 lightView;

uniform vec3 camPos;
uniform vec3 lightCol;
uniform vec4 lightPos;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

// Outputs
out vec4 fragColor;

float shadowFactor(vec3 pos, float bias){
	float result = 0.0;
	vec4 lightRelativePos = lightProject * lightView * vec4(pos, 1.0);
	vec3 hLightRelativePos = lightRelativePos.xyz / lightRelativePos.w;
	hLightRelativePos = hLightRelativePos * 0.5 + 0.5;
	float lightDepth = texture(lightMap, hLightRelativePos.xy).b;
	float projectionDepth = hLightRelativePos.z;
	if(projectionDepth > 1.0) return 1.0;
	if((projectionDepth - bias) > lightDepth)
		result = 1.0;
	return 1.0 - result;
}

void main(){
	vec3 output = ambient;
	vec3 eyeDir = camPos - position;

	vec3 lightCol = lightCol;
	vec3 lightDir;
	if(lightPos.w < 0.01){ // Directional light
		lightDir = normalize((lightPos).xyz);
	}
	else{ // Point light
		lightDir = lightPos.xyz - position;
	}
	vec3 halfVec = normalize(eyeDir + lightDir);
	float bias = max(10 * SHADOW_BIAS * (1.0 - dot(normal, lightDir)), SHADOW_BIAS);
	// Diffuse component
	output += max(dot(normal, lightDir), 0.0) * shadowFactor(origPosition, bias) * diffuse * lightCol;
	// Specular component
	output += pow(max(dot(normal, halfVec), 0.0), shininess) * shadowFactor(origPosition, bias) * specular * lightCol;
	
	fragColor = vec4(output, 1.0);
}