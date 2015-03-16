#ifndef MATERIAL_H
#define MATERIAL_H

//#include <GL/gl.h>
#include "texture.h"


#define AGA_MATERIAL_DULL       11      // uses only ambient & diffuse, and both have the same value
#define AGA_MATERIAL_BRIGHT     12      // uses all the components

//typedef struct _List list;

typedef struct _Material {
	// The default values that we use
	// are those defined by OpenGL
	char    *library;
	char    *name;

	float    ambient[3];            // GL_AMBIENT  : RGB default (0.2, 0.2, 0.2)
	float    diffuse[3];            // GL_DIFFUSE  : RGB default (0.8, 0.8, 0.8)
	int      has_specular;
	float    specular[3];           // GL_SPECULAR : RGB default (0.0, 0.0, 0.0)
	float    alpha;                 // alpha value. Default 1
	float    shininess;             // GL_SHININESS: default 0.0 ; range [0,128]
	int      has_emission;
	float    emission[3];           // GL_EMISSION : default (0.0,  0.0, 0.0, 1.0)

	Texture *texture;               // Texture
	Texture *bumpmap;		// Bump-mapping texture
} Material;

void   DestroyMaterial( Material **theMaterial );

/**
 * Create void material with default values and no name
 *
 * @return new material
 */

Material *CreateVoidMaterial();

/**
 * Create a named material
 *
 * @param libraryName the library
 * @param materialName the material name
 *
 * @return new material
 */

Material *CreateMaterial( char *libraryName, char* materialName);

/**
 * Set material name
 *
 * @param thisMaterial the material
 * @param libraryName the library
 * @param materialName the name
 */
void SetNameMaterial(Material *thisMaterial, char *libraryName, char* materialName);

/**
 * Get material name
 *
 * @param thisMaterial the material
 * @param libraryName pointer to a string where library name will be stored
 * @param materialName pointer to a string where material name will be stored
 */
void GetNameMaterial(Material *thisMaterial, char **libraryName, char **materialName);

/**
 * Set material transparency
 *
 * @param thisMaterial the material
 * @param alpha the new alpha value (must be in [0..1])
 */

void SetAlphaMaterial(Material *thisMaterial, float alpha);

/**
 * Get material transparency
 *
 * @param thisMaterial the material
 * @return the material alpha value (in [0..1])
 */

float GetAlphaMaterial(Material *thisMaterial);

/**
 * Set material difusse parameters
 *
 * @param thisMaterial the material
 * @param rgb pointer to a 3-vector rgb
 */

void SetDiffuseMaterial(Material *thisMaterial, float *rgb);

/**
 * Get material difusse parameters
 *
 * @param thisMaterial the material
 * @return a pointer to a 3-vector with rgb values
 */

float *GetDiffuseMaterial(Material *thisMaterial);

/**
 * Set material ambient parameters
 *
 * @param thisMaterial the material
 * @param rgb pointer to a 3-vector rgb
 */

void SetAmbientMaterial(Material *thisMaterial, float *rgb);

/**
 * Get material ambient parameters
 *
 * @param thisMaterial the material
 * @return a pointer to a 3-vector with rgb values
 */
float *GetAmbientMaterial(Material *thisMaterial);

/**
 * Set material specular parameters
 *
 * @param thisMaterial the material
 * @param rgba pointer to a 3-vector rgb
 * @note
 * Setting the specular channel causes the material to render also specular highlights.
 * @see MaterialSpecular
 */

void SetSpecularMaterial(Material *thisMaterial, float *rgb, float shine);

/**
 * Get material specular parameters
 *
 * @param thisMaterial the material
 * @return a pointer to a 3-vector with rgb values
 */
float *GetSpecularMaterial(Material *thisMaterial);

/**
 * Get material shininess
 *
 * @param thisMaterial the material
 * @return the shininess
 */
float GetShininessMaterial(Material *thisMaterial);

/**
 * Set/unset specular channel for this material
 *
 * @param thisMaterial the material
 * @param yesorno 0 disable specular. Any other enable specular.
 */

void HasSpecularMaterial(Material *thisMaterial, int yesorno);

/**
 * Set material specular parameters
 *
 * @param thisMaterial the material
 * @param rgb pointer to a 3-vector rgb
 * @note
 * Setting the emission channel causes the material to render also emmisive.
 * @see MaterialEmission
 */

void SetEmissionMaterial(Material *thisMaterial, float *rgb);

/**
 * Get material emission parameters
 *
 * @param thisMaterial the material
 * @return a pointer to a 3-vector with rgb values
 */
float *GetEmissionMaterial(Material *thisMaterial);


/**
 * Set/unset emission channel for this material
 *
 * @param thisMaterial the material
 * @param yesorno 0 disable emission. Any other enable emission.
 */

void HasEmissionMaterial(Material *thisMaterial, int yesorno);

/**
 * Attach a texture to a material
 *
 * @param thisMaterial the material
 * @param theTexture texture to be attached
 */

void SetTextureMaterial (Material *thisMaterial, Texture * theTexture);

/**
 * Get the texturematerial
 *
 * @param thisMaterial the material
 * @return the texture or NULL
 */

Texture *GetTextureMaterial (Material *thisMaterial);

/**
 * Attach a bump-mapping texture to a material
 *
 * @param thisMaterial the material
 * @param theTexture texture to be attached
 */

void SetBumpMapTextureMaterial (Material *thisMaterial, Texture * theTexture);

/**
 * Get the texturematerial
 *
 * @param thisMaterial the material
 * @return the texture or NULL
 */

Texture *GetBumpMapTextureMaterial (Material *thisMaterial);

/**
 * Check if a material is transparent
 *
 * @param thisMaterial the material
 *
 * @return a boolean indicating whether material is transparent
 */
int IsTranspMaterial(Material *thisMaterial);

/**
 * Check whether material has (normal) texture
 *
 * @param thisMaterial the material
 *
 * @return a boolean indicating whether material has texture
 */

int HasTextureMaterial(Material *thisMaterial);

/**
 * Check whether material has bump texture
 *
 * @param thisMaterial the material
 *
 * @return a boolean indicating wether material has bump texture
 */
int HasBumpMaterial(Material *thisMaterial);

/**
 * Copy material values
 *
 * @param thisMaterial  destination material
 * @param source source material
 */

void CopyValuesMaterial( Material *thisMaterial, Material *source );

/**
 * Check that material values are appropiate and fix in case thy are not
 *
 * @param thisMaterial the material
 */

void CheckMaterial( Material *thisMaterial );

/**
 * Compare materials according to their values
 *
 * @param thisMaterial the material
 */

int  CompareMaterial( Material *oneMaterial, Material *otherMaterial );

/**
 * Print material on screen
 *
 * @param thisMaterial the Material
 */

void PrintMaterial( Material *thisMaterial );


#endif /* MATERIAL_H */
