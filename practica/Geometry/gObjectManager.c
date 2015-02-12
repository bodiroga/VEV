#include "gObjectManager.h"
#include "hash.h"
#include <string.h>
#include <stddef.h>

static hash *gObj_hash = NULL;

// Create a key given a directory and filename

static char *create_gkey(char *lib, char *name) {

	static char AGA_SC_KEY[2048];

	size_t lgh;

	lgh = strlen(lib) + strlen(name) + 2;
	if (lgh > 2047) {
		fprintf(stderr,"[E] gObject name too long!\n%s/%s\n", lib, name);
		exit(1);
	}
	sprintf(AGA_SC_KEY, "%s/%s", lib, name);
	return AGA_SC_KEY;
}

void InitGObjectManager() {
	gObj_hash = CreateVoidHash();
}

void DestroyGObjectManager() {

	GObject *gObj;

	if (gObj_hash == NULL) return;
	gObj = StartLoopHash( gObj_hash );
	while(gObj) {
		DestroyGObject(&gObj);
		gObj = GetNextHash( gObj_hash );
	}
	DestroyHash(&gObj_hash);
	gObj_hash = NULL;
}

// Geometry objects

GObject *SceneFindObject(char *dir, char *fname) {

	char *key;

	key = create_gkey(dir, fname);
	return FindHashElement(gObj_hash, key);
}

GObject *SceneRegisterGObject(char *dir, char *filename) {

	char *key;
	GObject *theGobj;

	key = create_gkey(dir, filename);
	theGobj = FindHashElement(gObj_hash, key);
	if (!theGobj) {
		theGobj = InsertHashElement(gObj_hash, key, CreateFromWavefront(dir, filename));
	} else {
		fprintf(stderr, "[W] duplicate geometric object %s/%s. Skipping.\n", dir, filename);
	}
	return theGobj;
}

GObject *SceneRegisterVoidGObject(char *dir, char *filename) {
	char *key;
	GObject *theGobj;

	key = create_gkey(dir, filename);
	theGobj = FindHashElement(gObj_hash, key);
	if (!theGobj) {
		theGobj = InsertHashElement(gObj_hash, key, CreateVoidGObject());
	} else {
		fprintf(stderr, "[W] duplicate geometric object %s/%s. Skipping.\n", dir, filename);
	}
	return theGobj;
}
