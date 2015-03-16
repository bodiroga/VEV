#ifndef LMANAGER_H
#define LMANAGER_H

#include "light.h"

void InitLightManager();
void DestroyLightManager();

Light *SceneRegisterLight(char * lightName);

Light *SceneGetLight(char *lightName);

/**
 * Release a light from the scene
 *
 * @param theLight a pointer to the light to be released
 */

void SceneDestroyLight(char *lightName);

/**
 * Switch all lights 'ON'
 *
 */
void SceneSwitchOnLights();

/**
 * Switch all lights 'OFF'
 *
 */
void SceneSwicthOffLights();


/**
 * Print all registered lights
 *
 */
void PrintRegisteredLights();

#endif //SCENE_H
