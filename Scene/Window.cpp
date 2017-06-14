#include "Window.h"
#include "Shader.hpp"
#include "OBJObject.h"
#include "Plane.h"
#include "Skybox.h"
#include "Sphere.h"
#include "DepthMap.h"

#include <FreeImage.h>

// Window width and height
int width, height;
int sessionScreenshots;

// For the trackball
double lastX, lastY;
enum trackballAction { NO_ACTION, C_ROTATE };
trackballAction mouseAction;

// For shader programs
bool usingPhong;
GLuint phongShader, ashikhminShader, objShader, texShader;
GLuint skyShader, reflectShader, depthShader;

// Light properties
vec4 lightPosition;
vec3 lightColor;

// Depth maps
DepthMap* depthMap;
mat4 lightProjection;
mat4 lightView;

// Material properties
Material *gold_Phong, *gold_Ashikhmin;
vec3 goldAmbient = vec3(.24725f, .1995f, .0745f);
vec3 goldDiffuse_p = vec3(.75164f, .60648f, .22648f);
vec3 goldSpecular_p = vec3(.628281f, .555802f, .366065f);
float goldShininess = 100.f;

vec3 goldDiffuse_a = vec3(.5f, .37f, .15f);
vec3 goldSpecular_a = vec3(1.f, .75f, .3f);
float goldRd = .1f;
float goldRs = .9f;
float goldnu = 5.f;
float goldnv = 10.f;

Material *dull_sphere;
vec3 sAmbient = vec3(.2f, .2f, .2f);
vec3 sDiff = vec3(.9f, .9f, .9f);
vec3 sSpec = vec3(.9f, .9f, .9f);
float sShine = 1.f;

// For the ground
vec3 groundColor = vec3(.6f, .6f, .6f);
Plane *ground;
Plane *testScreen;

// Skybox
const char* texFiles[6] ={
	"textures/right.ppm",
	"textures/left.ppm",
	"textures/top.ppm",
	"textures/bottom.ppm",
	"textures/back.ppm",
	"textures/front.ppm"
};
const char* plainColor[6] ={
	"textures/plain.ppm", "textures/plain.ppm", "textures/plain.ppm",
	"textures/plain.ppm", "textures/plain.ppm", "textures/plain.ppm"
};
Skybox *skybox;

// Other variables
vec3 cam_pos(0, 0, 7), cam_lookAt(0, 0, 0) , cam_up(0, 1, 0);
mat4 projection, view;
const mat4 identityMat = mat4(1.f);

OBJObject* objModel;
Sphere * sphere;

// Helper func generates random string; len = number of characters, appends ".jpg" to end
void gen_random(char *s, const int len) {
	const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";

	for(int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	int i = sessionScreenshots;
	int j = 1;
	do{
		s[len - j] = '0' + (i % 10);
		j++;
		i/=10;
	} while(i > 0);
	s[len - j] = '_';
	s[len] = '.';
	s[len + 1] = 'p';
	s[len + 2] = 'n';
	s[len + 3] = 'g';
	s[len + 4] = 0;
}

// Helper func to save a screenshot
void saveScreenshot() {
	int pix = width * height;
	BYTE *pixels = new BYTE[3*pix];
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

	char *s = new char[15];
	gen_random(s, 10);
	sessionScreenshots++;
	FreeImage_Save(FIF_PNG, img, s, 0);
	cout << "Saved screenshot: " << s << endl;
	delete s;
	delete pixels;
}

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
	srand(rand() % 32768);

	// Create the model
	objModel = new OBJObject("objects/tank_T72.obj");
	sphere = new Sphere(20, 20);

	// Light - directional
	lightPosition = vec4(.4f, .9f, -.5f, 0.f);
	lightColor = vec3(1, 1, 1);

	// Initialize shaders
	objShader = phongShader = LoadShaders("shaders/basic.vert", "shaders/phong.frag");
	ashikhminShader = LoadShaders("shaders/basic.vert", "shaders/ashikhmin.frag");
	// Phong shading and the regular materials
	glUseProgram(phongShader);
	gold_Phong = new RegMaterial();
	((RegMaterial*)gold_Phong)->setMaterial(goldAmbient, goldDiffuse_p, goldSpecular_p, goldShininess);
	gold_Phong->getUniformLocs(phongShader);

	dull_sphere = new RegMaterial();
	((RegMaterial*)dull_sphere)->setMaterial(sAmbient, sDiff, sSpec, sShine);
	dull_sphere->getUniformLocs(phongShader);

	// Ashikhmin BRDF and the ashikhmin material
	glUseProgram(ashikhminShader);
	gold_Ashikhmin = new AshikhminMaterial();
	((AshikhminMaterial*)gold_Ashikhmin)->setMaterial(goldDiffuse_a, goldSpecular_a, goldRd, goldRs);
	((AshikhminMaterial*)gold_Ashikhmin)->setRoughness(goldnu, goldnv);
	gold_Ashikhmin->getUniformLocs(ashikhminShader);

	objModel->setMaterial(gold_Phong, gold_Ashikhmin);

	sphere->setModel(translate(mat4(1.f), vec3(0.f, 3.5f, 0.f)));
	sphere->setMaterial(dull_sphere);

	// Create the ground
	texShader = LoadShaders("shaders/texture.vert", "shaders/texture.frag");
	glUseProgram(texShader);
	ground = new Plane(texShader);
	ground->setColor(groundColor);
	ground->setModel(translate(mat4(1), vec3(0, -2.5f, 0)) 
		* rotate(mat4(1), -PI/2.f, vec3(1, 0, 0))
		* scale(mat4(1), vec3(50, 50, 1)));

	testScreen = new Plane(texShader);
	testScreen->setModel(scale(mat4(1), vec3(2, 2, 1)));
	testScreen->doTexture();

	// Create skybox
	skyShader = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");
	glUseProgram(skyShader);
	skybox = new Skybox(texFiles);

	// Environment mapping shader
	reflectShader = LoadShaders("shaders/reflect.vert", "shaders/reflect.frag");

	// Depth map shader
	depthShader = LoadShaders("shaders/depthmap.vert", "shaders/depthmap.frag");
	depthMap = new DepthMap(1);
	lightProjection = ortho(-10.f, 10.f, -10.f, 10.f, .1f, 15.f);
	lightView = lookAt(vec3(lightPosition) * 5.f, vec3(0, 0, 0), vec3(0, 1, 0));

	// Misc initializations
	usingPhong = true;
	sessionScreenshots = 0;
}

void destroyObjects(){
	if(objModel) delete objModel;
	if(gold_Phong) delete gold_Phong;
	if(gold_Ashikhmin) delete gold_Ashikhmin;
	if(testScreen) delete testScreen;
	if(depthMap) delete depthMap;
	if(ground) delete ground;
	if(skybox) delete skybox;
	if(sphere) delete sphere;
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
	// Draw the relevant depth map
	glUseProgram(depthShader);
	depthMap->bind();
	glUniformMatrix4fv(glGetUniformLocation(depthShader, "projection"), 1, GL_FALSE, &(lightProjection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(depthShader, "view"), 1, GL_FALSE, &(lightView[0][0]));

	ground->draw(depthShader);
	sphere->draw(depthShader);
	objModel->draw(depthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Regular draw here
	glDisable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glDepthMask(GL_FALSE);
	glUseProgram(skyShader);
	skybox->display(skyShader, projection, view);
	glDepthMask(GL_TRUE);

	glUseProgram(texShader);
	glUniformMatrix4fv(glGetUniformLocation(texShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(texShader, "view"), 1, GL_FALSE, &(view[0][0]));
	glUniform1i(glGetUniformLocation(texShader, "lightMap"), 1);
	glUniformMatrix4fv(glGetUniformLocation(texShader, "lightProject"), 1, GL_FALSE, &(lightProjection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(texShader, "lightView"), 1, GL_FALSE, &(lightView[0][0]));
	//testScreen->draw();
	ground->draw();
	
	glUseProgram(objShader);
	glUniformMatrix4fv(glGetUniformLocation(objShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(objShader, "view"), 1, GL_FALSE, &(view[0][0]));
	glUniform3f(glGetUniformLocation(objShader, "camPos"), cam_pos[0], cam_pos[1], cam_pos[2]);
	glUniform4fv(glGetUniformLocation(objShader, "lightPos"), 1, &lightPosition[0]);
	glUniform3fv(glGetUniformLocation(objShader, "lightCol"), 1, &lightColor[0]);
	glUniform1i(glGetUniformLocation(objShader, "lightMap"), 1);
	glUniformMatrix4fv(glGetUniformLocation(objShader, "lightProject"), 1, GL_FALSE, &(lightProjection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(objShader, "lightView"), 1, GL_FALSE, &(lightView[0][0]));
	if(!usingPhong) skybox->bindTexture(objShader);
	objModel->draw(objShader);

	if(usingPhong){
		sphere->draw(objShader);
	}
	else{
		glUseProgram(reflectShader);
		glUniformMatrix4fv(glGetUniformLocation(reflectShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
		glUniformMatrix4fv(glGetUniformLocation(reflectShader, "view"), 1, GL_FALSE, &(view[0][0]));
		glUniform3f(glGetUniformLocation(reflectShader, "cameraPos"), cam_pos[0], cam_pos[1], cam_pos[2]);
		skybox->bindTexture(reflectShader);
		sphere->draw(reflectShader);
	}
	
	glfwSwapBuffers(window);

	glfwPollEvents();
}

// Keyboard callback func
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS) {
		switch(key) {
		// Take a screenshot of the application
		case GLFW_KEY_P:
			saveScreenshot();
			break;

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

// Mouse cursor callback func
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

// Mouse button callback func
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

// Scroll wheel callback func
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset){
	cam_pos *= (yOffset > 0)? .99f : 1.01f;
	view = lookAt(cam_pos, cam_lookAt, cam_up);
}