// Fragment shader for Ashikhmin direct lighting
#version 410

// Definitions if any
const float M_PI = 3.14159265359f;

// Inputs
in vec3 position;
in vec3 normal;
in vec3 origPosition;

// Uniform variables
const float SHADOW_BIAS = 0.005;
uniform sampler2D lightMap[8];
uniform mat4 lightProject[8];
uniform mat4 lightView[8];

uniform int isShadowMap;
uniform int numLights;
uniform vec3 camPos;
uniform vec3 lightCol[8];
uniform vec4 lightPos[8];
uniform samplerCube skybox;

uniform float n_u;
uniform float n_v;
uniform float rs;
uniform float rd;
uniform vec3 diffuse;
uniform vec3 specular;

uniform mat4 model;

// Outputs
out vec4 fragColor;

// Perlin noise hash to generate a "pseudorandom" number
float rand(vec2 co)
{
   return fract(sin(dot(co.xy,vec2(12.9898,78.233))) * 43758.5453);
}

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
	vec3 k1 = normalize(camPos - position);
	vec3 output = vec3(0, 0, 0);
	vec3 uVec = normalize(cross(vec3(0, 1, 0), normal));
	if(length(uVec) < 0.0001) 
		uVec = normalize(cross(vec3(1, 0, 0), normal));
	vec3 vVec = normalize(cross(normal, uVec));
	// Do a hashing of the normal to generate consistent pseudorandom numbers
	float zeta1 = rand(normal.xy);
	float zeta2 = rand(normal.zx);
	float phi = atan(sqrt((n_u + 1.0) / (n_v + 1.0)) * tan(M_PI * zeta1 / 2.0));
	int quadrant = int(rand(normal.yz) * 4.0);
	if(quadrant == 1) phi = M_PI - phi;
	else if(quadrant == 2) phi = M_PI + phi;
	else if(quadrant == 3) phi = 2.f * M_PI - phi;
	float cosTheta = pow((1.0 - zeta2), 1.0 / 
		(1.0 + (n_u * pow(cos(phi), 2)) + (n_v * pow(sin(phi), 2))));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	vec3 hVec = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
	hVec = normalize(hVec.y * uVec + hVec.z * normal + hVec.x * vVec);
	
	// Calculate direct lighting
	vec3 k2;
	for(int i = 0; i < numLights; i++){
		if(lightPos[i].w < 0.001){
			k2 = normalize(vec3(lightPos[i]));
		} else{
			k2 = normalize(vec3(lightPos[i]) - position);
		}
		vec3 halfVec = normalize(k1 + k2);
		float fresnel = rs + (1.0 - rs) * pow(1.0 - dot(k1, halfVec), 5);
		float spec = (sqrt((n_u + 1.0) * (n_v + 1.0)) / (8.0)) * fresnel *
			pow(dot(normal, halfVec), ((n_u * pow(dot(halfVec, uVec), 2) + n_v *
			pow(dot(halfVec, vVec), 2)) / (1.0 - pow(dot(halfVec, normal), 2)))) /
			(dot(halfVec, k1) * max(dot(normal, k1), dot(normal, k2)));
		float diff = ((28.0 * rd) / (23.0)) * rd *		(1.0 - pow(1.0 - dot(normal, k1) / 2.f, 5)) *		(1.0 - pow(1.0 - dot(normal, k2) / 2.f, 5));
		// Clamp just in case
		if(diff < 0) diff = 0.0; if(diff > 1) diff = 1.0;
		if(spec < 0) spec = 0.0; if(spec > 1) spec = 1.0;
		float bias = max(10 * SHADOW_BIAS * (1.0 - dot(normal, k2)), SHADOW_BIAS);
		if(isShadowMap > 0){
			output += shadowFactor(origPosition, bias) * (diff * diffuse * lightCol[i] + spec * specular * lightCol[i]);
		} else{
			output += (diff * diffuse * lightCol[i] + spec * specular * lightCol[i]);
		}
	}

	// Now do an estimate of indirect lighting
	vec3 outgoing = reflect(k1, hVec);
	if(dot(outgoing, normal) < 0) 
		outgoing = -outgoing;
	// Do a small shift on the output vector to move it with the object
	outgoing = normalize(vec3(model * vec4(outgoing, 1.0)));
	//output += rs * specular * vec3(texture(skybox, outgoing));
	// Clamp the resulting color just in case
	if(output.x > 1.0) output.x = 1.0;
	if(output.y > 1.0) output.y = 1.0;
	if(output.z > 1.0) output.z = 1.0;
	fragColor = vec4(output, 1.0);
}