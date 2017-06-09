// Basic texture shader - colors with either a texture or solid color
#version 330 core
// Inputs
in vec2 texCoords;
in vec3 origPosition;

// Uniform variables
const int MAXLIGHTS = 8;
const float SHADOW_BIAS = 0.05f;
uniform sampler2D lightMaps[MAXLIGHTS];
uniform int numLights;
uniform mat4 lightProjects[MAXLIGHTS];
uniform mat4 lightViews[MAXLIGHTS];

uniform int isTexturing;
uniform sampler2D sampler;
uniform vec3 color;

// Outputs
out vec4 fragColor;

float shadowFactor(vec3 pos){
	float result = 0.0;
	for(int i = 0; i < numLights; i++){
		vec4 lightRelativePos = lightProjects[i] * lightViews[i] * vec4(pos, 1.0);
		vec3 hLightRelativePos = lightRelativePos.xyz / lightRelativePos.w;
		hLightRelativePos = hLightRelativePos * 0.5 + 0.5;
		float lightDepth = texture(lightMaps[i], hLightRelativePos.xy).r;
		float projectionDepth = hLightRelativePos.z;
		if((projectionDepth - SHADOW_BIAS) > lightDepth)
			result += (1.0 / numLights);
	}
	return 1.0 - result;
}

void main(){
	if(isTexturing > 0){
		fragColor = vec4(vec3(shadowFactor(origPosition) * texture(sampler, texCoords)), 1.0);
	}
	else{
		fragColor = vec4(shadowFactor(origPosition) * color, 1.0);
	}
}