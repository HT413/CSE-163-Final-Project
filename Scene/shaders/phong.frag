// Fragment shader for Phong shading
#version 410

// Definitions if any
const int maxLights = 8;

// Inputs
in vec3 position;
in vec3 normal;
in vec3 origPosition;

// Uniform variables
const int MAXLIGHTS = 8;
const float SHADOW_BIAS = 0.05f;
uniform sampler2D lightMaps[MAXLIGHTS];
uniform mat4 lightProjects[MAXLIGHTS];
uniform mat4 lightViews[MAXLIGHTS];

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

float shadowFactor(vec3 pos, float bias){
	float result = 0.0;
	for(int i = 0; i < numLights; i++){
		vec4 lightRelativePos = lightProjects[i] * lightViews[i] * vec4(pos, 1.0);
		vec3 hLightRelativePos = lightRelativePos.xyz / lightRelativePos.w;
		hLightRelativePos = hLightRelativePos * 0.5 + 0.5;
		float lightDepth = texture(lightMaps[i], hLightRelativePos.xy).r;
		float projectionDepth = hLightRelativePos.z;
		if((projectionDepth - bias) > lightDepth)
			result += (1.0 / numLights);
	}
	return 1.0 - result;
}

void main(){
	vec3 output = ambient;
	vec3 eyeDir = camPos - position;
	for(int i = 0; i < numLights; i++){
		vec4 lightPos = lights[i];
		vec3 lightCol = lightCols[i];
		vec3 lightDir;
		if(lightPos.w < 0.0001){ // Directional light
			lightDir = normalize((lightPos).xyz);
		}
		else{ // Point light
			lightDir = lightPos.xyz - position;
		}
		vec3 halfVec = normalize(eyeDir + lightDir);
		float bias = max(10 * SHADOW_BIAS * (1.0 - dot(normal, lightDir)), SHADOW_BIAS);
		// Diffuse component
		output += shadowFactor(origPosition, bias) * (diffuse * lightCol * max(dot(normal, lightDir), 0.0));
		// Specular component
		output += shadowFactor(origPosition, bias) * (specular * lightCol * pow(max(dot(normal, halfVec), 0.0), shininess));
	}
	fragColor = vec4(output, 1.0);
}