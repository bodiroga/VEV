#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "tools.h"
#include "hash.h"
#include "materialManager.h"

static hash *mat_hash = NULL;

// Create a key given a library and material name

static char *create_mkey(char *lib, char *name) {

	static char AGA_SC_KEY[2048];

	size_t lgh;

	lgh = strlen(lib) + strlen(name) + 2;
	if (lgh > 2047) {
		fprintf(stderr, "[E] material name too long!\n%s/%s\n", lib, name);
		exit(1);
	}
	sprintf(AGA_SC_KEY, "%s/%s", lib, name);
	return AGA_SC_KEY;
}

void InitMaterialManager() {
	mat_hash = CreateVoidHash();
}

void DestroyMaterialManager() {

	Material *mat;

	if (mat_hash == NULL) return;
	mat = StartLoopHash( mat_hash );
	while(mat) {
		DestroyMaterial(&mat);
		mat = GetNextHash( mat_hash );
	}
	DestroyHash(&mat_hash);
	mat_hash = NULL;
}

Material *SceneDefaultMaterial() {

	static Material *default_material = NULL;

	if (default_material == NULL) {
		default_material = SceneRegisterMaterial("_AGA_Default_", "_AGA_Default_");
	}
	return default_material;
}

Material *SceneFindMaterial(char *lib, char *mname) {

	char *key;

	key = create_mkey(lib, mname);
	return FindHashElement(mat_hash, key);
}

Material *SceneRegisterMaterial(char *lib, char *name) {

	char *key;
	Material *theMat;

	key = create_mkey(lib, name);
	theMat = FindHashElement(mat_hash, key);
	if (!theMat) {
		theMat = InsertHashElement(mat_hash, key, CreateMaterial(lib, name));
	} else {
		fprintf(stderr, "[W] duplicate material %s/%s\n", lib, name);
	}
	return theMat;

}

void SceneDestroyMaterial(char *lib, char *name) {

	Material *theMat;
	char *key;

	key = create_mkey(lib, name);
	theMat = FindHashElement(mat_hash, key);
	if (!theMat) return;
	RemoveHashElement(mat_hash, key);
	DestroyMaterial(&theMat);
}

// Debug

void PrintRegisteredMaterials() {

	Material *mat;

	if (mat_hash == NULL) return;
	mat = StartLoopHash( mat_hash );
	while(mat) {
		PrintMaterial(mat);
		mat = GetNextHash( mat_hash );
	}
}
