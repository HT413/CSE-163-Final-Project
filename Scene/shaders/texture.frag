// Basic texture shader - colors with either a texture or solid color
#version 330 core
// Inputs
in vec2 texCoords;
in vec3 origPosition;

// Uniform variables
const float SHADOW_BIAS = 0.005;
uniform sampler2D lightMap;
uniform mat4 lightProject;
uniform mat4 lightView;

uniform int isTexturing;
uniform vec3 color;

// Outputs
out vec4 fragColor;

float shadowFactor(vec3 pos){
	float result = 0.0;
	vec4 lightRelativePos = lightProject * lightView * vec4(pos, 1.0);
	vec3 hLightRelativePos = lightRelativePos.xyz / lightRelativePos.w;
	hLightRelativePos = hLightRelativePos * 0.5 + 0.5;
	float lightDepth = texture(lightMap, hLightRelativePos.xy).b;
	float projectionDepth = hLightRelativePos.z;
	if(projectionDepth > 1.0) return 1.0;
	if((projectionDepth - SHADOW_BIAS) > lightDepth)
		result = 1.0;
	return 1.0 - result;
}

void main(){
	if(isTexturing > 0){
		fragColor = texture(lightMap, texCoords);
	}
	else{
		fragColor = vec4(shadowFactor(origPosition) * color, 1.0);
	}
}