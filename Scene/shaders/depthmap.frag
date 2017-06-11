// Fragment shader for the depth map
#version 330 core

// Outputs
out vec4 fragColor;

void main(){
	float depth = gl_FragCoord.z;
	fragColor = vec4(depth, depth, depth, 1.0);
}