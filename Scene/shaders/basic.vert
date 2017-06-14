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
out vec3 origPosition;

void main(){
	gl_Position = projection * view * model * vec4(pos, 1.0);
	vec4 posn = model * vec4(pos, 1.0);
	position = posn.xyz / posn.w;
	normal = normalize(transpose(inverse(mat3(model))) * norm);
	origPosition = vec3(model * vec4(pos, 1.0));
}