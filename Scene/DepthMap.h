// Declarations for a depth map. Used for shadow mapping.

#ifndef _DEPTH_MAP_H_
#define _DEPTH_MAP_H_

#include "Globals.h"

class DepthMap
{
private:
	int texUnit;
	GLuint FBO, texID;
	const int SHADOW_W = 1024;
	const int SHADOW_H = 1024;

public:
	DepthMap(int);
	~DepthMap();
	void bind();
	void bindTexture();
};

#endif