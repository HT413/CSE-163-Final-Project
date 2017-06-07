// Fragment shader for the depth map
#version 330 core
// Inputs
in float depth;
// Outputs
out vec4 fragColor;

void main(){
	fragColor = vec4(depth, depth, depth, 1.0);
}