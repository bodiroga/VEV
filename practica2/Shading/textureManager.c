#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include "hash.h"
#include "textureManager.h"

static hash *tex_hash = NULL;

//#define MAX_TEXTURES   32
static int LOCAL_TM_MIPMAP = 0;

// Functions

static char *create_tkey(char *name, int type) {

	static char AGA_SC_KEY[2048];

	size_t lgh;

	lgh = strlen(name) + 8; // "#" + 7 characters for type
	if (lgh > sizeof(AGA_SC_KEY) - 1) {
		fprintf(stderr, "[E] texture name too long!\n%s#%d\n", name, type);
		exit(1);
	}
	sprintf(AGA_SC_KEY, "%s/%d", name, type);
	return AGA_SC_KEY;
}


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


Texture *SceneFindTexture(char *fname, int type) {

	char *key;

	key = create_tkey(fname, type);
	return FindHashElement(tex_hash, key);
}


Texture *SceneRegisterTexture(char *fname) {

	char *key;
	Texture *tex;

	key = create_tkey(fname, AGA_TEX_TEX);
	tex = FindHashElement(tex_hash, key);
	if (!tex) {
		tex = CreateVoidTexture(fname);
		SetImageTexture(tex, fname);
		tex = InsertHashElement(tex_hash, key, tex);
	}
	return tex;
}

Texture *SceneRegisterBumpTexture(char *fname) {

	char *key;
	Texture *tex;

	key = create_tkey(fname, AGA_TEX_BUMPMAP);
	tex = FindHashElement(tex_hash, key);
	if (!tex) {
		tex = CreateVoidTexture(fname);
		SetBumpTexture(tex, fname);
		tex = InsertHashElement(tex_hash, key, tex);
	}
	return tex;

}

Texture *SceneRegisterProjTexture(char *fname) {

	char *key;
	Texture *tex;

	key = create_tkey(fname, AGA_TEX_PROJ);
	tex = FindHashElement(tex_hash, key);
	if (!tex) {
		tex = CreateVoidTexture(fname);
		SetProjTexture(tex, fname);
		tex = InsertHashElement(tex_hash, key, tex);
	}
	return tex;

}

Texture *SceneRegisterDepthMapTexture(char *texName, int height, int width) {

	char *key;
	Texture *tex;

	key = create_tkey(texName, AGA_TEX_DEPTH);
	tex = FindHashElement(tex_hash, key);
	if (!tex) {
		tex = CreateVoidTexture(texName);
		SetDepthMapTexture(tex, height, width);
		tex = InsertHashElement(tex_hash, key, tex);
	}
	return tex;
}

Texture *SceneRegisterColorMapTexture(char *texName, int height, int width) {

	char *key;
	Texture *tex;

	key = create_tkey(texName, AGA_TEX_COLOR);
	tex = FindHashElement(tex_hash, key);
	if (!tex) {
		tex = CreateVoidTexture(texName);
		SetColorMapTexture(tex, height, width);
		tex = InsertHashElement(tex_hash, key, tex);
	}
	return tex;

}

Texture *SceneRegisterCubeMapTexture(char *texName, char *xpos, char *xneg,
									 char *ypos, char *yneg,
									 char *zpos, char *zneg)  {

	char *key;
	Texture *tex;

	key = create_tkey(texName, AGA_TEX_CUBEMAP);
	tex = FindHashElement(tex_hash, key);

	if (!tex) {
		tex = CreateVoidTexture(texName);
		SetCubeMapTexture(tex,
						  xpos, xneg,
						  ypos, yneg,
						  zpos, zneg);
		tex = InsertHashElement(tex_hash, key, tex);
	}
	return tex;

}

void SceneDestroyTexture(char *texName, int type) {

	char *key;
	Texture *theTexture;

	key = create_tkey(texName, AGA_TEX_TEX);
	theTexture = FindHashElement(tex_hash, key);
	if(!theTexture) return;
	RemoveHashElement(tex_hash, key);
	DestroyTexture(&theTexture);
}

void PrintRegisteredTextures() {

	Texture *item;
	for( item = StartLoopHash(tex_hash); item; item = GetNextHash(tex_hash) )
		PrintTexture(item);
}
