// Vertex shader for textured object
#version 330 core

// Inputs
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texcoord;

// Uniform variables
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Outputs
out vec2 texCoords;

void main(){
	gl_Position = projection * view * model * vec4(pos, 1.0);
	texCoords = texcoord;
}