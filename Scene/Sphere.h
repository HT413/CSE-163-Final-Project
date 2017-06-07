#ifndef _SPHERE_GEOMETRY_
#define _SPHERE_GEOMETRY_

#include "Globals.h"
#include "Materials.h"

class Sphere{
private:
	GLuint VAO, VBO, NBO;
	mat4 model;
	vec3 color;
	Material *mat;
	int stks, slcs;

public:
	Sphere(int, int);
	~Sphere();
	void draw(GLuint);
	void setModel(mat4 m){ model = m; }
	void setMaterial(Material* m){ mat = m; }
};

#endif