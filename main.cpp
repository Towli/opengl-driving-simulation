#include <windows.h>
#include "gl/glew.h"
#include "gl/wglew.h"
#pragma comment(lib, "glew32.lib")

#include "shaders/Shader.h"   // include shader header file, this is not part of OpenGL

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "CollisionTest.h"
#include "Octree/Octree.h"

// SHADER OBJECTS
Shader* myShader; 
Shader* myBasicShader;
Shader* cubeMapShader;

// GAME OBJECTS
#include "GameObject.h"
GameObject ground;
GameObject car;

// CUBEMAP
#include "CubeMap.h"
CubeMap* cubeMap;

// MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"
ThreeDModel* cityBlocks;
ThreeDModel mitsubishi;
ThreeDModel frontWheelLeft, frontWheelRight;
ThreeDModel backWheels;
vector<ThreeDModel*> buildings;
vector<ThreeDModel*> lightPoles;
OBJLoader objLoader;

// CAMERA
#include "Camera.h"
Camera camera;

// MATRICES
glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

// MATERIAL PROPERTIES
float Material_Ambient[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
float Material_Diffuse[4] = {0.8f, 0.8f, 0.5f, 1.0f};
float Material_Specular[4] = {0.9f,0.9f,0.8f,1.0f};
float Material_Shininess = 50;

//LIGHT PROPERTIES
float Light_Ambient_And_Diffuse[4] = {0.8f, 0.8f, 0.9f, 1.0f};
float Light_Specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float LightPos[4] = { 0.0f, 1.0f, 0.0f, 0.0f };

// SIMULATION STATES
enum class SimulationState { RUNNING = 1, ENDED = 2 };
SimulationState STATE;

// MISC INPUT VARIABLES
int	mouse_x=0, mouse_y=0;
bool LeftPressed = false;
int screenWidth=600, screenHeight=600;
bool keys[256];

//DELTA-TIME
#include <time.h>
clock_t t;
double currentTime = clock();
double previousTime = 0.0;
double deltaTime = 0.0;

// GLOBALS
bool drawBoundingSpheres = false;
bool drawBoundingBoxes = false;
float mapMinX = -490.0f, mapMaxX = 490.0f, mapMinZ = -490.0f, mapMaxZ = 490.0f;
float fallAngle = 0.0f;

//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void update(double deltaTime);				//called in winmain to update variables
void handleLighting();
void toggleLighting();
void calculateDeltaTime();
ThreeDModel* loadModel(char* filePath, Shader* shader);
void handleCollisions();
void handleOOB();
void resetSimulation();

/*************    START OF OPENGL FUNCTIONS   ****************/
void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);	//sets the clear colour to yellow
	glEnable(GL_DEPTH_TEST);

	// --------------------- LOAD SHADERS ---------------------
	myShader = new Shader;
	if (!myShader->load("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load basicTransformations shader" << endl;
	}

	myBasicShader = new Shader;
	if (!myBasicShader->load("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	{
		cout << "failed to load basic shader" << endl;
	}

	cubeMapShader = new Shader;
	if (!cubeMapShader->load("CubeMap", "glslfiles/cubeMap.vert", "glslfiles/cubeMap.frag"))
	{
		cout << "failed to load CubeMap shader" << endl;
	}

	glUseProgram(myShader->handle());

	glEnable(GL_TEXTURE_2D);

	// --------------------- MODEL LOADING ---------------------
	// Load models with .obj loader
	vector<ThreeDModel*> carAnatomy = vector<ThreeDModel*>();
	cityBlocks = loadModel("Models/CityBlocksRoads/city_roads.obj", myShader);
	carAnatomy.push_back(loadModel("Models/mitsubishi_eclipse/mitsubishi_chassis.obj", myShader));
	carAnatomy.push_back(loadModel("Models/mitsubishi_eclipse/front_wheel_left.obj", myShader));
	carAnatomy.push_back(loadModel("Models/mitsubishi_eclipse/front_wheel_right.obj", myShader));
	carAnatomy.push_back(loadModel("Models/mitsubishi_eclipse/back_wheels.obj", myShader));
	
	cityBlocks->calcCentrePoint();
	cityBlocks->centreOnZero();

	// Load buildings
	buildings = vector<ThreeDModel*>();
	int numBuildingModels = 47;
	for (int i = 1; i <= numBuildingModels; i++) {
		string strPath = "Models/buildings/building_" + to_string(i) + ".obj";
		char* path = &strPath[0];
		cout << "path: " << path << endl;
		buildings.push_back(loadModel(path, myShader));
	}

	// Load light poles
	lightPoles = vector<ThreeDModel*>();
	int numLightPoles = 79;
	for (int i = 1; i <= numLightPoles; i++) {
		string strPath = "Models/light_poles/light_pole_" + to_string(i) + ".obj";
		char* path = &strPath[0];
		cout << "path: " << path << endl;
		lightPoles.push_back(loadModel(path, myShader));
	}

	// --------------------- INIT GAMEOBJECTS ---------------------
	car = GameObject(myShader, carAnatomy);
	ground = GameObject(myShader, cityBlocks);

	// --------------------- INIT CUBEMAP ---------------------
	cubeMap = new CubeMap(cubeMapShader);
	
	// --------------------- INIT CAMERA ---------------------
	// Initialise a third-person camera to follow a Box object
	camera = Camera(&car, Type::THIRD);

	// -------------- INIT SIMULATION STATE ------------------
	STATE = SimulationState::RUNNING;
}

void toggleLighting(int i)
{
	if (i == 1)
	{
		Light_Ambient_And_Diffuse[0] = { 0.2f };
		Light_Ambient_And_Diffuse[1] = { 0.2f };
		Light_Ambient_And_Diffuse[2] = { 0.2f };
		Light_Ambient_And_Diffuse[3] = { 1.0f };
		LightPos[0] = { 0.0f };
		LightPos[1] = { 0.0f };
		LightPos[2] = { 0.0f };
		LightPos[3] = { 0.0f };
	}
	else {
		Light_Ambient_And_Diffuse[0] = { 0.8f };
		Light_Ambient_And_Diffuse[1] = { 0.8f };
		Light_Ambient_And_Diffuse[2] = { 0.9f };
		Light_Ambient_And_Diffuse[3] = { 1.0f };
		LightPos[0] = { 0.0f };
		LightPos[1] = { 1.0f };
		LightPos[2] = { 0.0f };
		LightPos[3] = { 0.0f };
	}

}
void handleLighting()
{
	glUniform4fv(glGetUniformLocation(myShader->handle(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->handle(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->handle(), "material_shininess"), Material_Shininess);
}
void resetSimulation()
{
	car.setOutOfBounds(false);
	fallAngle = 0.0f;
	car.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	car.setSpeed(0.0f);
	car.setBusted(false);
	STATE = SimulationState::RUNNING;
}

void display()									
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glUseProgram(myShader->handle());  // use the shader

	GLuint matLocation = glGetUniformLocation(myShader->handle(), "ProjectionMatrix");  
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewingMatrix;
	glm::vec3 carPosition = car.getPosition();

	// Get the camera's viewing matrix
	viewingMatrix = camera.getViewingMatrix();
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	handleLighting();

	//DRAW THE CUBEMAP
	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0.0f, 100.0f, 0.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(car.getPosition().x*0.9, car.getPosition().y, car.getPosition().z*0.9));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
	glUseProgram(cubeMapShader->handle());
	glUniformMatrix4fv(glGetUniformLocation(cubeMapShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(cubeMapShader->handle(), "ViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniform4fv(glGetUniformLocation(cubeMapShader->handle(), "Brightness"), 1, Light_Ambient_And_Diffuse);
	cubeMap->render();

	//DRAW THE CAR
	ModelViewMatrix = viewingMatrix;
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUseProgram(myBasicShader->handle());  // use the shader
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ProjectionMatrix"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUseProgram(myShader->handle());  // use the shader

	// If car is out of map bounds, fall
	if (car.isOutOfBounds()) {
		// Model (local) Transformations on primary GameObject (Car)
		ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(carPosition));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, car.getDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, fallAngle, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 0.0f, -6.0f));
	}
	else
	{
		// Model (local) Transformations on primary GameObject (Car)
		ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(carPosition));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, car.getDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
		if (drawBoundingSpheres) {
			// Draw Car's Bounding Sphere
			glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
			car.drawGeometry();
		}
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 0.0f, -6.0f));
	}

	// ModelView (global) transformations on primary GameObject (Car)
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, -3.5f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.4f, 0.4f, 0.4f));

	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	for each(ThreeDModel* m in car.getModels()) {
		m->drawElementsUsingVBO(myShader);
	}

	// DRAW THE GROUND
	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0.0, -2.0, 0.0));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	//Pass the uniform for the modelview matrix - in this case just "r"
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	ground.getModel()->drawElementsUsingVBO(myShader);

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0.0, -2.0, 0.0));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->handle(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	//Pass the uniform for the modelview matrix - in this case just "r"
	glUniformMatrix4fv(glGetUniformLocation(myShader->handle(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	for each(ThreeDModel* m in buildings) {
		m->drawElementsUsingVBO(myShader);
		if (drawBoundingBoxes)
			m->drawBoundingBox(myShader);
	}

	for each(ThreeDModel* m in lightPoles) {
		m->drawElementsUsingVBO(myShader);
	}

	glFlush();
}

ThreeDModel* loadModel(char* filePath, Shader* shader)
{
	ThreeDModel* model = new ThreeDModel();

	if (objLoader.loadModel(filePath, *model)) //returns true if the model is loaded, puts the model in the model parameter
	{
		cout << "Model: " << filePath << " loaded" << endl;

		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		model->calcCentrePoint();
		//model->centreOnZero();

		model->calcVertNormalsUsingOctree();  //the method will construct the octree if it hasn't already been created.

		//turn on VBO by setting useVBO to true in threeDmodel.cpp default constructor - only permitted on 8 series cards and higher
		model->initDrawElements();
		model->initVBO(myShader);
		model->deleteVertexFaceData();

		return model;
	}
	else
	{
		cout << "Model: " << filePath << "failed to load " << endl;
	}
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system

	glViewport(0,0,width,height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(60.0f, (GLfloat)screenWidth/(GLfloat)screenHeight, 1.0f, 10000.0f);
}
void processKeys()
{
	if (STATE == SimulationState::RUNNING) 
	{
		if (keys['W'])
		{
			car.setSpeed(car.getSpeed() + 60.0f*deltaTime);
			if (car.getSpeed() > 200.0f)
				car.setSpeed(200.0f);
		}
		else if (keys['S'])
		{
			car.setSpeed(car.getSpeed() - 60.0f*deltaTime);
			if (car.getSpeed() < -200.0f)
				car.setSpeed(-200.0f);
		}
		else
		{
			if (car.getSpeed() < 0.0f) {
				car.setSpeed(car.getSpeed() + 50.0f*deltaTime);
				if (car.getSpeed() >= 0) {
					car.setSpeed(0.0f);
				}
			}
			else {
				car.setSpeed(car.getSpeed() - 50.0f*deltaTime);
				if (car.getSpeed() <= 0) {
					car.setSpeed(0.0f);
				}
			}
		}

		if (keys['A'])
		{
			car.turn(1);
		}
		if (keys['D'])
		{
			car.turn(-1);
		}
	}
	

	if (keys['1'])
	{
		camera.setType(Type::FIRST);
	}

	if (keys['2'])
	{
		camera.setType(Type::THIRD);
	}

	if (keys['3'])
	{
		camera.setType(Type::STATIC);
	}

	if (keys['4'])
	{
		camera.setType(Type::ACTION);
	}

	if (keys['5'])
	{
		drawBoundingBoxes = true;
		drawBoundingSpheres = true;
	}

	if (keys['6'])
	{
		drawBoundingBoxes = false;
		drawBoundingSpheres = false;
	}

	if (keys['7'])
	{
		toggleLighting(1);
	}
	
	if (keys['8'])
	{
		toggleLighting(0);
	}

	if (keys['R'])
	{
		resetSimulation();
	}
}

void calculateDeltaTime() 
{
	previousTime = currentTime;
	currentTime = clock();
	deltaTime = (currentTime - previousTime);
	deltaTime = deltaTime / (double)CLOCKS_PER_SEC;	
	update(deltaTime);
	std::cout << "speed: " << car.getSpeed() << std::endl;
}

void handleCollisions()
{
	for each(ThreeDModel* m in buildings) {
		Octree* oct = m->getOctree();
		bool collision = CollisionTest::sphereAABB(car.getBoundingSphere(), oct->getMin(), oct->getMax());
		if (collision) {
			car.respondToCollision(deltaTime);
		}
	}

	// Check if the sphere is OOB (Out of Bounds)
	bool collision = CollisionTest::sphereOOB(car.getBoundingSphere(), mapMinX, mapMinZ, mapMaxX, mapMaxZ);
	if (collision) {
		STATE = SimulationState::ENDED;
		car.setOutOfBounds(true);
		handleOOB();
	}
}

void handleOOB()
{
	if (car.getSpeed() > 0.0f)
		fallAngle += 0.5f;
	else
		fallAngle -= 0.5f;
	car.setPosition(car.getPosition() + glm::vec3(0.0f, -1.0f, 0.0f));
}

void update(double deltaTime)
{
	car.move(deltaTime);
	camera.update();
	handleCollisions();
	if (car.isBusted())
		STATE = SimulationState::ENDED;
}
/**************** END OPENGL FUNCTIONS *************************/

//WIN32 functions
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
void KillGLWindow();									// releases and destroys the window
bool CreateGLWindow(char* title, int width, int height); //creates the window
int WINAPI WinMain(	HINSTANCE, HINSTANCE, LPSTR, int);  // Win32 main function

//win32 global variabless
HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application


/******************* WIN32 FUNCTIONS ***************************/
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	bool	done=false;								// Bool Variable To Exit Loop

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	// Create Our OpenGL Window
	if (!CreateGLWindow("OpenGL Win32 Example",screenWidth,screenHeight))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=true;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			if(keys[VK_ESCAPE])
				done = true;

			processKeys();			//process keyboard
			
			display();					// Draw The Scene
			calculateDeltaTime();		// calculate delta time and call update using delta time
			SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (int)(msg.wParam);						// Exit The Program
}

//WIN32 Processes function - useful for responding to user inputs or other events.
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}
		break;

		case WM_SIZE:								// Resize The OpenGL Window
		{
			reshape(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
		break;

		case WM_LBUTTONDOWN:
			{
	            mouse_x = LOWORD(lParam);          
				mouse_y = screenHeight - HIWORD(lParam);
				LeftPressed = true;
			}
		break;

		case WM_LBUTTONUP:
			{
	            LeftPressed = false;
			}
		break;

		case WM_MOUSEMOVE:
			{
	            mouse_x = LOWORD(lParam);          
				mouse_y = screenHeight  - HIWORD(lParam);
			}
		break;
		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = true;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}
		break;
		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = false;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}
		break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void KillGLWindow()								// Properly Kill The Window
{
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*/
 
bool CreateGLWindow(char* title, int width, int height)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;											// Return FALSE
	}
	
	dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	HGLRC tempContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, tempContext);

	glewInit();

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};
	
    if(wglewIsSupported("WGL_ARB_create_context") == 1)
    {
		hRC = wglCreateContextAttribsARB(hDC,0, attribs);
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hDC, hRC);
	}
	else
	{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		hRC = tempContext;
		cout << " not possible to make context "<< endl;
	}

	//Checking GL version
	const GLubyte *GLVersionString = glGetString(GL_VERSION);

	cout << GLVersionString << endl;

	//OpenGL 3.2 way of checking the version
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	reshape(width, height);					// Set Up Our Perspective GL Screen

	init();
	
	return true;									// Success
}



