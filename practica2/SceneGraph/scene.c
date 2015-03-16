#include <assert.h>
#include "scene.h"
#include "tools.h"
#include "trfm3D.h"
#include "textureManager.h"
#include "materialManager.h"
#include "lightManager.h"
#include "gObjectManager.h"
#include "cameraManager.h"
#include "shaderManager.h"
#include "image.h"
#include "hash.h"
#include "renderState.h"


static Node *rootNode = NULL;
static hash *node_hash = NULL; // nodes are not owned.

static trfm3D *I;

static RenderState *rstate;

// Functions

void InitScene() {

	rootNode = CreateNode();
	node_hash = CreateVoidHash();
	I = CreateTrfm3D();
	rstate = CreateRenderState();
	InitCameraManager();
	InitGObjectManager();
	InitMaterialManager();
	InitImageManager();
	InitTextureManager();
	InitLightManager();
	InitShaderManager();

	SceneSetTextureFunction(GL_MODULATE);
}

void DestroyScene() {

	DestroyNode(&rootNode);
	DestroyHash(&node_hash);
	DestroyRenderState(&rstate);
	if (I) DestroyTrfm3D(&I);
	DestroyShaderManager();
	DestroyLightManager();
	DestroyImageManager();
	DestroyTextureManager();
	DestroyMaterialManager();
	DestroyGObjectManager();
	DestroyCameraManager();
}

// Render state

RenderState *RenderStateScene() {
	return rstate;
}

// Node Management

Node *RootNodeScene() {

	return rootNode;
}

int SetNodenameScene(Node *theNode, char *name) {

	if (!name) return 0;
	if (FindHashElement(node_hash, name) != NULL) {
		fprintf(stderr, "[W] duplicate node %s\n", name);
		return 0; // a node with same name already exists
	}
	return InsertHashElement(node_hash, name, theNode) != NULL;
}

Node *FindNodeScene(char * name) {

	return FindHashElement(node_hash, name);
}

void SetShadingScene(GLenum face, GLenum mode) {

	glPolygonMode(face, mode);
}

void NodeAttachScene(Node *theNode) {

	NodeAttach(rootNode, theNode);
}

// TODO: deal with transparent objects

void DrawScene() {

	if (rootNode) {
		DrawNode(rootNode);
	}
}
