// Declarations and definitions for the materials class

#ifndef _MATERIALS_FILE_H_
#define _MATERIALS_FILE_H_

#include "Globals.h"

struct Material{
protected:
	vec3 diffuse;
	vec3 specular;
	GLuint uDiffuse, uSpecular;
public:
	virtual void getUniformLocs(GLuint) = 0;
	virtual void bindMaterial() = 0;
};

struct RegMaterial : public Material{
protected:
	vec3 ambient;
	float shininess;
	GLuint uAmbient, uShininess;
public:
	void bindMaterial() override;
	void getUniformLocs(GLuint) override;
	void setMaterial(vec3, vec3, vec3, float);
};

struct AshikhminMaterial: public Material{
protected:
	float nu, nv, rs, rd;
	GLuint uNu, uNv, uRs, uRd;
public:
	void bindMaterial() override;
	void getUniformLocs(GLuint) override;
	void setMaterial(vec3, vec3, float, float);
	void setRoughness(float, float);
};

#endif