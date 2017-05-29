#include "Materials.h"

/*********************************************
REGULAR MATERIAL STRUCT DEFINITIONS
*********************************************/
// getUniformLocs func
// Gets the uniform locations of relevant variables
void RegMaterial::getUniformLocs(GLuint shaderProgram){
	uDiffuse = glGetUniformLocation(shaderProgram, "diffuse");
	uSpecular = glGetUniformLocation(shaderProgram, "specular");
	uAmbient = glGetUniformLocation(shaderProgram, "ambient");
	uShininess = glGetUniformLocation(shaderProgram, "shininess");
}

// bindMaterial func
// Sets the relevant uniform variable values
void RegMaterial::bindMaterial(){
	glUniform3f(uAmbient, ambient[0], ambient[1], ambient[2]);
	glUniform3f(uDiffuse, diffuse[0], diffuse[1], diffuse[2]);
	glUniform3f(uSpecular, specular[0], specular[1], specular[2]);
	glUniform1f(uShininess, shininess);
}

// setMaterial func
// Sets the ambient, diffuse, specular, shininess properties
void RegMaterial::setMaterial(vec3 a, vec3 d, vec3 s, float e){
	ambient = a;
	diffuse = d;
	specular = s;
	shininess = e;
}


/*********************************************
ASHIKHMIN MATERIAL STRUCT DEFINITIONS
*********************************************/
// getUniformLocs func
// Gets the uniform locations of relevant variables
void AshikhminMaterial::getUniformLocs(GLuint shaderProgram){
	uDiffuse = glGetUniformLocation(shaderProgram, "diffuse");
	uSpecular = glGetUniformLocation(shaderProgram, "specular");
	uNu = glGetUniformLocation(shaderProgram, "n_u");
	uNv = glGetUniformLocation(shaderProgram, "n_v");
	uRs = glGetUniformLocation(shaderProgram, "rs");
	uRd = glGetUniformLocation(shaderProgram, "rd");
}

// bindMaterial func
// Sets the relevant uniform variable values
void AshikhminMaterial::bindMaterial(){
	glUniform3f(uDiffuse, diffuse[0], diffuse[1], diffuse[2]);
	glUniform3f(uSpecular, specular[0], specular[1], specular[2]);
	glUniform1f(uNu, nu);
	glUniform1f(uNv, nv);
	glUniform1f(uRs, rs);
	glUniform1f(uRd, rd);
}


// setMaterial func
// Sets the specular, diffuse, and material level properties
void AshikhminMaterial::setMaterial(vec3 d, vec3 s, float ld, float ls){
	diffuse = d;
	specular = s;
	rd = ld;
	rs = ls;
}

// setRoughness func
// Sets the vertical and horizontal roughness for the specular lobes
void AshikhminMaterial::setRoughness(float u, float v){
	nu = u;
	nv = v;
}