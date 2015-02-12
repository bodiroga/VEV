#ifndef TMANAGER_H
#define TMANAGER_H

#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "texture.h"


void InitTextureManager();
void DestroyTextureManager();

/**
 * Get a registered texture
 *
 *
 * @param FileName: The name of the texture including the path.
 * @return the texture or NULL if not found.
 */

Texture *SceneFindTexture(char *FileName);

/**
 * Register a new void texture
 *
 * If a texture with the same name exists, return the associated texture.
 *
 * @param texName: The name of the texture.
 */

Texture *SceneRegisterVoidTexture(char *texName);

/**
 * Register a new texture
 *
 * If FileName is new, create new texture and register it.
 * If FileName already exists, return the associated texture.
 *
 * @param FileName: The name of the texture including the path.
 */

Texture *SceneRegisterTexture(char *FileName);

/**
 * Destroy a registered texture
 *
 * @param texName: The name of the texture
 */

void SceneDestroyTexture(char *texName);

/**
 * Set MipMap
 *
 * Wether new registered textures will be mipmaps or not.
 *
 * @param mmap (0: no mipmap, 1: mipmap) Default: 0.
 * @return the old mipmap value
 */

int SceneSetMipMap(int mmap);

/**
 * Blend function
 *
 * @param texFunc GL_DECAL, GL_REPLACE, GL_MODULATE, GL_BLEND. Default GL_MODULATE
 */

void SceneSetTextureFunction(GLint texFunc);

void PrintRegisteredTextures();

#endif //SCENE_H
