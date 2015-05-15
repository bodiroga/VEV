#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "tools.h"
#include "trfm3D.h"
#include "scene.h"
#include "node.h"
#include "gObjectManager.h"
#include "textureManager.h"
#include "materialManager.h"
#include "shaderManager.h"
#include "lightManager.h"
#include "cameraManager.h"
#include "avatar.h"
#include "scenes.h"
#include "skybox.h"

static float step = 0.5;
static float angle_step = 1.0f * DEGREE_TO_RAD;
static float fovy = 30.0;

static int mouseX = 0;
static int mouseY = 0;

static Light * mainLight;
static Light * spLight;
static Light * dirLight;
static Light * dirLight2;

static int mainLight_s;
static int dirLight_s;
static int dirLight2_s;
static int spLight_s;

static Node *displayNode;

static GObject *sky;

static camera *theCamera;
static int     check_cull = 0;
static Avatar *theAvatar;
static int     theAvatar_mode;

// Animation settings

// The time in milliseconds between timer ticks
static int AGA_TIMERMSECS = 33;
static int runAnimation = 0;

// Global variables for measuring time (in milli-seconds)
static int startTime;
static int prevTime;

// Given a camera, put its the projection matrix in Render State stack

static void update_rs_proj_matrix(camera *theCamera) {

	RenderState *rs;

	rs = RenderStateScene();
	LoadTrfmRS(rs, AGA_PROJECTION, CameraProjectionTrfm(theCamera));

}

void InitLight() {

	static float pos[4] = {0, 10, -10, 1}; //position (the final 1 means the Light is positional)
	static float amb[4] = {0.6, 0.6, 0.6, 1};  //ambient
	static float dif[4] = {0.5, 0.5, 0.5, 1};   //diffuse
	static float spec[4] = {0.6, 0.6, 0.6, 1};  //specular

	static float pos_dir[4] = {-1, -1, -1, 0}; //position (the final 0 means the Light is directional)
	static float amb_dir[4] = {0.6, 0.6, 0.6, 1};  //ambient
	static float dif_dir[4] = {1, 0, 0, 1};   //diffuse
	static float spec_dir[4] = {0.6, 0.6, 0.6, 1};  //specular

	static float pos_dir2[4] = {1, 1, 1, 0}; //position (the final 0 means the Light is directional)
	static float amb_dir2[4] = {0, 1, 0, 1};  //ambient
	static float dif_dir2[4] = {1, 0, 0, 1};   //diffuse
	static float spec_dir2[4] = {0.6, 0.6, 0.6, 1};  //specular


	static float pos_sp[4] = {0, 10, 0, 1}; //position (the final 1 means the Light is positional)
	static float dir_sp[4] = {0, 0, -1}; //direction
	static float dif_sp[4] = {1, 1, 1, 1};   //diffuse
	static float cutoff_sp = 10.0;
	static float exp_sp = 40;

	mainLight = SceneRegisterLight("mainlight");
	SetPositionLight(mainLight, &pos[0]);
	SetAmbientLight(mainLight, &amb[0]);
	SetDiffuseLight(mainLight, &dif[0]);
	SetSpecularLight(mainLight, &spec[0]);
	SwitchLight(mainLight, 1);
	mainLight_s = 1;

	dirLight = SceneRegisterLight("dirlight");
	SetPositionLight(dirLight, &pos_dir[0]);
	SetAmbientLight(dirLight, &amb_dir[0]);
	SetDiffuseLight(dirLight, &dif_dir[0]);
	SetSpecularLight(dirLight, &spec_dir[0]);
	SwitchLight(dirLight, 0);
	dirLight_s = 0;

	dirLight2 = SceneRegisterLight("dirlight2");
	SetPositionLight(dirLight2, &pos_dir2[0]);
	SetAmbientLight(dirLight2, &amb_dir2[0]);
	SetDiffuseLight(dirLight2, &dif_dir2[0]);
	SetSpecularLight(dirLight2, &spec_dir2[0]);
	SwitchLight(dirLight2, 0);
	dirLight2_s = 0;

	spLight = SceneRegisterLight("spot1");
	SetSpotLight(spLight, &dir_sp[0], cutoff_sp, exp_sp);
	SetPositionLight(spLight, &pos_sp[0]);
	SetDiffuseLight(spLight, &dif_sp[0]);
	SwitchLight(spLight, 0);
	spLight_s = 0;

}

void InitCamera(int Width, int Height) {

	theCamera = CreateOriginCamera();
	SetCamera(theCamera,
			  0, 0, 0,   // position
			  0, 0, -1,  // look-at
			  0, 1, 0);  // up vector
	InitConicCamera(theCamera, fovy * DEGREE_TO_RAD, (float)Width / (float) Height, 0.1f, 2500.0f);

	theAvatar = CreateAvatar(theCamera, 1.0);
	theAvatar_mode = WalkOrFlyAvatar(theAvatar, 0); // Fly mode
	update_rs_proj_matrix(theCamera);
}

static void InitShaders() {

	RegisterShaderScene("dummy", "Shaders/dummy.vert", "Shaders/dummy.frag");
	RegisterShaderScene("pervertex", "Shaders/pervertex.vert", "Shaders/pervertex.frag");
	RegisterShaderScene("pervertex_notex", "Shaders/pervertex.vert", "Shaders/dummy.frag");
	RegisterShaderScene("perfragment", "Shaders/perfragment.vert", "Shaders/perfragment.frag");
	RegisterShaderScene("perfragment_notex", "Shaders/perfragment.vert", "Shaders/perfragment_notex.frag");
	RegisterShaderScene("bump", "Shaders/bump_shader.vert", "Shaders/bump_shader.frag");
	RegisterShaderScene("sky", "Shaders/sky.vert", "Shaders/sky.frag");
}


void InitGL(int Width, int Height) {          // We call this right after our OpenGL window is created.

	// Init lights
	InitLight();

	// set OpenGL state values
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glViewport(0, 0, Width, Height);              // Reset The Current Viewport And Perspective Transformation

	InitCamera(Width, Height);

	// Enable culling

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Turn Depth Testing On
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f); // Also, sets GLSL fragmen shader gl_DepthRange variable

	// Aliasing

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	InitShaders();
}

static void check_cull_camera() {

	camera *mapC;
	RenderState *rs;

	mapC = SceneFindCamera("mapC");

	if (!mapC) {
		mapC = SceneRegisterCamera("mapC");
		InitConicCamera(mapC, fovy * DEGREE_TO_RAD, 900.0 / 700.0, 0.1f, 1500.0f);
		update_rs_proj_matrix(mapC);
	}
	SetCamera(mapC,
			  theCamera->Ex, 500, theCamera->Ez,
			  theCamera->Ex, 0, theCamera->Ez,
			  -theCamera->Dx, 0, -theCamera->Dz);
	rs = RenderStateScene();
	LoadIdentityRS(rs, AGA_MODELVIEW);
	MultTrfmRS(rs, AGA_MODELVIEW, ViewTrfmCamera(mapC));
}


static void Resize(int Width, int Height) {

	if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
		Height=1;

	SetCameraAspectRatio(theCamera, Width, Height);
	update_rs_proj_matrix(theCamera);
	glViewport(0, 0, (GLsizei) Width, (GLsizei) Height);
}


static void Display(void) {

	RenderState *rs;

	// draw the background color
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	rs = RenderStateScene();

	LoadIdentityRS(rs, AGA_MODELVIEW);
	MultTrfmRS(rs, AGA_MODELVIEW, ViewTrfmCamera(theCamera));

	PlaceSceneLight(SceneGetLight("mainlight"));
	PlaceSceneLight(SceneGetLight("dirlight"));
	PlaceSceneLight(SceneGetLight("dirlight2"));
	PlaceSceneLight(SceneGetLight("spot1"));

	UpdateCullNode(RootNodeScene(), theCamera);
	if (check_cull) check_cull_camera();
	DrawScene();

	glutSwapBuffers();
}


// Keyboard dispatcher when ALT key is pressed
static void Keyboard_alt(unsigned char key) {

	static int line_aliasing = 1;
	RenderState *rs;


	switch(key)
		{
		case 'a':
			line_aliasing = 1 - line_aliasing;
			if (line_aliasing) {
				glEnable(GL_LINE_SMOOTH);
			} else {
				glDisable(GL_LINE_SMOOTH);
			}
			break;
		case 'm':
			PrintRegisteredMaterials();
			break;
		case 't':
			PrintRegisteredTextures();
			break;
		case 'c':
			PrintCamera(theCamera);
			break;
		case 'l':
			PrintRegisteredLights();
			break;
		case 'i':
			PrintRegisteredImages();
			break;
		case 'v':
			rs = RenderStateScene();
			PrintTopRS(rs, AGA_MODELVIEW);
			break;
		case 'p':
			rs = RenderStateScene();
			PrintTopRS(rs, AGA_PROJECTION);
			break;
		case 'b':
			rs = RenderStateScene();
			SetBBoxDrawRS(rs, !GetBBoxDrawRS(rs));
			break;
		case '2':
			check_cull = 1 - check_cull;
			break;
		}
	glutPostRedisplay( );
}

// General keyboard dispatcher
static void Keyboard (unsigned char key, int x, int y) {

	static size_t i = 0;
	int key_mod;
	Texture *tex;

	key_mod = glutGetModifiers();
	if (key_mod == GLUT_ACTIVE_ALT) {
		// If ALT key pressed, call Keyboard_alt and exit
		Keyboard_alt(key);
		return;
	}

	switch(key)
		{
		case '0':
			runAnimation = !runAnimation;
			break;
		case '2':
			displayNode = ParentNode(displayNode);
			break;
		case '3':
			displayNode = FirstChildNode(displayNode);
			break;
		case '4':
			displayNode = NextSiblingNode(displayNode);
			break;
		case '5':
			tex = SceneFindTexture("./obj/cubes/brick.jpg", AGA_TEX_TEX);
			if (tex) {
				CycleTextureMagFilter(tex);
			}
			break;
		case '6':
			tex = SceneFindTexture("./obj/cubes/brick.jpg", AGA_TEX_TEX);
			if (tex) {
				CycleTextureMinFilter(tex);
			}
			break;
		case 's':
			// Enable Shading
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case 'S':
			// Disable Shading
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case 'l':
			// Enable Lighting
			SceneSwitchOnLights();
			break;
		case 'L':
			// Disable Lighting
			SceneSwicthOffLights();
			break;
		case 'z':
			glEnable(GL_CULL_FACE);
			break;
		case 'Z':
			glDisable(GL_CULL_FACE);
			break;

		case '.':
			theAvatar_mode = WalkOrFlyAvatar(theAvatar, theAvatar_mode);
			break;

		case 'a':
			RotateYNode(displayNode, -angle_step);
			break;
		case 'd':
			RotateYNode(displayNode, angle_step);
			break;
		case 'w':
			RotateXNode(displayNode, -angle_step);
			break;
		case 'x':
			RotateXNode(displayNode, angle_step);
			break;
		case 'i':
			TransNode(displayNode, 0.0, 0.0, 2.0*-step);
			break;
		case 'k':
			TransNode(displayNode, 0.0, 0.0, 2.0*step);
			break;
		case 27: // ESC
			exit(0);
			break;
		}
	glutPostRedisplay( );
}

// Special keyboard dispatcher (arrow keys, etc).
static void SpecialKey (int key, int x, int y) {

	switch(key)
		{
		case GLUT_KEY_RIGHT:
			LeftRightAvatar(theAvatar, -0.05f);
			break;
		case GLUT_KEY_LEFT:
			LeftRightAvatar(theAvatar, 0.05f);
			break;
		case GLUT_KEY_UP:
			UpDownAvatar(theAvatar, 0.05f);
			break;
		case GLUT_KEY_DOWN:
			UpDownAvatar(theAvatar, -0.05f);
			break;
		case GLUT_KEY_PAGE_UP:
			AdvanceAvatar(theAvatar, 1);
			break;
		case GLUT_KEY_PAGE_DOWN:
			AdvanceAvatar(theAvatar, -1);
			break;
		case GLUT_KEY_HOME:
			SetCamera(theCamera,
					  0, 0, 0,
					  0, 0, -1,
					  0, 1, 0);
			break;
		/// F1, F2, F3, Fxyz4 keys
      	case GLUT_KEY_F1:
         		mainLight_s = 1 - mainLight_s;
			SwitchLight(mainLight, mainLight_s);
			break;
		case GLUT_KEY_F2:
			dirLight_s = 1 - dirLight_s;
			SwitchLight(dirLight, dirLight_s);
			break;
		case GLUT_KEY_F3:
			dirLight2_s = 1 - dirLight2_s;
			SwitchLight(dirLight2, dirLight2_s);
			break;
		case GLUT_KEY_F4:
			spLight_s = 1 - spLight_s;
			SwitchLight(spLight, spLight_s);
			break;
		}
	glutPostRedisplay( );
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			mouseX = x;
			mouseY = y;
		}
}

void mouseMotion(int x, int y) {

	LeftRightAvatar(theAvatar, (float)(mouseX-x)/10000);
	UpDownAvatar(theAvatar, (float)(mouseY-y)/10000);

	glutPostRedisplay();

}


void idle(void) {
}

void animate(int value) {
	// Set up the next timer tick (do this first)
	glutTimerFunc(AGA_TIMERMSECS, animate, 0);
	// Measure the elapsed time
	int currTime = glutGet(GLUT_ELAPSED_TIME);
	int timeSincePrevFrame = currTime - prevTime;
	int elapsedTime = currTime - startTime;

	// ##### REPLACE WITH YOUR OWN GAME/APP MAIN CODE HERE #####
	if (runAnimation) {
		// Force a redisplay to render the new image

		glutPostRedisplay();
	}
	// ##### END OF GAME/APP MAIN CODE #####
	prevTime = currTime;
}


int main(int argc, char** argv) {

	GLenum glew_err;

	// define parameters that will be used to create windows

	// Init openGL and create a window
	srand(time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize ( 900, 700 );
	glutInitWindowPosition ( 100, 0 );
	// create one window for OpenGL graphics
	glutCreateWindow("browser");

	// Uncomment following line if you have problems initiating GLEW
	//
	// glewExperimental = GL_TRUE;

	glew_err = glewInit();

	if (glew_err != GLEW_OK) {
		fprintf(stderr, "Error when calling glewInit: %s\n", glewGetString(glew_err));
		exit(1);
	}

	glutDisplayFunc( Display );
	glutKeyboardFunc( Keyboard );
	glutSpecialFunc( SpecialKey );
	//glutMouseFunc(mouse);     // Don't use mouse by now
	//glutMotionFunc(mouseMotion);
	glutReshapeFunc( Resize );
	//glutIdleFunc( idle );

	// Init scene
	InitScene();

	// Init openGL objects
	InitGL(900, 700);

	displayNode = create_scene_fragment();
	// Other possible scene:
	//
	//create_scene_city();

	// Start the timer (uncomment if you want animations)
	glutTimerFunc(AGA_TIMERMSECS, animate, 0);

	// Initialize the time variables
	startTime = glutGet(GLUT_ELAPSED_TIME);
	prevTime = startTime;

	sky = CreateSkybox("skydome", "obj/sky");

	glutMainLoop();

	DestroyScene();

	return 0;
}

/*
 * Local Variables:
 * mode: c
 * End:
 */
