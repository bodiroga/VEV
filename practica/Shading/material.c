#include <assert.h>
#include "tools.h"
#include "material.h"
#include "textureManager.h"

void DestroyMaterial( Material **theMaterial ) {

	Material        *thisMaterial = *theMaterial;
	if( ! thisMaterial ) return;

	DestroyString( &thisMaterial->library );
	DestroyString( &thisMaterial->name );

	if (thisMaterial->texture) DestroyTexture(&(thisMaterial->texture));

	free( thisMaterial );

	theMaterial = NULL;
}

Material *CreateVoidMaterial() {

	Material        *newMaterial;

	newMaterial = malloc( sizeof(*newMaterial) * 1 );

	newMaterial->library            = NULL;
	newMaterial->name               = NULL;

	newMaterial->ambient[0]         =  0.2f;
	newMaterial->ambient[1]         =  0.2f;
	newMaterial->ambient[2]         =  0.2f;

	newMaterial->diffuse[0]         =  0.8f;
	newMaterial->diffuse[1]         =  0.8f;
	newMaterial->diffuse[2]         =  0.8f;
	newMaterial->alpha              =  1.0f;

	newMaterial->has_specular       =  0;
	newMaterial->specular[0]        =  0.0f;
	newMaterial->specular[1]        =  0.0f;
	newMaterial->specular[2]        =  0.0f;
	newMaterial->shininess          =  65.0f;

	newMaterial->has_emission       =  0;
	newMaterial->emission[0]        =  0.0f;
	newMaterial->emission[1]        =  0.0f;
	newMaterial->emission[2]        =  0.0f;

	newMaterial->texture            =  NULL;

	return newMaterial;
}


Material *CreateMaterial(char *libraryName,
						 char* materialName) {

	Material        *newMaterial;

	newMaterial = CreateVoidMaterial();

	newMaterial->library = CopyString( libraryName );
	newMaterial->name = CopyString( materialName );

	return newMaterial;
}


Material *CreateCopyMaterial( Material *thisMaterial ) {

	Material        *newMaterial;

	newMaterial = malloc( sizeof(*newMaterial) * 1 );
	CopyValuesMaterial( newMaterial, thisMaterial );

	return newMaterial;
}


void SetNameMaterial(Material *thisMaterial,
					 char *libraryName,
					 char* materialName) {

	DestroyString( &thisMaterial->library );
	DestroyString( &thisMaterial->name );
	thisMaterial->library = CopyString( libraryName );
	thisMaterial->name = CopyString( materialName );
}

void GetNameMaterial(Material *thisMaterial,
					 char **libraryName,
					 char **materialName) {
	*libraryName = thisMaterial->library;
	*materialName = thisMaterial->name;
}


void SetDiffuseMaterial(Material *thisMaterial, float *rgb) {

	thisMaterial->diffuse[0]         =  rgb[0];
	thisMaterial->diffuse[1]         =  rgb[1];
	thisMaterial->diffuse[2]         =  rgb[2];
}

float *GetDiffuseMaterial(Material *thisMaterial) {
	return &(thisMaterial->diffuse[0]);
}

void SetAlphaMaterial(Material *thisMaterial, float alpha) {
	thisMaterial->alpha = alpha;
}

float GetAlphaMaterial(Material *thisMaterial) {
	return thisMaterial->alpha;
}

void SetAmbientMaterial(Material *thisMaterial, float *rgb) {

	thisMaterial->ambient[0]         =  rgb[0];
	thisMaterial->ambient[1]         =  rgb[1];
	thisMaterial->ambient[2]         =  rgb[2];
}

float *GetAmbientMaterial(Material *thisMaterial) {
	return &(thisMaterial->ambient[0]);
}

void SetSpecularMaterial(Material *thisMaterial, float *rgb,
						 float shine) {

	thisMaterial->has_specular        =  0;
	thisMaterial->specular[0]         =  rgb[0];
	thisMaterial->specular[1]         =  rgb[1];
	thisMaterial->specular[2]         =  rgb[2];
	thisMaterial->shininess           =  shine;
}

float *GetSpecularMaterial(Material *thisMaterial) {
	return &(thisMaterial->specular[0]);
}

float GetShininessMaterial(Material *thisMaterial) {
	return thisMaterial->shininess;
}


void HasSpecularMaterial(Material *thisMaterial, int yesorno) {
	thisMaterial->has_specular = (yesorno != 0);
}

void SetEmissionMaterial(Material *thisMaterial, float rgb[3]) {

	thisMaterial->has_emission        =  1;
	thisMaterial->emission[0]         =  rgb[0];
	thisMaterial->emission[1]         =  rgb[1];
	thisMaterial->emission[2]         =  rgb[2];
}

float *GetEmissionMaterial(Material *thisMaterial) {
	return &(thisMaterial->emission[0]);
}


void HasEmissionMaterial(Material *thisMaterial, int yesorno) {
	thisMaterial->has_emission = (yesorno != 0);
}

void SetTextureMaterial (Material *thisMaterial,
						 Texture *theTexture) {
	thisMaterial->texture = theTexture;
}

Texture *GetTextureMaterial (Material *thisMaterial) {
	return thisMaterial->texture;
}

int IsTranspMaterial(Material *thisMaterial) {
	return thisMaterial->alpha != 1.0;
}

/* Auxiliary functions */

void CopyValuesMaterial( Material *thisMaterial, Material *source ) {

	DestroyString(&(thisMaterial->library));
	DestroyString(&(thisMaterial->name));

	thisMaterial->library           = CopyString( source->library );
	thisMaterial->name              = CopyString( source->name );

	thisMaterial->ambient[0]        =  source->ambient[0];
	thisMaterial->ambient[1]        =  source->ambient[1];
	thisMaterial->ambient[2]        =  source->ambient[2];

	thisMaterial->diffuse[0]        =  source->diffuse[0];
	thisMaterial->diffuse[1]        =  source->diffuse[1];
	thisMaterial->diffuse[2]        =  source->diffuse[2];
	thisMaterial->alpha             =  source->alpha;

	thisMaterial->has_specular      =  source->has_specular;
	thisMaterial->specular[0]       =  source->specular[0];
	thisMaterial->specular[1]       =  source->specular[1];
	thisMaterial->specular[2]       =  source->specular[2];

	thisMaterial->has_emission      =  source->has_emission;
	thisMaterial->emission[0]       =  source->emission[0];
	thisMaterial->emission[1]       =  source->emission[1];
	thisMaterial->emission[2]       =  source->emission[2];

	thisMaterial->shininess         =  source->shininess;

	if (source->texture) thisMaterial->texture =  CreateCopyTexture(source->texture);
}

void CheckMaterial( Material *thisMaterial ) {
	float check = -999;

	if( thisMaterial->diffuse[0] < 0.0f ) check = thisMaterial->diffuse[0] = 0.0f;
	if( thisMaterial->diffuse[1] < 0.0f ) check = thisMaterial->diffuse[1] = 0.0f;
	if( thisMaterial->diffuse[2] < 0.0f ) check = thisMaterial->diffuse[2] = 0.0f;
	if (thisMaterial->alpha < 0.0f) check = thisMaterial->alpha = 0.0f;

	if( thisMaterial->ambient[0] < 0.0f ) check = thisMaterial->ambient[0] = 0.0f;
	if( thisMaterial->ambient[1] < 0.0f ) check = thisMaterial->ambient[1] = 0.0f;
	if( thisMaterial->ambient[2] < 0.0f ) check = thisMaterial->ambient[2] = 0.0f;

	if( thisMaterial->specular[0] < 0.0f ) check = thisMaterial->specular[0] = 0.0f;
	if( thisMaterial->specular[1] < 0.0f ) check = thisMaterial->specular[1] = 0.0f;
	if( thisMaterial->specular[2] < 0.0f ) check = thisMaterial->specular[2] = 0.0f;

	if( thisMaterial->emission[0] < 0.0f ) check = thisMaterial->emission[0] = 0.0f;
	if( thisMaterial->emission[1] < 0.0f ) check = thisMaterial->emission[1] = 0.0f;
	if( thisMaterial->emission[2] < 0.0f ) check = thisMaterial->emission[2] = 0.0f;

	if( thisMaterial->shininess < 0.0f ) check = thisMaterial->shininess = 0.0f;

	if( thisMaterial->diffuse[0] < 0.0f ) check = thisMaterial->diffuse[0] = 0.0f;
	if( thisMaterial->diffuse[1] < 0.0f ) check = thisMaterial->diffuse[1] = 0.0f;
	if( thisMaterial->diffuse[2] < 0.0f ) check = thisMaterial->diffuse[2] = 0.0f;


	if( thisMaterial->ambient[0] > 1.0f ) check = thisMaterial->ambient[0] = 1.0f;
	if( thisMaterial->ambient[1] > 1.0f ) check = thisMaterial->ambient[1] = 1.0f;
	if( thisMaterial->ambient[2] > 1.0f ) check = thisMaterial->ambient[2] = 1.0f;
	if (thisMaterial->alpha > 1.0f) check = thisMaterial->alpha = 1.0f;

	if( thisMaterial->specular[0] > 1.0f ) check = thisMaterial->specular[0] = 1.0f;
	if( thisMaterial->specular[1] > 1.0f ) check = thisMaterial->specular[1] = 1.0f;
	if( thisMaterial->specular[2] > 1.0f ) check = thisMaterial->specular[2] = 1.0f;

	if( thisMaterial->emission[0] > 1.0f ) check = thisMaterial->emission[0] = 1.0f;
	if( thisMaterial->emission[1] > 1.0f ) check = thisMaterial->emission[1] = 1.0f;
	if( thisMaterial->emission[2] > 1.0f ) check = thisMaterial->emission[2] = 1.0f;

	if( thisMaterial->shininess > 128.0f ) check = thisMaterial->shininess = 128.0f;

	if( check != -999 ) printf( "Warning file %s material %s\n",
								thisMaterial->library, thisMaterial->name );
}

int  CompareMaterial( Material *thisMaterial, Material *otherMaterial ) {

	if( fabs( thisMaterial->diffuse[0] - otherMaterial->diffuse[0] ) > 0.03f ) return FALSE;
	if( fabs( thisMaterial->diffuse[1] - otherMaterial->diffuse[1] ) > 0.03f ) return FALSE;
	if( fabs( thisMaterial->diffuse[2] - otherMaterial->diffuse[2] ) > 0.03f ) return FALSE;
	if( fabs( thisMaterial->alpha - otherMaterial->alpha ) > 0.03f ) return FALSE;

	if( strcmp( thisMaterial->name, otherMaterial->name ) != 0 ) return FALSE;
	if( strcmp( thisMaterial->library, otherMaterial->library ) != 0 ) return FALSE;

	if( fabs( thisMaterial->ambient[0] - otherMaterial->ambient[0] ) > 0.03f ) return FALSE;
	if( fabs( thisMaterial->ambient[1] - otherMaterial->ambient[1] ) > 0.03f ) return FALSE;
	if( fabs( thisMaterial->ambient[2] - otherMaterial->ambient[2] ) > 0.03f ) return FALSE;

	if (thisMaterial->has_specular != otherMaterial->has_specular) return FALSE;
	if (thisMaterial->has_specular) {
		if( fabs( thisMaterial->specular[0] - otherMaterial->specular[0] ) > 0.03f ) return FALSE;
		if( fabs( thisMaterial->specular[1] - otherMaterial->specular[1] ) > 0.03f ) return FALSE;
		if( fabs( thisMaterial->specular[2] - otherMaterial->specular[2] ) > 0.03f ) return FALSE;
		if( fabs( thisMaterial->shininess - otherMaterial->shininess ) > 0.3f ) return FALSE;
	}

	if (thisMaterial->has_emission != otherMaterial->has_emission) return FALSE;
	if (thisMaterial->has_emission) {
		if( fabs( thisMaterial->emission[0] - otherMaterial->emission[0] ) > 0.03f ) return FALSE;
		if( fabs( thisMaterial->emission[1] - otherMaterial->emission[1] ) > 0.03f ) return FALSE;
		if( fabs( thisMaterial->emission[2] - otherMaterial->emission[2] ) > 0.03f ) return FALSE;
	}

	return TRUE;
}

void PrintMaterial( Material *thisMaterial ) {
	if( ! thisMaterial ) {
		fprintf(stderr, "[E] NULL Material pointer\n");
		return;
	}

	printf("Library:%s Name: %s\n", thisMaterial->library, thisMaterial->name );
	printf(" Ambient  %04.2f %04.2f %04.2f\n",
		   thisMaterial->ambient[0], thisMaterial->ambient[1],
		   thisMaterial->ambient[2]);
	printf(" Diffuse  %04.2f %04.2f %04.2f\n",
		   thisMaterial->diffuse[0], thisMaterial->diffuse[1],
		   thisMaterial->diffuse[2]);
	printf(" Transparent: ");
	if ( IsTranspMaterial(thisMaterial) ) {
		printf("yes (%04.2f)\n", thisMaterial->alpha);
	} else {
		printf("no\n");
	}
	printf(" Specular %04.2f %04.2f %04.2f\n",
		   thisMaterial->specular[0], thisMaterial->specular[1],
		   thisMaterial->specular[2]);
	printf(" Shininess %5.1f\n", thisMaterial->shininess );
	printf(" Emission %04.2f %04.2f %04.2f\n",
		   thisMaterial->emission[0], thisMaterial->emission[1],
		   thisMaterial->emission[2]);
	if (thisMaterial->texture) PrintTexture(thisMaterial->texture);
}
