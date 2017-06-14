// Definitions for the depth map used in shadow mapping
#include "DepthMap.h"

DepthMap::DepthMap(int ID)
{
	texUnit = ID;
	// Generate the units of concern
	glGenFramebuffers(1, &FBO);
	glGenTextures(1, &texID);
	// Initialize texture properties
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texID); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_W, SHADOW_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] ={1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Attach texture to frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// Bind texture to this frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


DepthMap::~DepthMap()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &texID);
}

// Bind func - binds the render buffer so we can draw the depth map
void DepthMap::bind(){
	glViewport(0, 0, SHADOW_W, SHADOW_H);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texID);
}

// Bind texture func - simply binds its texture
void DepthMap::bindTexture(){
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texID);
}