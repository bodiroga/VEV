#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "camera.h"

void InitCameraManager();
void DestroyCameraManager();

/**
 * Get a registered camera
 *
 *
 * @param cameraName: The name of the camera
 * @return the camera or NULL if not found.
 */

camera *SceneFindCamera(char *cameraName);

/**
 * Register a new camera
 *
 * If cameaName is new, create new origin camera and register it.
 * If cameaName already exists, return the associated camera.
 *
 * @param cameaName: The name of the camera
 * @return the new or existing camera
 */

camera *SceneRegisterCamera(char *cameraName);

/**
 * Destroy a registered camera
 *
 * @param texName: The name of the camera
 */

void SceneDestroyCamera(char *cameraName);


void PrintRegisteredCameras();

#endif // CAMERAMANAGER_H
