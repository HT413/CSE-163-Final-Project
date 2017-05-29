#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include "Globals.h"
#include "Materials.h"

#include <algorithm>
#include <fstream>
#include <sstream>

class OBJObject
{
private:
	vector<unsigned int> indices;
	vector<vec3> vertices;
	vector<vec3> normals;
	GLuint VBO, NBO, VAO, EBO;
	mat4 model;
	GLuint shaderProg, uModel;
	Material *mat, *a_mat;

public:
	OBJObject(const char* filepath);
	~OBJObject();

	void parse(const char* filepath);
	void draw(GLuint shaderProgram);
	void setMaterial(Material* a, Material* b){ mat = a; a_mat = b; }
};

#endif