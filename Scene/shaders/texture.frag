// Basic texture shader - colors with either a texture or solid color
#version 330 core
// Inputs
in vec2 texCoords;
in vec3 origPosition;

// Uniform variables
const float SHADOW_BIAS = 0.005;
uniform int numLights;
uniform sampler2D lightMap[8];
uniform mat4 lightProject[8];
uniform mat4 lightView[8];

uniform int isTexturing;
uniform int isShadowMap;
uniform vec3 color;

// Outputs
out vec4 fragColor;

float shadowFactor(vec3 pos){
	float result = 0.0;
	for(int i = 0; i < numLights; i++){
		vec4 lightRelativePos = lightProject[i] * lightView[i] * vec4(pos, 1.0);
		vec3 hLightRelativePos = lightRelativePos.xyz / lightRelativePos.w;
		hLightRelativePos = hLightRelativePos * 0.5 + 0.5;
		vec2 texelSize = 1.0 / textureSize(lightMap[i], 0);
		for(int dx = -1; dx <= 1; dx++){
			for(int dy = -1; dy <= 1; dy++){
				float lightDepth = texture(lightMap[i], hLightRelativePos.xy + vec2(dx, dy) * texelSize).r;
				float projectionDepth = hLightRelativePos.z;
				if(projectionDepth > 1.0) return 1.0;
				if((projectionDepth - SHADOW_BIAS) > lightDepth){
					if(dx == 0 && dy == 0)
						result += 9.0 / numLights;
					else if(dx == 0 || dy == 0)
						result += 3.0 / numLights;
					else
						result += 1.0 / numLights;
				}
			}
		}
	}
	return 1.0 - result / 25.0;
}

void main(){
	if(isTexturing > 0){
		fragColor = texture(lightMap[1], texCoords);
	}
	else{
		if(isShadowMap > 0){
			fragColor = vec4(shadowFactor(origPosition) * color, 1.0);
		} else{
			fragColor = vec4(color, 1.0);
		}
	}
}