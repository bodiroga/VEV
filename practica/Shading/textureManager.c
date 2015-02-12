#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include "hash.h"
#include "textureManager.h"

static hash *tex_hash = NULL;

//#define MAX_TEXTURES   32
static int LOCAL_TM_MIPMAP = 0;

// Functions

void InitTextureManager() {
	tex_hash = CreateVoidHash();
}

void DestroyTextureManager() {

	Texture *oneTexture;

	if (tex_hash == NULL) return;
	oneTexture = StartLoopHash( tex_hash );
	while(oneTexture) {
		DestroyTexture(&oneTexture);
		oneTexture = GetNextHash( tex_hash );
	}
	DestroyHash(&tex_hash);
}

int SceneSetMipMap(int mmap) {
	int res = LOCAL_TM_MIPMAP;
	if (!mmap) LOCAL_TM_MIPMAP = 0;
	else LOCAL_TM_MIPMAP = 1;
	return res;
}

void SceneSetTextureFunction(GLint texFunc) {
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texFunc);
}

Texture *SceneRegisterVoidTexture(char *texName) {

	Texture *theTexture;
	Texture *newTexture;

	theTexture = FindHashElement(tex_hash, texName);
	if (!theTexture) {
		newTexture = CreateVoidTexture(texName);
		if (LOCAL_TM_MIPMAP) {
			newTexture->mipmap = 1;
			SetTextureFilters(theTexture, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST);
		}
		theTexture = InsertHashElement(tex_hash, texName, newTexture);
	} else {
		fprintf(stderr, "[W] duplicate texture %s\n", texName);
	}
	return theTexture;
}

Texture *SceneRegisterTexture(char *texName) {

	Texture *theTexture;
	Texture *newTexture;

	theTexture = FindHashElement(tex_hash, texName);
	if (!theTexture) {
		if (LOCAL_TM_MIPMAP) {
			newTexture = CreateMipmapTexture( texName );
		} else {
			newTexture = CreateTexture( texName );
		}
		theTexture = InsertHashElement(tex_hash, texName, newTexture);
	} else {
		fprintf(stderr, "[W] duplicate texture %s\n", texName);
	}
	return theTexture;
}

Texture *SceneFindTexture(char *texName) {

	return FindHashElement(tex_hash, texName);
}

void SceneDestroyTexture(char *texName) {

	Texture *theTexture;

	theTexture = FindHashElement(tex_hash, texName);
	if(!theTexture) return;
	RemoveHashElement(tex_hash, texName);
	DestroyTexture(&theTexture);
}

void PrintRegisteredTextures() {

	Texture *item;
	for( item = StartLoopHash(tex_hash); item; item = GetNextHash(tex_hash) )
		PrintTexture(item);
}
