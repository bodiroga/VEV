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
 * @param type: the texture type (see texture.h)
 * @return the texture or NULL if not found.
 */

Texture *SceneFindTexture(char *FileName, int type);

/**
 * Register a new texture
 *
 * If FileName is new, create new texture and register it.
 * If FileName already exists, return the associated texture.
 *
 * @param FileName: The name of the texture image including the path.
 */

Texture *SceneRegisterTexture(char *FileName);

/**
 * Register a new texture for bump (normal) mapping
 *
 * If FileName is new, create new texture and register it.
 * If FileName already exists, return the associated texture.
 *
 * @param FileName: The name of the bump image including the path.
 */

Texture *SceneRegisterBumpTexture(char *FileName);

/**
 * Register a new texture for projective texturing
 *
 * If FileName is new, create new texture and register it.
 * If FileName already exists, return the associated texture.
 *
 * @param FileName: The name of the texture to project including the path.
 */

Texture *SceneRegisterProjTexture(char *FileName);

/**
 * Register a new texture for Depth mapping
 *
 * Used to render the depth value (z-buffer) to a texture instead of
 * framebuffer.
 *
 * @param texName: the name of the texture
 * @param height: the height of the texture (must be power of 2)
 * @param width: the width of the texture (must be power of 2)
 */

Texture *SceneRegisterDepthMapTexture(char *texName, int height, int width);

/**
 * Register a new texture for Color mapping
 *
 * Used to render the image to a texture instead of framebuffer.
 *
 * @param texName: the name of the texture
 * @param height: the height of the texture (must be power of 2)
 * @param width: the width of the texture (must be power of 2)
 */

Texture *SceneRegisterColorMapTexture(char *texName, int height, int width);

/**
 * Register a new cubemap texture
 *
 * If texName is new, create new texture and register it.
 * If texName already exists, return the associated texture.
 *
 * @param texName The name of the texture
 * @param xpos the filename with the XPOS image
 * @param xneg the filename with the XNEG image
 * @param ypos the filename with the YPOS image
 * @param yneg the filename with the YNEG image
 * @param zpos the filename with the ZPOS image
 * @param zneg the filename with the ZNEG image
 */

Texture *SceneRegisterCubeMapTexture(char *texName, char *xpos, char *xneg,
									 char *ypos, char *yneg,
									 char *zpos, char *zneg);


/**
 * Set MipMap
 *
 * Wether new registered textures will be mipmaps or not.
 * Only valid when appropriate
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

/**
 * Destroy a registered texture
 *
 * @param texName: The name of the texture
 * @param type: the texture type
 */

void SceneDestroyTexture(char *texName, int type);

void PrintRegisteredTextures();

#endif //SCENE_H
