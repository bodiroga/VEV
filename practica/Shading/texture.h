#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <stddef.h>
#include <GL/glew.h>
#include <GL/glut.h>

// Texture types:
//
// AGA_TEX_NORMAL: normal texture
// AGA_TEX_DEPTH: depth map
// AGA_TEX_COLOR: color texture

#define AGA_TEX_NORMAL 0
#define AGA_TEX_DEPTH  1
#define AGA_TEX_COLOR  2

typedef struct {

	int type;         // AGA_TEX_NORMAL

	GLuint textureId; // OpenGL id for the texture

	int height;     // Texture height (must be power of 2)
	int width;      // Texture width (must be power of 2)

	char *fileName; // The image path
	int mipmap;     // 0 - no mipmap, 1 - mipmap. Default 0


	GLenum magFilter; // GL_NEAREST, GL_LINEAR. Default GL_NEAREST.
	GLenum minFilter; // GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
					  // GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR
					  // and GL_LINEAR_MIPMAP_LINEAR.
					  // Default GL_NEAREST (GL_NEAREST_MIPMAP_NEAREST if mipmap)

	GLenum wrapS;     // GL_CLAMP, GL_REPEAT. Default GL_REPEAT.
	GLenum wrapT;     // GL_CLAMP, GL_REPEAT. Default GL_REPEAT.

	int components;       // number of components of each pixel (3 or 1)
	GLuint format;        // format of pixels (GL_RGB or DEPTH_COMPONENT)
	size_t size;          // size (in bytes) of the image
	unsigned char *data;  // image data

} Texture;


// Create, Destroy & Copy functions

Texture *CreateTexture(char *filename);
Texture *CreateMipmapTexture(char *filename);
void DestroyTexture (Texture **theTexture);
Texture *CreateCopyTexture(const Texture *from);
void CopyTexture (Texture *to, const Texture *from);
Texture *CreateVoidTexture(char *name);

int GetTypeTexture(Texture *theTexture);

// Texture maps
void SetTextureDepthMap(Texture *thisTexture, int height, int width);
void SetTextureColorMap(Texture *thisTexture, int height, int width);

/////////////////////////////////////////////////
// Functions for changing the texture parameters

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
int BindGLTexture(Texture *thisTexture);
// Bind a texture to OpenGL at a given texture unit
void BindGLTextureTunit(Texture *thisTexture, int location);

// Save a texture to a JPG file
void SaveJPGTexture(Texture *thisTexture, char *filename);

// Cycle between filters

void CycleTextureMagFilter(Texture *thisTexture);
void CycleTextureMinFilter(Texture *thisTexture);


void PrintTexture (Texture *thisTexture);

#endif //TEXTURE_H
