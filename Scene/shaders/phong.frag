// Fragment shader for Phong shading
#version 410

// Definitions if any

// Inputs
in vec3 position;
in vec3 normal;
in vec3 origPosition;

// Uniform variables
const float SHADOW_BIAS = 0.005;
uniform int numLights;
uniform sampler2D lightMap[8];
uniform mat4 lightProject[8];
uniform mat4 lightView[8];

uniform vec3 camPos;
uniform vec3 lightCol[8];
uniform vec4 lightPos[8];

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

// Outputs
out vec4 fragColor;

float shadowFactor(vec3 pos, float bias){
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
				if((projectionDepth - bias) > lightDepth){
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
	vec3 output = ambient;
	vec3 eyeDir = camPos - position;

	vec3 lightDir;
	for(int i = 0; i < numLights; i++){
		if(lightPos[i].w < 0.01){ // Directional light
			lightDir = normalize((lightPos[i]).xyz);
		}
		else{ // Point light
			lightDir = lightPos[i].xyz - position;
		}
		vec3 halfVec = normalize(eyeDir + lightDir);
		float bias = max(10 * SHADOW_BIAS * (1.0 - dot(normal, lightDir)), SHADOW_BIAS);
		// Diffuse component
		output += max(dot(normal, lightDir), 0.0) * shadowFactor(origPosition, bias) * diffuse * lightCol[i];
		// Specular component
		output += pow(max(dot(normal, halfVec), 0.0), shininess) * shadowFactor(origPosition, bias) * specular * lightCol[i];
	}

	fragColor = vec4(output, 1.0);
}