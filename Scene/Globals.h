#ifndef GLOBAL_DECLS
#define GLOBAL_DECLS

#define _CRT_SECURE_NO_WARNINGS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

#define WINDOW_TITLE "Final Project"
#define PI 3.14159265359f
#define PROGERR(x) getchar(); exit(x);

using namespace std;
using namespace glm;

extern bool usingPhong;

#endif
