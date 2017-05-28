#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include "Globals.h"

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

public:
	OBJObject(const char* filepath);
	~OBJObject();

	void parse(const char* filepath);
	void draw(GLuint shaderProgram);
};

#endif