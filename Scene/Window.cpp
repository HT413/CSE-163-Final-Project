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
const int numLights = 4;
vec4 lightPosition[numLights];
vec3 lightColor[numLights];

// Depth maps
DepthMap* depthMap[numLights];
mat4 lightProjection[numLights];
mat4 lightView[numLights];

// Material properties
Material *gold_Phong, *gold_Ashikhmin;
Material *ruby_Phong, *ruby_Ashikhmin;
Material *bronze_Phong, *bronze_Ashikhmin;
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

vec3 rubyAmbient = vec3(0.1745f, 0.01175f, 0.01175f);
vec3 rubyDiffuse_p = vec3(0.61424f, 0.04136f, 0.04136f);
vec3 rubySpecular_p = vec3(0.727811f, 0.626959f, 0.626959f);
float rubyShininess = 150.f;

vec3 rubyDiffuse_a = vec3(.735f, .091f, .091f);
vec3 rubySpecular_a = vec3(.962f, .336f, .336f);
float rubyRd = .05f;
float rubyRs = .95f;
float rubynu = 6.f;
float rubynv = 3.f;

vec3 bronzeAmbient = vec3(0.2125f, 0.1275f, 0.054f);
vec3 bronzeDiffuse_p = vec3(0.714f, 0.4284f, 0.18144f);
vec3 bronzeSpecular_p = vec3(0.393548f, 0.271906, 0.166721);
float bronzeShininess = 50.f;

vec3 bronzeDiffuse_a = vec3(.815f, .532f, .247f);
vec3 bronzeSpecular_a = vec3(.419f, .381f, .276f);
float bronzeRd = .3f;
float bronzeRs = .7f;
float bronzenu = 2.f;
float bronzenv = 5.f;

Material *dull_sphere;
vec3 sAmbient = vec3(.2f, .2f, .2f);
vec3 sDiff = vec3(.9f, .9f, .9f);
vec3 sSpec = vec3(.9f, .9f, .9f);
float sShine = 1.f;

// For the ground
vec3 groundColor = vec3(.6f, .6f, .6f);
Plane *ground;
Plane *testScreen;

bool isShadowMapping, showSphere;

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
vec3 sphereElevation = vec3(0, 3.5f, 0);

OBJObject* objModel, *objModel2, *objModel3;
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
	objModel = new OBJObject("objects/dragon.obj");
	objModel->setModel(translate(mat4(1.f), vec3(0, -.78f, 0)) * rotate(mat4(1.f), PI/2.f, vec3(0, 1, 0)));
	objModel2 = new OBJObject("objects/dragon.obj");
	objModel2->setModel(translate(mat4(1.f), vec3(-2.5f, -.78f, 0)) * rotate(mat4(1.f), PI/2.f, vec3(0, 1, 0)));
	objModel3 = new OBJObject("objects/dragon.obj");
	objModel3->setModel(translate(mat4(1.f), vec3(2.5f, -.78f, 0)) * rotate(mat4(1.f), PI/2.f, vec3(0, 1, 0)));
	sphere = new Sphere(20, 20);

	// Lights - directional
	lightPosition[0] = vec4(.4f, .9f, -.5f, 0.f);
	lightColor[0] = vec3(1, 1, 1);

	lightPosition[1] = vec4(-.6f, 1.f, .3f, 0.f);
	lightColor[1] = vec3(1, 1, 1);

	lightPosition[2] = vec4(-.3f, 1.f, -.2f, 0.f);
	lightColor[2] = vec3(1, 1, 1);

	lightPosition[3] = vec4(.7f, 1.f, .4f, 0.f);
	lightColor[3] = vec3(1, 1, 1);

	// Initialize shaders
	objShader = phongShader = LoadShaders("shaders/basic.vert", "shaders/phong.frag");
	ashikhminShader = LoadShaders("shaders/basic.vert", "shaders/ashikhmin.frag");
	// Phong shading and the regular materials
	glUseProgram(phongShader);
	gold_Phong = new RegMaterial();
	((RegMaterial*)gold_Phong)->setMaterial(goldAmbient, goldDiffuse_p, goldSpecular_p, goldShininess);
	gold_Phong->getUniformLocs(phongShader);

	ruby_Phong = new RegMaterial();
	((RegMaterial*)ruby_Phong)->setMaterial(rubyAmbient, rubyDiffuse_p, rubySpecular_p, rubyShininess);
	ruby_Phong->getUniformLocs(phongShader);

	bronze_Phong = new RegMaterial();
	((RegMaterial*)bronze_Phong)->setMaterial(bronzeAmbient, bronzeDiffuse_p, bronzeSpecular_p, bronzeShininess);
	bronze_Phong->getUniformLocs(phongShader);

	dull_sphere = new RegMaterial();
	((RegMaterial*)dull_sphere)->setMaterial(sAmbient, sDiff, sSpec, sShine);
	dull_sphere->getUniformLocs(phongShader);

	// Ashikhmin BRDF and the ashikhmin material
	glUseProgram(ashikhminShader);
	gold_Ashikhmin = new AshikhminMaterial();
	((AshikhminMaterial*)gold_Ashikhmin)->setMaterial(goldDiffuse_a, goldSpecular_a, goldRd, goldRs);
	((AshikhminMaterial*)gold_Ashikhmin)->setRoughness(goldnu, goldnv);
	gold_Ashikhmin->getUniformLocs(ashikhminShader);

	ruby_Ashikhmin = new AshikhminMaterial();
	((AshikhminMaterial*)ruby_Ashikhmin)->setMaterial(rubyDiffuse_a, rubySpecular_a, rubyRd, rubyRs);
	((AshikhminMaterial*)ruby_Ashikhmin)->setRoughness(rubynu, rubynv);
	ruby_Ashikhmin->getUniformLocs(ashikhminShader);

	bronze_Ashikhmin = new AshikhminMaterial();
	((AshikhminMaterial*)bronze_Ashikhmin)->setMaterial(bronzeDiffuse_a, bronzeSpecular_a, bronzeRd, bronzeRs);
	((AshikhminMaterial*)bronze_Ashikhmin)->setRoughness(bronzenu, bronzenv);
	bronze_Ashikhmin->getUniformLocs(ashikhminShader);

	objModel->setMaterial(gold_Phong, gold_Ashikhmin);
	objModel2->setMaterial(ruby_Phong, ruby_Ashikhmin);
	objModel3->setMaterial(bronze_Phong, bronze_Ashikhmin);

	sphere->setModel(translate(mat4(1.f), sphereElevation));
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
	for(int i = 0; i < numLights; i++){
		depthMap[i] = new DepthMap(i + 1);
		lightProjection[i] = ortho(-20.f, 20.f, -20.f, 20.f, .1f, 15.f);
		lightView[i] = lookAt(vec3(lightPosition[i]) * 5.f, vec3(0, 0, 0), vec3(0, 1, 0));
	}

	// Misc initializations
	usingPhong = true;
	sessionScreenshots = 0;
	showSphere = true;
	isShadowMapping = true;
}

void destroyObjects(){
	if(objModel) delete objModel;
	if(objModel2) delete objModel2;
	if(objModel3) delete objModel3;
	if(gold_Phong) delete gold_Phong;
	if(gold_Ashikhmin) delete gold_Ashikhmin;
	if(ruby_Phong) delete ruby_Phong;
	if(ruby_Ashikhmin) delete ruby_Ashikhmin;
	if(bronze_Phong) delete bronze_Phong;
	if(bronze_Ashikhmin) delete bronze_Ashikhmin;
	if(testScreen) delete testScreen;
	for(int i = 0; i < numLights; i++) delete depthMap[i];
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

void renderReflection(GLFWwindow* window, int side){
	for(int i = 0; i < numLights; i++){
		glUseProgram(depthShader);
		depthMap[i]->bind();
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "projection"), 1, GL_FALSE, &(lightProjection[i][0][0]));
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "view"), 1, GL_FALSE, &(lightView[i][0][0]));

		ground->draw(depthShader);
		if(showSphere) sphere->draw(depthShader);
		objModel->draw(depthShader);
		objModel2->draw(depthShader);
		objModel3->draw(depthShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	mat4 sProj = perspective(PI / 2.f, 1.f, 0.1f, 10.0f);
	mat4 sView;
		if(side == 0)
			sView = lookAt(sphereElevation, sphereElevation + vec3(1, 0, 0), vec3(0, -1, 0));
		else if(side == 1)
			sView = lookAt(sphereElevation, sphereElevation + vec3(-1, 0, 0), vec3(0, -1, 0));
		else if(side == 2)
			sView = lookAt(sphereElevation, sphereElevation + vec3(0, 1, 0), vec3(0, 0, 1));
		else if(side == 3)
			sView = lookAt(sphereElevation, sphereElevation + vec3(0, -1, 0), vec3(0, 0, -1));
		else if(side == 4)
			sView = lookAt(sphereElevation, sphereElevation + vec3(0, 0, 1), vec3(0, -1, 0));
		else if(side == 5)
			sView = lookAt(sphereElevation, sphereElevation + vec3(0, 0, -1), vec3(0, -1, 0));

	// Regular draw here
	glDisable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 512, 512);
	glDepthMask(GL_FALSE);
	glUseProgram(skyShader);
	skybox->display(skyShader, sProj, sView);
	glDepthMask(GL_TRUE);

	GLint maps[4] = {1, 2, 3, 4};

	glUseProgram(texShader);
	glUniformMatrix4fv(glGetUniformLocation(texShader, "projection"), 1, GL_FALSE, &(sProj[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(texShader, "view"), 1, GL_FALSE, &(sView[0][0]));
	glUniform1iv(glGetUniformLocation(texShader, "lightMap"), numLights, maps);
	glUniform1i(glGetUniformLocation(texShader, "numLights"), numLights);
	glUniform1i(glGetUniformLocation(texShader, "isShadowMap"), (isShadowMapping)? 1 : 0);
	glUniformMatrix4fv(glGetUniformLocation(texShader, "lightProject"), numLights, GL_FALSE, &(lightProjection[0][0][0]));
	glUniformMatrix4fv(glGetUniformLocation(texShader, "lightView"), numLights, GL_FALSE, &(lightView[0][0][0]));
	//testScreen->draw();
	ground->draw();

	glUseProgram(objShader);
	glUniformMatrix4fv(glGetUniformLocation(objShader, "projection"), 1, GL_FALSE, &(sProj[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(objShader, "view"), 1, GL_FALSE, &(sView[0][0]));
	glUniform3f(glGetUniformLocation(objShader, "camPos"), sphereElevation[0], sphereElevation[1], sphereElevation[2]);
	glUniform4fv(glGetUniformLocation(objShader, "lightPos"), numLights, &lightPosition[0][0]);
	glUniform3fv(glGetUniformLocation(objShader, "lightCol"), numLights, &lightColor[0][0]);
	glUniform1iv(glGetUniformLocation(objShader, "lightMap"), numLights, maps);
	glUniform1i(glGetUniformLocation(objShader, "numLights"), numLights);
	glUniform1i(glGetUniformLocation(objShader, "isShadowMap"), (isShadowMapping)? 1 : 0);
	glUniformMatrix4fv(glGetUniformLocation(objShader, "lightProject"), numLights, GL_FALSE, &(lightProjection[0][0][0]));
	glUniformMatrix4fv(glGetUniformLocation(objShader, "lightView"), numLights, GL_FALSE, &(lightView[0][0][0]));
	if(!usingPhong) skybox->bindTexture(objShader);
	objModel->draw(objShader);
	objModel2->draw(objShader);
	objModel3->draw(objShader);

	skybox->rebindReflection(side);
}

void displayCallback(GLFWwindow* window){
	// Draw the relevant depth map
	for(int i = 0; i < numLights; i++){
		glUseProgram(depthShader);
		depthMap[i]->bind();
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "projection"), 1, GL_FALSE, &(lightProjection[i][0][0]));
		glUniformMatrix4fv(glGetUniformLocation(depthShader, "view"), 1, GL_FALSE, &(lightView[i][0][0]));

		ground->draw(depthShader);
		if(showSphere) sphere->draw(depthShader);
		objModel->draw(depthShader);
		objModel2->draw(depthShader);
		objModel3->draw(depthShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Regular draw here
	glDisable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glDepthMask(GL_FALSE);
	glUseProgram(skyShader);
	skybox->display(skyShader, projection, view);
	glDepthMask(GL_TRUE);

	GLint maps[4] = {1, 2, 3, 4};

	glUseProgram(texShader);
	glUniformMatrix4fv(glGetUniformLocation(texShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(texShader, "view"), 1, GL_FALSE, &(view[0][0]));
	glUniform1iv(glGetUniformLocation(texShader, "lightMap"), numLights, maps);
	glUniform1i(glGetUniformLocation(texShader, "numLights"), numLights);
	glUniform1i(glGetUniformLocation(texShader, "isShadowMap"), (isShadowMapping)? 1 : 0);
	glUniformMatrix4fv(glGetUniformLocation(texShader, "lightProject"), numLights, GL_FALSE, &(lightProjection[0][0][0]));
	glUniformMatrix4fv(glGetUniformLocation(texShader, "lightView"), numLights, GL_FALSE, &(lightView[0][0][0]));
	//testScreen->draw();
	ground->draw();
	
	glUseProgram(objShader);
	glUniformMatrix4fv(glGetUniformLocation(objShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(objShader, "view"), 1, GL_FALSE, &(view[0][0]));
	glUniform3f(glGetUniformLocation(objShader, "camPos"), cam_pos[0], cam_pos[1], cam_pos[2]);
	glUniform4fv(glGetUniformLocation(objShader, "lightPos"), numLights, &lightPosition[0][0]);
	glUniform3fv(glGetUniformLocation(objShader, "lightCol"), numLights, &lightColor[0][0]);
	glUniform1iv(glGetUniformLocation(objShader, "lightMap"), numLights, maps);
	glUniform1i(glGetUniformLocation(objShader, "numLights"), numLights);
	glUniform1i(glGetUniformLocation(objShader, "isShadowMap"), (isShadowMapping)? 1 : 0);
	glUniformMatrix4fv(glGetUniformLocation(objShader, "lightProject"), numLights, GL_FALSE, &(lightProjection[0][0][0]));
	glUniformMatrix4fv(glGetUniformLocation(objShader, "lightView"), numLights, GL_FALSE, &(lightView[0][0][0]));
	if(!usingPhong) skybox->bindTexture(objShader);
	objModel->draw(objShader);
	objModel2->draw(objShader);
	objModel3->draw(objShader);

	if(showSphere){
		if(usingPhong){
			sphere->draw(objShader);
		}
		else{
			glUseProgram(reflectShader);
			glUniformMatrix4fv(glGetUniformLocation(reflectShader, "projection"), 1, GL_FALSE, &(projection[0][0]));
			glUniformMatrix4fv(glGetUniformLocation(reflectShader, "view"), 1, GL_FALSE, &(view[0][0]));
			glUniform3f(glGetUniformLocation(reflectShader, "cameraPos"), cam_pos[0], cam_pos[1], cam_pos[2]);
			skybox->bindReflection(reflectShader);
			sphere->draw(reflectShader);
		}
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
			for(int i = 0; i < 6; i++){
				if(i == 2) continue;
				renderReflection(window, i);
			}
			break;

		// Toggle displaying the sphere
		case GLFW_KEY_S:
			showSphere = !showSphere;
			break;

		// Toggle shadow mapping
		case GLFW_KEY_M:
			isShadowMapping = !isShadowMapping;	
			for(int i = 0; i < 6; i++){
				if(i == 2) continue;
				renderReflection(window, i);
			}
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