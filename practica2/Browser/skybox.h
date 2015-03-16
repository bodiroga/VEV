#ifndef SKYBOX_H
#define SKYBOX_H

#include "gObject.h"
#include "camera.h"

GObject *CreateSkybox(char *sbname,
					  char *dirname);

void DisplaySky(GObject *sky, camera *cam);

#endif /* SKYBOX_H */
