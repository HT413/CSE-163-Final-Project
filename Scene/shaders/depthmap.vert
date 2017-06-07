// Vertex shader for the depth map
#version 330 core
// Inputs
layout(location = 0) in vec3 position;
// Uniform variables
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
// Outputs
out float depth;

void main(){
	gl_Position = projection * model * view * vec4(position, 1.0);
	depth = gl_Position.z;
}