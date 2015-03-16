#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <stddef.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "image.h"

// Texture types:

#define AGA_TEX_TEX       0  // texture (from JPG)
#define AGA_TEX_DEPTH     1  // Depth texture (generated)
#define AGA_TEX_COLOR     2  // Color texture (generated)
#define AGA_TEX_CUBEMAP   3  // Cubemap texture (from JPG)
#define AGA_TEX_BUMPMAP   4  // Bump map (normal) texture (from JPG)
#define AGA_TEX_PROJ      5  // Underlying texture of projection textures

typedef struct {

	int type;         // AGA_TEX_TEX

	GLenum target;    // GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP. Default is GL_TEXTURE_2D.
	GLuint textureId; // OpenGL id for the texture

	int mipmap;     // 0 - no mipmap, 1 - mipmap. Default 0

	GLenum magFilter; // GL_NEAREST, GL_LINEAR. Default GL_NEAREST.
	GLenum minFilter; // GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
	// GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR
	// and GL_LINEAR_MIPMAP_LINEAR.
	// Default GL_NEAREST (GL_NEAREST_MIPMAP_NEAREST if mipmap)

	GLenum wrapS;     // GL_CLAMP, GL_REPEAT. Default GL_REPEAT.
	GLenum wrapT;     // GL_CLAMP, GL_REPEAT. Default GL_REPEAT.
	GLenum wrapR;     // GL_CLAMP, GL_REPEAT.

	size_t size;             // Image size in bytes
	int height;           // Texture height (must be power of 2)
	int width;            // Texture width (must be power of 2)
	int components;       // number of components of each pixel (3 or 1)
	GLuint format;        // format of pixels (GL_RGB or DEPTH_COMPONENT)
	Img *img;             // jpg image

} Texture;


// Create, Destroy & Copy functions

void DestroyTexture (Texture **theTexture);
Texture *CreateVoidTexture(char *name);

int GetTypeTexture(Texture *theTexture);

// Texture map types
void SetImageTexture(Texture *thisTexture, char *img_fname); // image texture (jpg)
void SetBumpTexture(Texture *thisTexture, char *img_fname);  // bump texture (jpg)
void SetProjTexture(Texture *thisTexture, char *img_fname);  // projective texture (jpg)
void SetDepthMapTexture(Texture *thisTexture, int height, int width); // depth textures (generated)
void SetColorMapTexture(Texture *thisTexture, int height, int width); // color texture (generated)
void SetCubeMapTexture(Texture *thisTexture,
					   char *xpos, char *xneg,
					   char *ypos, char *yneg,
					   char *zpos, char *zneg); // cubemap textures (jpg)

/////////////////////////////////////////////////
// Functions for changing the texture parameters

int SetMipmapTexture(Texture *thisTexture, int mipmap);
int GetMipmapTexture(Texture *thisTexture);

// Wrap

void SetTextureWrapST(Texture *thisTexture, GLenum wrapS, GLenum wrapT);
void SetTextureWrapS(Texture *thisTexture, GLenum wrapS);
void SetTextureWrapT(Texture *thisTexture, GLenum wrapT);

// Cycle between wrap values

void CycleTextureWrapS(Texture *thisTexture);
void CycleTextureWrapT(Texture *thisTexture);

// Filters

void SetTextureMinFilter(Texture *thisTexture, GLenum minFilter);
void SetTextureMagFilter(Texture *thisTexture, GLenum magFilter);
void SetTextureFilters(Texture *thisTexture, GLenum minFilter, GLenum magFilter);

// Get image from OpenGL and store it in memory
void SyncOpenGLTexture(Texture *thisTexture);

// Bind a texture to OpenGL to the current texture unit
void BindGLTexture(Texture *thisTexture);
// Bind a texture to OpenGL at a given texture unit
void BindGLTextureTunit(Texture *thisTexture, int location);

// Unbind texture from OpenGL
void UnbindGLTexture(Texture *thisTexture);
// Unbind a texture from OpenGL at a given texture unit
void UnbindGLTextureTunit(Texture *thisTexture, int location);

// Save a texture to a JPG file
void SaveJPGTexture(Texture *thisTexture, char *filename);

// Cycle between filters

void CycleTextureMagFilter(Texture *thisTexture);
void CycleTextureMinFilter(Texture *thisTexture);

void PrintTexture (Texture *thisTexture);

#endif //TEXTURE_H
