#ifndef _GRAPHICS_WINDOW_H
#define _GRAPHICS_WINDOW_H

#include "main.h"

void initObjects();
void destroyObjects();
GLFWwindow* createWindow(int, int);
void resizeCallback(GLFWwindow*, int, int);
void update();
void displayCallback(GLFWwindow*);
void keyCallback(GLFWwindow*, int, int, int, int);
void cursorCallback(GLFWwindow*, double, double);
void mouseCallback(GLFWwindow*, int, int, int);
void scrollCallback(GLFWwindow*, double, double);

#endif