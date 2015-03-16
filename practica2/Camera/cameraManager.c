#include "cameraManager.h"
#include <string.h>
#include "hash.h"

static hash *cam_hash;

void InitCameraManager() {
	cam_hash = CreateVoidHash();
}


void DestroyCameraManager() {
	camera *theCamera;
	if (cam_hash == NULL) return;
	theCamera = StartLoopHash(cam_hash);
	while(theCamera) {
		DestroyCamera(&theCamera);
		theCamera = GetNextHash(cam_hash);
	}
	DestroyHash(&cam_hash);
}


camera *SceneFindCamera(char *cameraName) {
	return FindHashElement(cam_hash, cameraName);
}

camera *SceneRegisterCamera(char *cameraName) {

	camera *theCamera;
	camera *newCamera;

	theCamera = FindHashElement(cam_hash, cameraName);
	if (!theCamera) {
		newCamera = CreateOriginCamera();
		SetCameraName(newCamera, cameraName);
		theCamera = InsertHashElement(cam_hash, cameraName, newCamera);
	} else {
		printf("Waning: duplicate camera %s. Skipping.\n", cameraName);
	}
	return theCamera;
}

void SceneDestroyCamera(char *cameraName) {

	camera *theCamera;
	theCamera = FindHashElement(cam_hash, cameraName);
	if(!theCamera) return;
	DestroyCamera(&theCamera);
}

void PrintRegisteredCameras() {

	camera *item;
	for(item = StartLoopHash(cam_hash); item; item = GetNextHash(cam_hash) ) {
		PrintCamera(item);
	}
}
