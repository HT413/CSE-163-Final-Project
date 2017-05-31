#ifndef _PLAIN_WALL_H_
#define _PLAIN_WALL_H_

// Declarations for a wall. Can be used as the ground.
#include "Globals.h"

const GLfloat corners[] ={-.5f, -.5f, 0.f, .5f, -.5f, 0.f, .5f, .5f, 0.f,
						  -.5f, -.5f, 0.f, .5f, .5f, 0.f, -.5f, .5f, 0.f};
const GLfloat texCoords[] ={0.f, 0.f, 1.f, 0.f, 1.f, 1.f,
							0.f, 0.f, 1.f, 1.f, 0.f, 1.f};

class Plane{
private:
	vec3 color;
	mat4 model;
	GLuint wallVAO, wallVBO, wallTBO, wallTexture;
	GLuint uModel, uColor, uIsTex;
	bool useTex;
	GLuint texID;

public:
	Plane(GLuint);
	~Plane();
	void draw();
	void setColor(vec3 c){ color = c; }
	void setModel(mat4 m){ model = m; }
};

#endif