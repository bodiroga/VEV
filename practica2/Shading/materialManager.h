#ifndef MMANAGER_H
#define MMANAGER_H

#include "material.h"

void InitMaterialManager();
void DestroyMaterialManager();

/**
 * Register a new material
 *
 * If material name is new, create new material and register it.
 * If material already exists, return it
 *
 * @param lib: The name of the material library
 * @param name: The name of the material
 */

Material *SceneRegisterMaterial(char *lib, char *name);

/**
 * Destroy a registered material
 *
 * @param lib: The name of the material library
 * @param name: The name of the material
 */

void SceneDestroyMaterial(char *lib, char *name);

/**
 * Get a registered material
 *
 * @param lib: The name of the material library
 * @param name: The name of the material
 * @return the material or NULL if not found
 */

Material *SceneFindMaterial(char *lib, char *mname);

/**
 * Get the default material
 *
 * @return the default material
 */

Material *SceneDefaultMaterial();

void PrintRegisteredMaterials();

#endif //SCENE_H
