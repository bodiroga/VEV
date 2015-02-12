#ifndef _GOBJECTMANAGER_H_
#define _GOBJECTMANAGER_H_

#include "gObject.h"

void InitGObjectManager();
void DestroyGObjectManager();

/**
 * Register a new geometric object, reading a Wavefront file
 *
 * If gObject name is new, create new gObject and register it.
 * If gObject already exists, return it
 *
 * @param dir: The directory where the wavefront file is
 * @param name: The name of the wavefront file with the geometric info
 * @return the geometric object
 */

GObject *SceneRegisterGObject(char *dir, char *name);

/**
 * Register a new void geometric object
 *
 * If gObject name is new, create new gObject and register it.
 * If gObject already exists, return it
 *
 * @param dir: A name of a directory
 * @param name: The name for the gObject
 * @return the geometric object
 */

GObject *SceneRegisterVoidGObject(char *dir, char *filename);


/**
 * Find a geometric object
 *
 * @param dir: The directory where the wavefront file is
 * @param name: The name of the wavefront file with the geometric info
 * @return the geometric object or NULL if not found
 */

GObject *SceneFindGObject(char *dir, char *name);

#endif /* _GOBJECTMANAGER_H_ */
