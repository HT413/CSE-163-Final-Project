// Definitions for the plane class.
#include "Plane.h"

Plane::Plane(GLuint shaderProgram){	useTex = false;	uModel = glGetUniformLocation(shaderProgram, "model");	uColor = glGetUniformLocation(shaderProgram, "color");	uIsTex = glGetUniformLocation(shaderProgram, "isTexturing");	glGenVertexArrays(1, &wallVAO);
	glGenBuffers(1, &wallVBO);
	glGenBuffers(1, &wallTBO);
	glBindVertexArray(wallVAO);

	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(corners), corners, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, wallTBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Plane::~Plane(){
	glDeleteVertexArrays(1, &wallVAO);
	glDeleteBuffers(1, &wallVBO);
	glDeleteBuffers(1, &wallTBO);
}

void Plane::draw(){
	glUniformMatrix4fv(uModel, 1, GL_FALSE, &(model[0][0]));
	if(useTex){
		glUniform1i(uIsTex, 1);
	}
	else{
		glUniform1i(uIsTex, 0);
		glUniform3f(uColor, color[0], color[1], color[2]);
	}

	glBindVertexArray(wallVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}