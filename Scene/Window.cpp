#include "Window.h"
#include "Shader.hpp"
#include "OBJObject.h"

// Window width and height
int width, height;

// For the trackball
double lastX, lastY;
enum trackballAction { NO_ACTION, C_ROTATE };
trackballAction mouseAction;

// For shader programs
bool usingPhong;
GLuint phongShader, ashikhminShader, objShader;

// Light properties
const int MAX_LIGHTS = 8;
int numLights;
float *lightPositions;
float *lightColors;

// Material properties
Material *gold_Phong, *gold_Ashikhmin;
vec3 goldAmbient = vec3(.24725f, .1995f, .0745f);
vec3 goldDiffuse_p = vec3(.75164f, .60648f, .22648f);
vec3 goldSpecular_p = vec3(.628281f, .555802f, .366065f);
float goldShininess = 100.f;

vec3 goldDiffuse_a = vec3(.1f, .1f, .1f);
vec3 goldSpecular_a = vec3(1.f, .75f, .3f);
float goldRd = .05f;
float goldRs = .95f;
float goldnu = 10.f;
float goldnv = 1000.f;

// Other variables
vec3 cam_pos(0, 0, 5), cam_lookAt(0, 0, 0) , cam_up(0, 1, 0);
mat4 projection, view;

OBJObject* dragon;

GLFWwindow* createWindow(int w, int h){
	// Initialize GLFW
	if(!glfwInit())
	{
		cerr << "Failed to initialize GLFW!" << endl;
		PROGERR(30);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(w, h, WINDOW_TITLE, 0, 0);

	// Check if the window could not be created
	if(!window)
	{
		cerr << "Failed to open GLFW window!" << endl;
		glfwTerminate();
		PROGERR(31);
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	resizeCallback(window, width, height);

	return window;
}

void initObjects(){
	// Create the model
	dragon = new OBJObject("objects/bunny.obj");

	// Lights
	numLights = 1;
	lightPositions = new float[3 * MAX_LIGHTS];
	lightColors = new float[3 * MAX_LIGHTS];

	lightPositions[0] = 1.f; lightPositions[1] = 1.f; lightPositions[2] = 0.f; lightPositions[3] = 0.f;
	lightColors[0] = 1.f; lightColors[1] = 1.f; lightColors[2] = .9f;

	// Initialize shaders
	objShader = phongShader = LoadShaders("shaders/basic.vert", "shaders/phong.frag");
	ashikhminShader = LoadShaders("shaders/basic.vert", "shaders/ashikhmin.frag");

	glUseProgram(phongShader);
	gold_Phong = new RegMaterial();
	((RegMaterial*)gold_Phong)->setMaterial(goldAmbient, goldDiffuse_p, goldSpecular_p, goldShininess);
	gold_Phong->getUniformLocs(phongShader);

	glUseProgram(ashikhminShader);
	gold_Ashikhmin = new AshikhminMaterial();
	((AshikhminMaterial*)gold_Phong)->setMaterial(goldDiffuse_a, goldSpecular_a, goldRd, goldRs);
	((AshikhminMaterial*)gold_Phong)->setRoughness(goldnu, goldnv);
	gold_Ashikhmin->getUniformLocs(ashikhminShader);

	dragon->setMaterial(gold_Phong, gold_Ashikhmin);

	// Misc initializations
	usingPhong = true;
}

void destroyObjects(){
	if(dragon) delete dragon;
	if(gold_Phong) delete gold_Phong;
	if(gold_Ashikhmin) delete gold_Ashikhmin;
	if(lightPositions) delete[] lightPositions;
	if(lightColors) delete[] lightColors;
}

void resizeCallback(GLFWwindow* window, int w, int h){
	width = w;
	height = h;
	// Set the viewport size
	glViewport(0, 0, width, height);

	if(height > 0)
	{
		projection = perspective(PI / 2.f, (float)width / (float)height, 0.1f, 1000.0f);
		view = lookAt(cam_pos, cam_lookAt, cam_up);
	}
}

void update(){

}

void displayCallback(GLFWwindow* window){
	// Draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(objShader);
	glUniform1i(glGetUniformLocation(objShader, "numLights"), numLights);
	glUniformMatrix4fv(glGetUniformLocation(objShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(objShader, "view"), 1, GL_FALSE, &(view[0][0]));
	glUniform3f(glGetUniformLocation(objShader, "camPos"), cam_pos[0], cam_pos[1], cam_pos[2]);
	glUniform4fv(glGetUniformLocation(objShader, "lights"), numLights, lightPositions);
	glUniform3fv(glGetUniformLocation(objShader, "lightCols"), numLights, lightColors);

	dragon->draw(objShader);

	glfwSwapBuffers(window);

	glfwPollEvents();
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS) {
		switch(key) {
		// Toggle between phong shading and ashikhmin shading on pressing "i"
		case GLFW_KEY_I:
			usingPhong = !usingPhong;
			objShader = (usingPhong)? phongShader : ashikhminShader;
			break;

		// Kill the program on pressing Esc
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	// Camera rotation
	if(mouseAction == C_ROTATE) {
		float angle;
		// Perform horizontal (y-axis) rotation
		angle = (float)(lastX - xpos) / 100.0f;
		cam_pos = vec3(rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f)) * vec4(cam_pos, 1.0f));
		cam_up = vec3(rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f)) * vec4(cam_up, 1.0f));
		//Now rotate vertically based on current orientation
		angle = (float)(ypos - lastY) / 100.0f;
		vec3 axis = cross(cam_pos - cam_lookAt, cam_up);
		cam_pos = vec3(rotate(mat4(1.0f), angle, axis) * vec4(cam_pos, 1.0f));
		cam_up = vec3(rotate(mat4(1.0f), angle, axis) * vec4(cam_up, 1.0f));
		// Now update the camera
		view = lookAt(cam_pos, cam_lookAt, cam_up);
		lastX = xpos;
		lastY = ypos;
	}
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if(action == GLFW_RELEASE) {
		mouseAction = NO_ACTION;
	}
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &lastX, &lastY);
		mouseAction = C_ROTATE;
	}
}