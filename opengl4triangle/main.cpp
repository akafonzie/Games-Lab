/// ***
///
///		main.cpp - Co-Authored by Tom & Alice:
///		The applications' main methods are included here, and all objects for use in the game are initialised
///
/// ***

//Inclusion of helper classes from libraries used
#include<stdio.h>
#include <stdlib.h>
#include "include\GL\glew.h"
#define GLM_FORCE_RADIANS //as called for by compiler to remove warning
#include "include\glm\gtc\matrix_transform.hpp"
#include "include\glm\gtc\quaternion.hpp"
#include "include\GLFW\glfw3.h"
#include "include\fmod\inc\fmod.hpp"

//Inclusion of user written classes
#include "shaders.h"
#include "modelLoader.h"
#include "Camera.h"
#include "Player.h"
#include "Enemy.h"
#include "Character.h"
#include "Item.h"
#include "Level.h"
#include "gamePad.h"
#include "sound.h"
#include "oculus.h"


using namespace std;

/// *** Main Method Prototypes (Alphabetical order)
void cleanUp(void);
void fps(GLFWwindow*);
void gameLoop(void);
void winLose(void);
float getDeltaTime();
void gpInput();
void init(void);
void keyPress(GLFWwindow*, int key, int scancode, int action, int mods);
void postProcessFrameBuffer();
void render(void);
void renderEyePatch(OVR::Util::Render::StereoEye eye);
void renderSceneToFrameBuffer(void);
void resize(GLFWwindow*, int w, int h);
void setupFrameBuffer();
void setupGLHints(void);
void setupOccShaders(void);
void setupShaders(void);
void setupWindow(void);
void update();



/// *** Main Method Object Encapsulation
shaders sPointer;
modelLoader mPointer, m2Pointer, m3Pointer, m4Pointer, mLevelPointer, mItemPointer;
Camera *cam;
GLFWwindow* window;
model* leModel, *anotherModel;
Player * player;
Level* theLevel;
gamePad gp = gamePad(1);
sound* leSound;
oculus* theRift;
Enemy *zombie, *zombie2, *zombie3, *zombie4;
Item *item, *item2, *item3, *item4, *item5, *item6, *item7, *item8;
std::vector<Item> items;



bool alice = false, gamepad = true, keyboard = true;
bool debug = false;
//// *** Delta Time Variables- begin
float cTime;
float delta = 0.0f;
float dt;
int tickCount = 0;
bool ti = false;
//// *** Delta Time Variables - end
double start_Time;
int g_gl_w=1280;
int g_gl_h=800;
double cursor_x, cursor_y;
unsigned int gpc;
float rot;
GLuint shaderID, shader2ID, shader3ID, shader4ID, screenShader;
glm::mat4 Projection; 
glm::mat4 View;
bool headMove = true;


/// *** Oculus Related - Begin

struct eyePatch
{
	void setup(OVR::Util::Render::StereoEye eye)
	{

		static float riftVertBufferData[3][4][3]=
		{
			//centre eye
			{	{ -1.0f, -1.0f, 0.0f, },
				{  1.0f, -1.0f, 0.0f, },
				{  1.0f,  1.0f, 0.0f, },
				{ -1.0f,  1.0f, 0.0f, }, 
			},
			//left eye
			{	{ -1.0f, -1.0f, 0.0f, },
				{  0.0f, -1.0f, 0.0f, },
				{  0.0f,  1.0f, 0.0f, },
				{ -1.0f,  1.0f, 0.0f, },
			},
			//right eye
			{	{  0.0f, -1.0f, 0.0f, },
				{  1.0f, -1.0f, 0.0f, },
				{  1.0f,  1.0f, 0.0f, },
				{  0.0f,  1.0f, 0.0f, }, 
			},
		};

		static float riftUVBufferData[3][4][2]=
		{
			//centre eye
			{	{ 0.0f, 0.0f, },
				{ 1.0f, 0.0f, },
				{ 1.0f, 1.0f, },
				{ 0.0f, 1.0f, },  
			},
			//left eye
			{	{ 0.0f, 0.0f, },
				{ 0.5f, 0.0f, },
				{ 0.5f, 1.0f, },
				{ 0.0f, 1.0f, },
			},
			//right eye
			{	{ 0.5f, 0.0f, },
				{ 1.0f, 0.0f, },
				{ 1.0f, 1.0f, },
				{ 0.5f, 1.0f, },
			},
		};

		//load up the eye quad
		glGenVertexArrays(1, &riftVA);
		glBindVertexArray(riftVA);

		//set the VB for the rift eye
		glGenBuffers(1, &riftVB);
		glBindBuffer(GL_ARRAY_BUFFER, riftVB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(riftVertBufferData[eye]), &riftVertBufferData[eye][0][0], GL_DYNAMIC_DRAW);

		//set the UVB for the rift eye
		glGenBuffers(1, &riftUVB);
		glBindBuffer(GL_ARRAY_BUFFER, riftUVB);
		glBufferData(GL_ARRAY_BUFFER, sizeof(riftUVBufferData[eye]), &riftUVBufferData[eye][0][0], GL_DYNAMIC_DRAW);
	}

	void render()
	{
		glBindBuffer(GL_ARRAY_BUFFER, riftVB);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, riftUVB);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_QUADS, 0, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}

	void cleanUp()
	{
		glDeleteBuffers(1, &riftVB);
		glDeleteBuffers(1, &riftUVB);
		glDeleteVertexArrays(1, &riftVA);
	}
	GLuint riftVB;
	GLuint riftVA;
	GLuint riftUVB;
};
eyePatch m_EyePatch[3];
GLuint ocVert, ocFrag;
GLuint frameBuffer = 0;
GLuint frameBufferTexture = 0;
GLuint frameBufferDepth = 0;
GLuint drawBuffers[1];
GLuint ocShaders=0;

/// *** Shader operation methods
void processUniform2f(const char* vn, float a, float b)
	{GLuint varid = glGetUniformLocation(ocShaders, vn); glUniform2f(varid, a, b);}
void processUniform4f(const char* vn, float a, float b, float c, float d)
	{GLuint varid = glGetUniformLocation(ocShaders, vn); glUniform4f(varid, a, b, c, d);}
void processUniform1i(const char* vn, int a)
	{GLuint varid = glGetUniformLocation(ocShaders, vn); glUniform1i(varid, a);}

/// ********** Variables used to edit the oculus parameters via the keyboard!!!!
float xCentOffset = 0.3f, sfEdit = 1.2f, siEdit = 1.0f;

bool useTheRift = false; // *** SET TO TRUE IF RIFT IS CONNECTED AND ROCKING
bool prepareForRift = false; // *** SET TO TRUE TO SET UP RIFT TINGS

/// *** Oculus Related - End

void fps(GLFWwindow*)
{
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if(elapsed_seconds > 0.25)
	{
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		sprintf(tmp, "OpenGL4 | fps: %.0f | Windowed Voo", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count ++;
}

float getDeltaTime()
{
	float nTime = glfwGetTime();
	float fTime = nTime - cTime;
	cTime = nTime; 
	return fTime;	
}

void resize(GLFWwindow*, int w, int h)
{
	g_gl_w=w;
	g_gl_h=h;
}

void keyPress(GLFWwindow*, int key, int scancode, int action, int mods){
	if(GLFW_PRESS != action)
	{
		return;
	}
	switch(key)
	{
			/// *** Oculus Parameter Editing - Begin
	case GLFW_KEY_RIGHT_BRACKET: // ]: Increases the XcenterOffset
		xCentOffset += 0.1f;
		break;
	case GLFW_KEY_LEFT_BRACKET:	 // [: Decreases the XcenterOffset
		xCentOffset -= 0.1f;
		break;
	case GLFW_KEY_O:			 // O: Decreases the Overall Scale factor
		sfEdit -= 0.1f;
		break;
	case GLFW_KEY_P:			 // P: Increases the Overall Scale factor
		sfEdit += 0.1f; 
		break;
	case GLFW_KEY_K:			 // K: Increases the ScaleIn Factor
		siEdit -= 0.1f;
		break;
	case GLFW_KEY_L:			 // L: Decreases the ScaleIn Factor
		siEdit += 0.1f;
		break;
			/// *** Oculus Parameter Editing - End
	case GLFW_KEY_T:
		if(!debug)
			debug = true;
		else
			debug = false;
		break;
	case GLFW_KEY_Z:
		theLevel->vbp+=1;
		cout<<theLevel->vbp<<endl;
		break;
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, 1);
		glfwTerminate();
		std::exit(0);
		break;
	default:
		break;
	}
}

void setupWindow(void)
{
	glfwInit();
	char message [256];
	sprintf(message, "Starting GLFW &s", glfwGetVersionString());
	if(!glfwInit())
	{
		fprintf(stderr, "Error: Could not start GLFW3\n");
	}
	if(useTheRift)
	{
		const OVR::HMDInfo& hmdinf = theRift->getHMDInfo();
		window = glfwCreateWindow(hmdinf.HResolution, hmdinf.VResolution, "Games Lab, Oculus", NULL, NULL);
		glfwSetWindowPos(window, hmdinf.DesktopX, hmdinf.DesktopY);
	}
	else
	{
		window = glfwCreateWindow (1280, 800, "OpenGL 4 | Windowed Voo", NULL, NULL); //Windowed Mode
	}
	/// ** Just error check that the window has correctly been created, else exit
	if (!window)
	{
		fprintf (stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		std::exit(0);
	}
	glfwMakeContextCurrent (window);
	glfwSetWindowSizeCallback(window, resize);
	glfwSetKeyCallback(window, keyPress);
	
	const GLubyte* renderer = glGetString (GL_RENDERER);
	const GLubyte* version = glGetString (GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL Version supported %s\n", version);

	if(!alice)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4); //Multi sample Anti Aliasing
		GLFWmonitor* mon=glfwGetPrimaryMonitor();
	}
}

void setupGLHints(void)
{
	/// ** make dem tings look nice - begin

	// *** SHADING
	glShadeModel	(GL_SMOOTH);
	glEnable		(GL_NORMALIZE);
	glHint			(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	// *** CULLING
	glPolygonMode	(GL_FRONT_AND_BACK, GL_FILL);
	glEnable		(GL_CULL_FACE);
	//glFrontFace		(GL_CW);
	glCullFace		(GL_BACK);
	glEnable		(GL_DEPTH_TEST);
	glDepthFunc		(GL_LESS);
	glDepthMask		(GL_TRUE);
	glDepthFunc		(GL_LEQUAL);

	// *** BLENDING
	glEnable		(GL_BLEND);
	glBlendFunc		(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/// ** make dem tings look nice - end
}

void init(void)
{

	start_Time = GetTickCount();
	if(prepareForRift)
	{
		OVR::System::Init();
		theRift = new oculus();
		theRift->init();
	}
	setupWindow();
	setupGLHints();
	glewInit();

	/// *** Initialise all the objects used in the scene
	leSound = new sound();
	leSound->init();
	leSound->loadSounds();
	
	/// *** Draw Models Start
	player = new Player();
	if(prepareForRift)
		player->start(theRift);
	theLevel	= new Level(mLevelPointer.loadModel("models/level2/2004.obj"));
	zombie		= new Enemy(mPointer.loadModel("models/FemmeWalkRunAttack.dae"));
		zombie2		= new Enemy(zombie->myModel);
	zombie3		= new Enemy(zombie->myModel);
	zombie4		= new Enemy(zombie->myModel);
	item = new Item(mItemPointer.loadModel("models/Chest/chest.dae"));
	item2 = new Item(item->myModel); item3 = new Item(item->myModel); item4 = new Item(item->myModel);
	item5 = new Item(item->myModel); item6 = new Item(item->myModel); item7 = new Item(item->myModel);
	item8 = new Item(item->myModel);
	items.push_back(*item); items.push_back(*item2); items.push_back(*item3); items.push_back(*item4);
	items.push_back(*item5); items.push_back(*item6); items.push_back(*item7); items.push_back(*item8);
	
	player->setCentre(0.0, 1.5*6, 0.0);

	glm::vec3 zomb = mPointer.getCentre(zombie->myModel);//get original centre point from model's verts
	zombie->setCentre(zomb.x+2.0, zomb.z+40.0, zomb.y-80.0); //set the centre
	zombie->setUpZombie(glm::vec3(0.2, 0.2, 0.2), glm::vec3(-1400.0, 40.0, 1.0)); //scale and translate
	
	glm::vec3 zomb2 = m2Pointer.getCentre(zombie2->myModel);	
	zombie2->setCentre(zomb2.x+2.0, zomb2.z+40.0, zomb2.y-80.0);
	zombie2->setUpZombie(glm::vec3(0.2, 0.2, 0.2), glm::vec3(-1200, 80, 1.0));

	glm::vec3 zomb3 = m3Pointer.getCentre(zombie3->myModel);	
	zombie3->setCentre(zomb3.x+2.0, zomb3.z+40.0, zomb3.y-80.0);
	zombie3->setUpZombie(glm::vec3(0.2, 0.2, 0.2), glm::vec3(-2500, -360, 1.0));

	glm::vec3 zomb4 = m4Pointer.getCentre(zombie4->myModel);	
	zombie4->setCentre(zomb4.x+2.0, zomb4.z+40.0, zomb4.y-80.0);
	zombie4->setUpZombie(glm::vec3(0.2, 0.2, 0.2), glm::vec3(-2700, 1800, 1.0));
	
	glm::vec3 levCentre = mLevelPointer.getCentre(theLevel->levelModel);
	theLevel->levelModel->ModelView = glm::mat4(1.0);	
	theLevel->makeCollPlanes();

	item->initRand();

	//initialise the items, put them in different rooms
	for(int i=0; i<items.size(); i++){
		int room;
		if(i == 0)           room = 1;
		if(i == 1 || i == 2) room = 2;
		if(i == 3)           room = 3;
		if(i == 4 || i == 5) room = 4;
		if(i == 6 || i == 7) room = 5;
		glm::vec3 itemvec = mItemPointer.getCentre(items.at(i).myModel);
		items.at(i).setCentre(itemvec.x, itemvec.z, itemvec.y);
		items.at(i).initItem(glm::vec3(0.0, 0.0, 5.0), theLevel, room); 
	}


	/// *** Draw Models End

	cam = new Camera(player);
	if(useTheRift)
		Projection = glm::perspective(45.0f, (GLfloat)theRift->hmdInfo.HResolution / (GLfloat)theRift->hmdInfo.VResolution, 0.1f, 1000.0f);
	else
		Projection = glm::perspective(45.0f, (GLfloat)g_gl_w / (GLfloat)g_gl_h, 0.1f, 1000.0f);
}

void gpInput()
{
	float t = 0.3f; //threshold for thumbsticks to account for my broken-ass gamepad  '~( ^_^)~'
		//Buttons
	if(gp.bPress(xButtons.A)){printf("gamepad a pushed\n");}
	if(gp.bPress(xButtons.B)){printf("gamepad b pushed\n");}
	if(gp.bPress(xButtons.X)){printf("gamepad x pushed\n");}
	if(gp.bPress(xButtons.Y)){printf("gamepad y pushed\n");}

	if(gp.bPress(xButtons.dPad.up)){
		//actually down.
		player->changeTorchDir(5);
	}
	if(gp.bPress(xButtons.dPad.down)){
		//actually up
		player->changeTorchDir(-5);
	}
	
	player->turnSpeed = player->deltaTurnSpeed;

	if(gp.bPress(xButtons.rShoulder)){
		headMove = true;
		player->left = false;
		player->right = false;
	}
	else{
		if(headMove){
			cam->reInitialise(player);
		}
		//then reinitialise the camera again so it can follow as normal
		headMove = false;
	}

	if(gp.lTrigger() > 0.0 && gp.lTrigger() <= 1.0)
	{
		player->speed = player->deltaCrouchSpeed;
		leSound->walk1->setFrequency(40000.0f);
		player->setCentre(player->centre.x, 1.5*4, player->centre.z);
	} else if(gp.rTrigger() > 0.0 && gp.rTrigger() <= 1.0)
	{
		player->speed = player->deltaSprintSpeed;
		leSound->walk1->setFrequency(80000.0f);
	} else
	{
		player->setCentre(player->centre.x, 1.5*8, player->centre.z);
		player->speed = player->deltaSpeed;
		leSound->walk1->setFrequency(60000.0f);
	}

	//Left thumbstick 
	if(gp.lStickY() >= t)
	{
		player->setForward(true);
		player->setBackward(false);
		leSound->walk1->setPaused(false);
	}
	else if(gp.lStickY() <= -t)
	{
		player->setForward(false);
		player->setBackward(true);
		leSound->walk1->setPaused(false);
	}else{
		player->setBackward(false);
		player->setForward(false);
		leSound->walk1->setPaused(true);
	}
	
	if(gp.lStickX() >= t*1.5)
	{
		player->setStrafingL(true);
	}else if (gp.lStickX() <= -t*1.5)
	{
		player->setStrafingR(true);
	}else{
		player->setStrafingL(false);
		player->setStrafingR(false);
	}


	//Right Thumbstick
	if(gp.rStickX() > t)
	{
		if(!headMove){
			player->setLeft(false);
			player->setRight(true);
		}
		player->setLookLeft(false);
		player->setLookRight(true);
		
	}
	else{
		player->setLookRight(false);
		player->setRight(false);
	}

	if(gp.rStickX() < -t)
	{
		if(!headMove){
			player->setLeft(true);
			player->setRight(false);
		}
		player->setLookLeft(true);
		player->setLookRight(false);
		
	}
	else{
		player->setLookLeft(false);
		player->setLeft(false);
	}


	if(gp.rStickY() > t)
	{
		player->setLookUp(false);
		player->setLookDown(true);
	}
	else{
		player->setLookDown(false);
	}
	if(gp.rStickY() < -t)
	{
		player->setLookUp(true);
		player->setLookDown(false);
	}
	else{
		player->setLookUp(false);
	}
	
	
}

void update(){
	//updateSpeeds(); // update the speeds to reflect delta time

	gpInput();
	gp.update();
	gp.refreshState();
	delta = getDeltaTime();

	player->Move();

	float RunningTime = (float)(GetTickCount() - start_Time) / 1000.0f;

	int f = 0;
	for(int i=0; i< items.size(); i++){
		items.at(i).checkFound(player, &gp, RunningTime);
		if(items.at(i).found) f++;
	}
	if(f == items.size()){
		player->WIN = true;
	}

	//clear the bone transformation vectors
	zombie->Transforms.clear();
	zombie2->Transforms.clear();
	zombie3->Transforms.clear();
	zombie4->Transforms.clear();

	//calculate the bone transforms relative to the animation time
	mPointer.boneTransform(RunningTime, zombie->Transforms, zombie->anim, zombie->animtime);
	mPointer.boneTransform(RunningTime, zombie2->Transforms, zombie2->anim, zombie2->animtime); 
	mPointer.boneTransform(RunningTime, zombie3->Transforms, zombie3->anim, zombie3->animtime);
	mPointer.boneTransform(RunningTime, zombie4->Transforms, zombie4->anim, zombie3->animtime);

	//pass the diameters of the ciruit the zombies move in to their move methods
	zombie->Move(300.0, 150.0, player, &gp, RunningTime);
	zombie2->Move(150.0, 300.0, player, &gp, RunningTime);
	zombie3->Move(150.0, 150.0, player, &gp, RunningTime);
	zombie4->Move(600.0, 150.0, player, &gp, RunningTime);

	//check collisions with each other and the level
	zombie->checkCollision(zombie2); zombie->checkCollision(zombie3);
	zombie->checkCollision(zombie4); zombie2->checkCollision(zombie3);
	zombie2->checkCollision(zombie4); zombie3->checkCollision(zombie4);
	zombie->checkLvlCollision(theLevel); zombie2->checkLvlCollision(theLevel);
	zombie3->checkLvlCollision(theLevel); zombie4->checkLvlCollision(theLevel);

	player->checkCollision(theLevel);

	//update their speeds to use delta time
	if(delta < 1.0){
		zombie->updateDeltaSpeed(delta);
		zombie2->updateDeltaSpeed(delta);
		zombie3->updateDeltaSpeed(delta);
		zombie4->updateDeltaSpeed(delta);
		player->updateDeltaSpeed(delta);
	}

	if(headMove){
		cam->updateHead(player);
	}
	else{
		cam->update(player); //this one is supposed to follow the character with WASD
	}
		
	leSound->update();
	if(prepareForRift)
	{
		//Get the pitch, roll and yaw from the sensorFusion Device
		glm::vec3 eularAngles;
		theRift->getSensorOrientation(eularAngles.y, eularAngles.x, eularAngles.z);
		//negate the values so that they correspond to rift movement correctly
		eularAngles.x = -eularAngles.x;
		eularAngles.y = -eularAngles.y;
		eularAngles.z = -eularAngles.z;
		//convert it into a GLM quaternion
		glm::quat orient = glm::quat(eularAngles);
		//take the camera position and apply the orientation transformation to it!
		View =  glm::mat4_cast(orient) * glm::lookAt(
				glm::vec3(cam->eyex, cam->eyey, cam->eyez), // eye
				glm::vec3(cam->atx, cam->aty, cam->atz), // at
				glm::vec3(cam->upx, cam->upy, cam->upz)  // up
				);
	
	}
	else
	{
		View= glm::lookAt(
				glm::vec3(cam->eyex, cam->eyey, cam->eyez), // eye
				glm::vec3(cam->atx, cam->aty, cam->atz), // at
				glm::vec3(cam->upx, cam->upy, cam->upz)  // up
				);
	}
	
	if(!prepareForRift)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glViewport(0, 0, g_gl_w, g_gl_h);
	}
}

void setupFrameBuffer()
{
	//set up frame buffer, resolution should be set to at least the oculus resolution and prefarbly higher
	GLsizei width;
	GLsizei height;
	if(prepareForRift)
	{
	width	= theRift->getHMDInfo().HResolution;
	height	= theRift->getHMDInfo().VResolution;
	}else
	{
		width	= 1280;
		height	= 800;
	}
	glGenFramebuffers(1, &frameBuffer);


	//the texture we will render to
	glGenTextures(1, &frameBufferTexture);

	//Bind the newly created texture, all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

		//linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Give an empty image to openGL (the last "0")
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	//set renderedtexture as colour attachment #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

	//the depth buffer
	glGenRenderbuffers(1, &frameBufferDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, frameBufferDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBufferDepth);

	//set the list of draw buffers
	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers); //"1" is the size of DrawBuffers

	
	//always check that framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		assert("[FRAMEBUFFER] error");
		glfwTerminate();
		exit(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderSceneToFrameBuffer(void)
{
	glPushAttrib(GL_TEXTURE_BIT | GL_DEPTH_TEST | GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

	//render to the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPopAttrib();
}

void renderEyePatch(OVR::Util::Render::StereoEye eye)
{
	const OVR::Util::Render::StereoEyeParams& p = theRift->getStereoConfig().GetEyeRenderParams(eye);
	if(p.pDistortion)
	{
		GLsizei width	= theRift->getHMDInfo().HResolution;
		GLsizei height	= theRift->getHMDInfo().VResolution;
		OVR::Util::Render::DistortionConfig dist = *p.pDistortion;

		float	w = float(p.VP.w) / float(width),
				h = float(p.VP.h) / float(height), 
				x = float(p.VP.x) / float(width), 
				y = float(p.VP.y) / float(height);
		float	as = float(p.VP.w) / float(p.VP.h) * siEdit;

		//we are using 1/4 of Distortion centre offset value, since it is relative
		//to the [-1, 1] range that gets mapped to [0, 0.5]
		float hmdWarpParam[4]	= {dist.K[0], dist.K[1], dist.K[2], dist.K[3]};
		float chromAbParam[4]	= {dist.ChromaticAberration[0], dist.ChromaticAberration[1],
								   dist.ChromaticAberration[2], dist.ChromaticAberration[3]};
		float scaleFactor		= 1.0f / dist.Scale * sfEdit;
		
		// xcentoff = 0.151976421 | w = 0.5 |  h = 1.0 | x = 0 | y = 0| 
		if(eye == OVR::Util::Render::StereoEye::StereoEye_Right)
		{
			dist.XCenterOffset = -dist.XCenterOffset;
		}
		OVR::Vector2f lensCenter	(x + (w + (dist.XCenterOffset * xCentOffset)* 0.5f) * 0.5f, y + h * 0.5f);
		OVR::Vector2f screenCenter	(x + w * 0.5f,								 y + h * 0.5f);
		OVR::Vector2f scale			((w / 2) * scaleFactor,						 (h / 2) * scaleFactor * as);
		OVR::Vector2f scaleIn		((2 / w ),									 (2 / h) / as);	

		//send these delicious variables to the ocFrag shader 

		processUniform2f("LensCenter",		lensCenter.x,	lensCenter.y); 
		processUniform2f("ScreenCenter",	screenCenter.x, screenCenter.y);
		processUniform2f("Scale",			scale.x,		scale.y);
		processUniform2f("ScaleIn",			scaleIn.x,		scaleIn.y);
		processUniform4f("HmdWarpParam",	hmdWarpParam[0], hmdWarpParam[1], hmdWarpParam[2], hmdWarpParam[3]);
		processUniform4f("ChromAbParam",	chromAbParam[0], chromAbParam[1], chromAbParam[2], chromAbParam[3]);
		
	}
	m_EyePatch[eye].render();
	
}

void postProcessFrameBuffer()
{
	
	///***Send the 3D Screen Render to the display, and apply the post process shaders
	glPushAttrib(GL_TEXTURE_BIT | GL_DEPTH_TEST | GL_LIGHTING);
	glDisable(GL_CULL_FACE); 
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	//render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//reset the screen parameters
	glViewport(0, 0, theRift->getHMDInfo().HResolution, theRift->getHMDInfo().VResolution);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//set up the post process shader
	glUseProgram(ocShaders);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	processUniform1i("texture0", 0);

	//render the left and right eyes with the distortion shader
	if(prepareForRift){
		renderEyePatch(OVR::Util::Render::StereoEye_Left);
		renderEyePatch(OVR::Util::Render::StereoEye_Right);
	}
	
	//clean up
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(ocShaders);
	glPopAttrib();

	//swap dems buffers
	glfwSwapBuffers(window);
}

void setupShaders(void)
{
	shaderID = sPointer.loadShaders("shaders/debugvert.glsl", "shaders/debugfrag.glsl");
	shader2ID = sPointer.loadShaders("shaders/dasvert.glsl","shaders/dasfrag.glsl"); 
	//shader2ID = sPointer.loadShaders("shaders/vs2.glsl","shaders/fs2.glsl"); //tom's one with spec&atten
	shader3ID = shader2ID;
	shader4ID = shader2ID;
	screenShader = sPointer.loadShaders("shaders/screensVert.glsl", "shaders/screensFrag.glsl");
	/// ** Oculus shader ting
	if(prepareForRift)
		setupOccShaders();
}

void setupOccShaders(void)
{
		//setup the eye quads
	for(int i = 0; i < 3; i++)
	{
		m_EyePatch[i].setup((OVR::Util::Render::StereoEye) i);
	}

	/// *** Oculus Vert Shader
	const char* ocVertShader=
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec3 Position;\n"
		"layout(location = 1) in vec2 TexCoord;\n"
		"out vec2 oTexCoord;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(Position, 1);\n"
		"   oTexCoord = TexCoord;\n"
		"};\n";

	/// *** Oculus Frag Shader
	const char* ocFragShader=
		"uniform vec2 LensCenter;\n"
		"uniform vec2 ScreenCenter;\n"
		"uniform vec2 Scale;\n"
		"uniform vec2 ScaleIn;\n"
		"uniform vec4 HmdWarpParam;\n"
		"uniform vec4 ChromAbParam;\n"
		"uniform sampler2D Texture0;\n"
		"varying vec2 oTexCoord;\n"
		"\n"
		// Scales input texture coordinates for distortion.
		// ScaleIn maps texture coordinates to Scales to ([-1, 1]), although top/bottom will be
		// larger due to aspect ratio.
		"void main()\n"
		"{\n"
		"   vec2  theta = (oTexCoord - LensCenter) * ScaleIn;\n" // Scales to [-1, 1]
		"   float rSq= theta.x * theta.x + theta.y * theta.y;\n"
		"   vec2  theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + "
		"                  HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);\n"
		"   \n"
		"   // Detect whether blue texture coordinates are out of range since these will scaled out the furthest.\n"
		"   vec2 thetaBlue = theta1 * (ChromAbParam.z + ChromAbParam.w * rSq);\n"
		"   vec2 tcBlue = LensCenter + Scale * thetaBlue;\n"
		"   if (!all(equal(clamp(tcBlue, ScreenCenter-vec2(0.25,0.5), ScreenCenter+vec2(0.25,0.5)), tcBlue)))\n"
		"   {\n"
		"       gl_FragColor = vec4(0);\n"
		"       return;\n"
		"   }\n"
		"   \n"
		"   // Now do blue texture lookup.\n"
		"   float blue = texture2D(Texture0, tcBlue).b;\n"
		"   \n"
		"   // Do green lookup (no scaling).\n"
		"   vec2  tcGreen = LensCenter + Scale * theta1;\n"
		"   vec4  center = texture2D(Texture0, tcGreen);\n"
		"   \n"
		"   // Do red scale and lookup.\n"
		"   vec2  thetaRed = theta1 * (ChromAbParam.x + ChromAbParam.y * rSq);\n"
		"   vec2  tcRed = LensCenter + Scale * thetaRed;\n"
		"   float red = texture2D(Texture0, tcRed).r;\n"
		"   \n"
		"   gl_FragColor = vec4(red, center.g, blue, 1);\n"
		"}\n";

	//now the shaders
	ocVert = glCreateShader(GL_VERTEX_SHADER);
	ocFrag = glCreateShader(GL_FRAGMENT_SHADER);
	GLint result = GL_FALSE;
	int infoLogLength;
	//compile the vertex shader
	printf("Compiling the Oculus Vertex Shader!\n");
	glShaderSource(ocVert, 1, &ocVertShader, NULL);
	glCompileShader(ocVert);
	//check the vert shader
	glGetShaderiv(ocVert, GL_COMPILE_STATUS, &result);
	glGetShaderiv(ocVert, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0)
	{
		vector<char> vertShaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(ocVert, infoLogLength, NULL, &vertShaderErrorMessage[0]);
		printf("ocVert: %s\n", &vertShaderErrorMessage[0]);
	}

	//compile the frag shader
	printf("Compiling the Oculus Fragment Shader!\n");
	glShaderSource(ocFrag, 1, &ocFragShader, NULL);
	glCompileShader(ocFrag);
	//check the vert shader
	glGetShaderiv(ocFrag, GL_COMPILE_STATUS, &result);
	glGetShaderiv(ocFrag, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0)
	{
		vector<char> vertShaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(ocVert, infoLogLength, NULL, &vertShaderErrorMessage[0]);
		printf("ocFrag: %s\n", &vertShaderErrorMessage[0]);
	}

	//link the shaders
	printf("Linking Oculus Shaders!\n");
	ocShaders = glCreateProgram();
	glAttachShader(ocShaders, ocVert);
	glAttachShader(ocShaders, ocFrag);
	glLinkProgram(ocShaders);

	//check the ocShader program
	glGetShaderiv(ocShaders, GL_COMPILE_STATUS, &result);
	glGetShaderiv(ocShaders, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0)
	{
		vector<char> vertShaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(ocVert, infoLogLength, NULL, &vertShaderErrorMessage[0]);
		printf("ocShaders: %s\n", &vertShaderErrorMessage[0]);
	}
	glDeleteShader(ocVert);
	glDeleteShader(ocFrag);
	printf("ocShaders = %i\n", ocShaders);
}

void render(void)
{
	if(debug){
	sPointer.useShader(shaderID, View, Projection);
	theLevel->renderCollPlanes();
	glUseProgram(0);

	sPointer.useShader(shaderID, View, Projection);
	zombie->drawPoints();
	glUseProgram(0);

	sPointer.useShader(shaderID, View, Projection);
	zombie2->drawPoints();
	glUseProgram(0);

	sPointer.useShader(shaderID, View, Projection);
	zombie3->drawPoints();
	glUseProgram(0);

	sPointer.useShader(shaderID, View, Projection);
	zombie4->drawPoints();
	glUseProgram(0);
	}

	sPointer.useShader(shader4ID, theLevel->levelModel->ModelView, View, Projection, 
		1, player->caught, player->torchPos, player->torchDir, zombie, &items.at(0));
	mLevelPointer.renderModel(theLevel->levelModel);
	glUseProgram(0);

	sPointer.useShader(shader2ID, zombie->ModelView, View, Projection, 
		0, player->caught, player->torchPos, player->torchDir, zombie, &items.at(0));
	mPointer.renderModel(zombie->myModel);
	glUseProgram(0);

	sPointer.useShader(shader2ID, zombie2->ModelView, View, Projection, 
		0, player->caught, player->torchPos, player->torchDir, zombie2, &items.at(0));
	m2Pointer.renderModel(zombie2->myModel);
	glUseProgram(0);

	sPointer.useShader(shader2ID, zombie3->ModelView, View, Projection, 
		0, player->caught, player->torchPos, player->torchDir, zombie3, &items.at(0));
	m3Pointer.renderModel(zombie3->myModel);
	glUseProgram(0);

	sPointer.useShader(shader2ID, zombie4->ModelView, View, Projection, 
		0, player->caught, player->torchPos, player->torchDir, zombie4, &items.at(0));
	m4Pointer.renderModel(zombie4->myModel);
	glUseProgram(0);

	//treasure chests
	for(int i=0; i < items.size(); i++){
			sPointer.useShader(shader2ID, items.at(i).ModelView, View, Projection, 
				2, player->caught, player->torchPos, player->torchDir, zombie, &items.at(i));
			mItemPointer.renderModel(items.at(i).myModel);
			glUseProgram(0);

	}

}

void winLose(void){
	if(player->WIN){
		printf("\n\nYou Won!!\n\n");

	}
	if(player->LOSE){
		printf("\n\nYou Lost!!\n\n");
	}
}

void gameLoop(void)
{
	leSound->amb1->setPaused(false);
	while(!glfwWindowShouldClose(window) && !player->WIN && !player->LOSE)
	{
		fps(window);
		if(prepareForRift)
			renderSceneToFrameBuffer();
		update();
		render(); 
		if(prepareForRift)
			postProcessFrameBuffer();
		glfwPollEvents();
		if(!prepareForRift)
			glfwSwapBuffers(window);
	}
}

void cleanUp()
{
	/// *** do all the cleaning up in here, delete shiz and whatnot
}

int main()
{
	init();
	setupShaders();
	if(prepareForRift)
		setupFrameBuffer();
	gameLoop();
	cleanUp();
	winLose();
	glfwTerminate();
	return 0;
}
