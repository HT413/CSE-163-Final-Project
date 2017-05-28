// Vertex shader for Phong shading
#version 330 core

// Inputs
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

// Uniform variables
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Outputs
out vec3 position;
out vec3 normal;

void main(){
	gl_Position = projection * view * model * vec4(pos, 1.0);
	position = vec3(model * vec4(pos, 1.0));
	normal = inverse(transpose(mat3(model))) * norm;
}