#include <stdio.h>
#include "hash.h"
#include "shaderManager.h"

static hash *shader_hash;

void InitShaderManager() {
	shader_hash = CreateVoidHash();
}

void DestroyShaderManager() {

	ShaderProgram *oneProgram;
	oneProgram = StartLoopHash(shader_hash);
	while(oneProgram) {
		DestroyShader(&oneProgram);
		oneProgram = GetNextHash(shader_hash);
	}
	DestroyHash(&shader_hash);
}

ShaderProgram *RegisterShaderScene(char *shaderName, char *vShaderFile, char *fShaderFile) {
	ShaderProgram *newProgram;

	newProgram = FindHashElement(shader_hash, shaderName);
	if (newProgram != NULL) {
		fprintf(stderr, "[W] sceneRegisterShader: %s already registered\n", shaderName);
		return newProgram;
	}
	newProgram = CreateShader(shaderName, vShaderFile, fShaderFile);
	if (!newProgram) {
		fprintf(stderr, "[E] Can't register shader \'%s\'. See line above.\n", shaderName);
		exit(1);
	}
	return InsertHashElement(shader_hash, shaderName, newProgram);
}

ShaderProgram *FindShaderScene(char *shaderName) {
	ShaderProgram *theProgram;

	theProgram = FindHashElement(shader_hash, shaderName);
	if (theProgram == NULL) {
		fprintf(stderr, "[E] SetCurrentShader: %s not registered\n", shaderName);
		exit(1);
	}
	return theProgram;
}
