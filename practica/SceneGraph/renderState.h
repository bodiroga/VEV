#ifndef RENDERSTATE_H
#define RENDERSTATE_H

/**
 * @file   renderState.h
 *
 * @brief Rendering state, including matrices, materials, textures and
 * shaders. It replaces the openGL fixing pipeline. The Rendering state acts
 * like a singleton, and can be retrieved by any function within the engine. The
 * main function for retrieving the render state is RenderStateScene() in scene.h
 *
 *
 */


#include "list.h"
#include "hash.h"
#include "trfmStack.h"
#include "material.h"
#include "light.h"
#include "textureManager.h"
#include "shader.h"

// Possible matrix stacks

#define AGA_MODELVIEW                0
#define AGA_PROJECTION               1
#define AGA_TEXTURE                  2
#define AGA_MODELVIEW_PROJECTION     3


// Possible render targets

#define AGA_RT_FRMEBUFFER            0
#define AGA_RT_TEXTURE               1

// Create texture maps

#define AGA_TMAP_COLOR            0
#define AGA_TMAP_DEPTH            1

typedef struct _RenderTarget {
	// FrameBuffer objects
	GLuint fbo; // for rendering to texture
	GLuint rbo; // render buffer (for depth textures)
	int old_vp[4]; // old viewport coordinates
	Texture *renderTarget;
} RenderTarget;

typedef struct _RenderState {

	// Matrix stacks

	trfmStack *modelViewStack;           // model space to clip space
	trfmStack *projectionStack;          // clip space to NDC space
	trfmStack *textureStack;             // tex. coordinates: object space to texture space
	trfmStack *modelViewProjectionStack; // model space to NDC space

	// Lights

	list *lightList;

	// Materials

	Material *frontMaterial;
	Material *backMaterial;

	// Shaders

	ShaderProgram *shaderProgram;

	hash *rtargets; // string -> RenderTarget objects
	RenderTarget *active_target;

	int drawBBox;

} RenderState;

/**
 * Create a new rendering state
 *
 *
 * @return the new rendering state
 */
RenderState* CreateRenderState();


/**
 * Destroy rendering state
 *
 * @param rs a pointer to the rendering state.
 */
void DestroyRenderState( RenderState **rs );

///////////////////////////////////////////
// Matrix staff

/**
 * Set the matrix mode.
 *
 * @param rs the rendering state
 * @param matrixMode the mode. Possible values are: AGA_MODELVIEW (0), AGA_PROJECTION (1) or AGA_TEXTURE (2)

 */
void MatrixModeRS(RenderState *rs, int matrixMode);

/**
 * Push to a matrix in the stack (the stack is selected by matrixMode)
 *
 * @param rs the rendering state
 * @param matrixMode the mode. Possible values are: AGA_MODELVIEW (0), AGA_PROJECTION (1) or AGA_TEXTURE (2)
 */
void PushRS(RenderState *st, int matrixMode);

/**
 * Pop from a matrix in the stack (the stack is selected by matrixMode)
 *
 * @param rs the rendering state
 * @param matrixMode the mode. Possible values are: AGA_MODELVIEW (0), AGA_PROJECTION (1) or AGA_TEXTURE (2)
 */
trfm3D *PopRS(RenderState *st, int matrixMode);

/**
 * Get top trfm from a matrix in the stack (the stack is selected by matrixMode)
 *
 * @param rs the rendering state
 * @param matrixMode the mode. Possible values are: AGA_MODELVIEW (0), AGA_PROJECTION (1) or AGA_TEXTURE (2)
 */
trfm3D *TopRS(RenderState *st, int matrixMode);

/**
 * Load identity trfm into the top of a matrix in the stack (the stack is selected by matrixMode)
 *
 * @param rs the rendering state
 * @param matrixMode the mode. Possible values are: AGA_MODELVIEW (0), AGA_PROJECTION (1) or AGA_TEXTURE (2)
 */
void LoadIdentityRS(RenderState *st, int matrixMode);

/**
 * Load a trfm into the top of a matrix in the stack (the stack is selected by matrixMode)
 *
 * @param rs the rendering state
 * @param matrixMode the mode. Possible values are: AGA_MODELVIEW (0), AGA_PROJECTION (1) or AGA_TEXTURE (2)
 */
void LoadTrfmRS(RenderState *st, int matrixMode, trfm3D * T);

/**
 * Multiply (compose) a trfm into the top of a matrix in the stack (the stack is selected by matrixMode)
 *
 * @param rs the rendering state
 * @param matrixMode the mode. Possible values are: AGA_MODELVIEW (0), AGA_PROJECTION (1) or AGA_TEXTURE (2)
 */
void MultTrfmRS(RenderState *st, int matrixMode, trfm3D * T);

/**
 * Get openGL matrix of the top of a matrix in the stack (the stack is selected by matrixMode)
 *
 * @param rs the rendering state
 * @param matrixMode the mode. Possible values are: AGA_MODELVIEW (0), AGA_PROJECTION (1) or AGA_TEXTURE (2)
 */
GLfloat *OpenGLRS(RenderState *st, int MatrixMode);

/**
 * Print the top of a matrix in the stack (the stack is selected by matrixMode)
 *
 * @param rs the rendering state
 * @param matrixMode the mode. Possible values are: AGA_MODELVIEW (0), AGA_PROJECTION (1) or AGA_TEXTURE (2)
 */
void PrintTopRS(RenderState *st, int matrixMode);

/**
 * Get the matrix in the stack for AGA_MODELVIEW
 *
 * @param rs the rendering state
 */
trfmStack *modelViewStackRS( RenderState *rs);

/**
 * Get the matrix in the stack for AGA_PROJECTION
 *
 * @param rs the rendering state
 */
trfmStack *projectionStackRS( RenderState *rs);

///////////////////////////////////////////
// Shaders

/**
 * Set the Shader program
 *
 * @param rs the rendering state
 * @param p the ShaderProgram object
 */
void SetShaderProgramRS( RenderState *rs, ShaderProgram *p);

/**
 * Get the current Shader program
 *
 * @param rs the rendering state
 *
 * @return the current ShaderProgram object (NULL if none)
 */
ShaderProgram *GetShaderProgramRS( RenderState *rs);

/**
 * Activate the current Shader program
 *
 * @param rs the rendering state
 */
void ActivateShaderRS(RenderState *rs);

/**
 * Deactivate the current Shader program
 *
 * @param rs the rendering state
 */
void DeActivateShaderRS(RenderState *rs);

///////////////////////////////////////////
// Lights

/**
 * Add a light into the rendering state
 *
 * @param rs the rendering state
 * @param l a light
 */
void AddLightRS(RenderState *rs, Light *l);

/**
 * Remove a light from the rendering state
 *
 * @param rs the rendering state
 * @param l a light
 */
void RemoveLightRS(RenderState *rs, Light *l);

/**
 * Get a list with all active lights
 *
 * @param rs the rendering state
 *
 * @return a list with the active lights
 */
list *LightListRS(RenderState *rs);

///////////////////////////////////////////
// Material

/**
 * Set current front material
 *
 * @param rs the rendering state
 * @param mat the material
 */
void SetFrontMaterialRS(RenderState *rs, Material *mat);

/**
 * Get current front material
 *
 * @param rs the rendering state
 * @param mat the material
 */
Material *GetFrontMaterialRS(RenderState *rs);


/**
 * Set current back material
 *
 * @param rs the rendering state
 * @param mat the material
 */
void SetBackMaterialRS(RenderState *rs, Material *mat);

/**
 * Specify whether to draw BBox-es
 *
 * @param rs the rendering state
 * @param draw 1 draw BBoxes, 0 don't draw BBoxes
 */
void SetBBoxDrawRS(RenderState *rs, int draw);

/**
 * Get draw BBox-es
 *
 * @param rs the rendering state
 */
int GetBBoxDrawRS(RenderState *rs);

/**
 * Create a render target texture with name 'texName'. When activated, the
 * rendering will be addressed to this texture.
 *
 * @param rs RenderState object
 * @param texName the name of the render texture
 * @param mode one of AGA_TEX_COLOR or AGA_TEX_DEPTH
 *
 * @return the render target texture
 */

Texture *SetRenderTextureRS(RenderState *rs, char *texName, int mode);

/**
 * Set state to render into texture 'texName' instead of the
 * framebuffer. texName has to be initialized first using function
 * SetRenderTextureRS
 *
 * @param rs RenderState object
 * @param texName the name of the target texture
 */

void RenderToTextureRS(RenderState *rs, char *texName);

/**
 * Set state to render into framebuffer.
 *
 * @param rs RenderState object
 */

void RenderToFBRS(RenderState *rs);

#endif
