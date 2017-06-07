#include "Sphere.h"

Sphere::Sphere(int stacks, int slices)
{
	vector<vec3> sphereVertices;
	vector<vec3> sphereNormals;
	model = mat4(1.f);

	float fstacks = (float)stacks;
	float fslices = (float)slices;
	for(int i = 0; i < slices; i++) {
		for(int j = 0; j < stacks; j++) {
			// Bottom right
			sphereVertices.push_back(vec3(
				-cosf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * j / fslices),
				-cosf(PI * j / fslices),
				sinf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * j / fslices)));
			sphereNormals.push_back(normalize(vec3(
				-cosf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * j / fslices),
				-cosf(PI * j / fslices),
				sinf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * j / fslices))));
			// Top right
			sphereVertices.push_back(vec3(
				-cosf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * (j + 1.0) / fslices),
				-cosf(PI * (j + 1.0) / fslices),
				sinf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * (j + 1.0) / fslices)));
			sphereNormals.push_back(normalize(vec3(
				-cosf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * (j + 1.0) / fslices),
				-cosf(PI * (j + 1.0) / fslices),
				sinf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * (j + 1.0) / fslices))));
			// Top left
			sphereVertices.push_back(vec3(
				-cosf(2.0f * PI * i / fstacks) * sinf(PI * (j + 1.0f) / fslices),
				-cosf(PI * (j + 1.0f) / fslices),
				sinf(2.0f * PI * i / fstacks) * sinf(PI * (j + 1.0f) / fslices)));
			sphereNormals.push_back(normalize(vec3(
				-cosf(2.0f * PI * i / fstacks) * sinf(PI * (j + 1.0f) / fslices),
				-cosf(PI * (j + 1.0f) / fslices),
				sinf(2.0f * PI * i / fstacks) * sinf(PI * (j + 1.0f) / fslices))));

			// Bottom left
			sphereVertices.push_back(vec3(
				-cosf(2.0f * PI * i / fstacks) * sinf(PI * j / fslices),
				-cosf(PI * j / fslices),
				sinf(2.0f * PI * i / fstacks) * sinf(PI * j / fslices)));
			sphereNormals.push_back(normalize(vec3(
				-cosf(2.0f * PI * i / fstacks) * sinf(PI * j / fslices),
				-cosf(PI * j / fslices),
				sinf(2.0f * PI * i / fstacks) * sinf(PI * j / fslices))));
			// Bottom right
			sphereVertices.push_back(vec3(
				-cosf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * j / fslices),
				-cosf(PI * j / fslices),
				sinf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * j / fslices)));
			sphereNormals.push_back(normalize(vec3(
				-cosf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * j / fslices),
				-cosf(PI * j / fslices),
				sinf(2.0f * PI * (i + 1.0) / fstacks) * sinf(PI * j / fslices))));
			// Top left
			sphereVertices.push_back(vec3(
				-cosf(2.0f * PI * i / fstacks) * sinf(PI * (j + 1.0f) / fslices),
				-cosf(PI * (j + 1.0f) / fslices),
				sinf(2.0f * PI * i / fstacks) * sinf(PI * (j + 1.0f) / fslices)));
			sphereNormals.push_back(normalize(vec3(
				-cosf(2.0f * PI * i / fstacks) * sinf(PI * (j + 1.0f) / fslices),
				-cosf(PI * (j + 1.0f) / fslices),
				sinf(2.0f * PI * i / fstacks) * sinf(PI * (j + 1.0f) / fslices))));
		}
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * sphereNormals.size(), &sphereNormals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	stks = stacks;
	slcs = slices;
}

Sphere::~Sphere()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
}

void Sphere::draw(GLuint shaderProgram){
	if(usingPhong)
		mat->bindMaterial();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &(model[0][0]));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, stks * slcs * 6);
	glBindVertexArray(0);
}