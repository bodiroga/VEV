#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include "hash.h"
#include "lightManager.h"
#include <GL/glut.h>

static hash *light_hash = NULL;


void InitLightManager() {

	light_hash = CreateVoidHash();
}

void DestroyLightManager() {
	Light *oneLight;

	if (light_hash == NULL) return;
	oneLight = StartLoopHash( light_hash );
	while(oneLight) {
		DestroyLight(&oneLight);
		oneLight = GetNextHash( light_hash );
	}
	DestroyHash(&light_hash);
}

static void scene_destroy_light(char *key, Light *theLight) {
	RemoveHashElement(light_hash, key);
	DestroyLight(&theLight);
}

void SceneDestroyLight(char *lightName) {

	Light * theLight;

	theLight = FindHashElement(light_hash, lightName);
	if (!theLight) return;
	scene_destroy_light(lightName, theLight);
}

Light *SceneRegisterLight(char *lightName) {

	Light *theLight = NULL;

	theLight = FindHashElement(light_hash, lightName);
	if (theLight) {
		fprintf(stderr, "[W] duplicate light %s\n. Skipping.", lightName);
		return theLight;
	}
	// New light
	return InsertHashElement(light_hash, lightName, CreateVoidLight());
}

Light *SceneGetLight(char *lightName) {

	return FindHashElement(light_hash, lightName);

}

void SceneSwitchOnLights() {

	Light *oneLight;

	oneLight = StartLoopHash( light_hash );
	while(oneLight) {
		SwitchLight(oneLight, 1);
		oneLight = GetNextHash( light_hash );
	}
}

void SceneSwicthOffLights() {

	Light *oneLight;

	oneLight = StartLoopHash( light_hash );
	while(oneLight) {
		SwitchLight(oneLight, 0);
		oneLight = GetNextHash( light_hash );
	}
}

hash *SceneGetRegisteredLights() {
	return light_hash;
}

// Debug

void PrintRegisteredLights() {

	Light *item;
	if (light_hash == NULL) return;
	for( item = StartLoopHash(light_hash); item; item = GetNextHash(light_hash) )
		PrintLight(item);

}
