#include <stdlib.h>
#include "tools.h"
#include "renderState.h"

RenderState* CreateRenderState() {

	RenderState *newRenderState;

	newRenderState = malloc(sizeof(*newRenderState));
	newRenderState->modelViewStack = CreateTrfmStack();
	newRenderState->projectionStack = CreateTrfmStack();
	newRenderState->textureStack = CreateTrfmStack();

	newRenderState->modelViewProjectionStack = CreateTrfmStack();

	newRenderState->shaderProgram = NULL;
	newRenderState->lightList = CreateVoidList();
	newRenderState->frontMaterial = NULL;
	newRenderState->backMaterial = NULL;

	newRenderState->rtargets = CreateVoidHash();
	newRenderState->active_target = NULL;
	newRenderState->drawBBox = 0;

	return newRenderState;
}

void DestroyRenderState( RenderState **rs ) {

	RenderState *theRenderState = *rs;

	DestroyTrfmStack(&(theRenderState->modelViewStack));
	DestroyTrfmStack(&(theRenderState->projectionStack));
	DestroyTrfmStack(&(theRenderState->textureStack));
	DestroyTrfmStack(&(theRenderState->modelViewProjectionStack));
	DestroyList(&(theRenderState->lightList));
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0); // just in case
	RenderTarget *rtgt;
	for( rtgt = StartLoopHash(theRenderState->rtargets); rtgt; rtgt = GetNextHash(theRenderState->rtargets) ) {
		//TODO release fbo and rbo
		if (rtgt->rbo) glDeleteRenderbuffers(1, &rtgt->rbo);
		if (rtgt->fbo) glDeleteFramebuffers(1, &rtgt->fbo);
		free(rtgt);
	}
	free(theRenderState);
	*rs = NULL;
}

///////////////////////////////////////////////////////////////
// Matrix stack stuff


static trfmStack *choose_trfm_stack(RenderState *rs, int matrixMode) {

	trfmStack *res;

	res = NULL;

	switch (matrixMode) {
	case AGA_MODELVIEW:
		res = rs->modelViewStack;
		break;
	case AGA_PROJECTION:
		res  = rs->projectionStack;
		break;
	case AGA_TEXTURE:
		res = rs->textureStack;
		break;
	case AGA_MODELVIEW_PROJECTION:
		res = rs->modelViewProjectionStack;
		break;
	default:
		fprintf(stderr, "[E] choose_trfm_stack: invalid matrix mode %d\n", matrixMode);
		exit(1);
	}
	return res;
}

// run after stack top changes

static void after_update_top(RenderState *rs, int matrixMode) {
	if(matrixMode == AGA_MODELVIEW || matrixMode == AGA_PROJECTION) {
		// update modelViewProjection matrix
		SetCopyTrfm3D(TopTrfmStack(rs->modelViewProjectionStack), TopTrfmStack(rs->projectionStack));
		CompositionTrfm3D(TopTrfmStack(rs->modelViewProjectionStack), TopTrfmStack(rs->modelViewStack));
	}
}

void PushRS(RenderState *rs, int matrixMode) {
	PushTrfmStack(choose_trfm_stack(rs, matrixMode));
}

trfm3D *PopRS(RenderState *rs, int matrixMode) {

	trfm3D *aux;
	trfmStack *st;
	st = choose_trfm_stack(rs, matrixMode);

	aux = PopTrfmStack(st);
	after_update_top(rs, matrixMode);
	return aux;
}

trfm3D *TopRS(RenderState *rs, int matrixMode) {
	return TopTrfmStack(choose_trfm_stack(rs, matrixMode));
}

void LoadIdentityRS(RenderState *rs, int matrixMode) {
	LoadIdentityTrfmStack(choose_trfm_stack(rs, matrixMode));
	after_update_top(rs, matrixMode);
}

void LoadTrfmRS(RenderState *rs, int matrixMode, trfm3D * T) {
	LoadTrfmStack(choose_trfm_stack(rs, matrixMode), T);
	after_update_top(rs, matrixMode);
}

void MultTrfmRS(RenderState *rs, int matrixMode, trfm3D * T) {
	MultTrfmStack(choose_trfm_stack(rs, matrixMode), T);
	after_update_top(rs, matrixMode);
}

GLfloat *OpenGLRS(RenderState *rs, int matrixMode) {
	return OpenGLTrfmStack(choose_trfm_stack(rs, matrixMode));
}


void PrintTopRS(RenderState *rs, int matrixMode) {
	PrintTopTrfmStack(choose_trfm_stack(rs, matrixMode));
}

///////////////////////////////////////////
// Shaders

void SetShaderProgramRS(RenderState *rs, ShaderProgram *program) {
	if(rs->shaderProgram != NULL)
		DeactivateShader(rs->shaderProgram);
	rs->shaderProgram = program;
	ActivateShader(program);
}

ShaderProgram *GetShaderProgramRS(RenderState *rs) {
	return rs->shaderProgram;
}

///////////////////////////////////////////
// Lights

void AddLightRS(RenderState *rs, Light *l) {
	AddLast(rs->lightList, l);
}

void RemoveLightRS(RenderState *rs, Light *l) {
	RemoveFromList(rs->lightList, l);
}

list *LightListRS(RenderState *rs) {
	return rs->lightList;
}

///////////////////////////////////////////
// Materials

void SetFrontMaterialRS(RenderState *rs, Material *mat) {
	rs->frontMaterial = mat;
}

Material *GetFrontMaterialRS(RenderState *rs) {
	return rs->frontMaterial;
}

void SetBackMaterialRS(RenderState *rs, Material *mat) {
	rs->backMaterial = mat;
}

void SetBBoxDrawRS(RenderState *rs, int draw) {
	rs->drawBBox = (draw != 0);
}

int GetBBoxDrawRS(RenderState *rs) {
	return rs->drawBBox;
}

/////////////////////////////////////
// Prepare fbos etc. to render to texture "texName"

// mode : AGA_TEX_COLOR, AGA_TEX_DEPTH (see texture.h)

Texture *SetRenderTextureRS(RenderState *rs, char *texName, int mode) {

	RenderTarget *rtgt;
	Texture *tex;
	int h, w; // texture width and heigth
	GLenum attachment;

	h = w = 512;
	if (mode == AGA_TEX_DEPTH) {
		tex = SceneRegisterDepthMapTexture(texName, h, w);
	} else {
		// mode == AGA_TEX_COLOR
		tex = SceneRegisterColorMapTexture(texName, h, w);
	}

	rtgt = FindHashElement(rs->rtargets, texName);
	if (!rtgt) {
		rtgt = malloc(sizeof(*rtgt));
		rtgt->fbo = 0;
		rtgt->rbo = 0;
		rtgt->old_vp[0] = 0;
		rtgt->old_vp[1] = 0;
		rtgt->old_vp[2] = 0;
		rtgt->old_vp[3] = 0;
		InsertHashElement(rs->rtargets, texName, rtgt);
	}
	rtgt->renderTarget = tex;
	// Set texture fbo
	if(!rtgt->fbo) glGenFramebuffers(1, &(rtgt->fbo));
	glBindFramebuffer(GL_FRAMEBUFFER, rtgt->fbo);
	if(mode == AGA_TEX_DEPTH) {
		// Instruct openGL that we won't bind a color texture with the currently binded FBO
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		// attach depth texture to current fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex->textureId, 0);
	} else {
		// Color maps also need depth buffer handling
		if(!rtgt->rbo) glGenRenderbuffers(1, &rtgt->rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rtgt->rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		// attach renderbuffer object to the current fbo
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rtgt->rbo);
		// attach color texture to current fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->textureId, 0);
	}
	//Does the GPU support current FBO configuration?
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "[E] This GPU does not support FBOs\n");
		exit(1);
	}
	// switch back to window-system-provided framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return rtgt->renderTarget;
}

void RenderToTextureRS(RenderState *rs, char *texName) {
	RenderTarget *rtgt;

	rtgt = FindHashElement(rs->rtargets, texName);
	if (!rtgt) {
		fprintf(stderr, "[E] No texture to render to\n");
		exit(1);
	}
	// Save old viewport
	glGetIntegerv(GL_VIEWPORT, &(rtgt->old_vp[0]));
	//set the viewport to be the size of the texture
	glBindFramebuffer(GL_FRAMEBUFFER, rtgt->fbo);
	if(rtgt->renderTarget->type != AGA_TEX_DEPTH) {
		glBindRenderbuffer(GL_RENDERBUFFER, rtgt->rbo);
	}
	glViewport(0,0, rtgt->renderTarget->width, rtgt->renderTarget->height);
	rs->active_target = rtgt;
}

void RenderToFBRS(RenderState *rs) {
	RenderTarget *rtgt;

	if (!rs->active_target) return; // Already rendering to FrameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if(rs->active_target->renderTarget->type != AGA_TEX_DEPTH) {
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	} else {
		glDrawBuffer(GL_BACK);
		glReadBuffer(GL_BACK);
	}
	// Set original viewport
	glViewport(rs->active_target->old_vp[0],rs->active_target->old_vp[1],
			   rs->active_target->old_vp[2], rs->active_target->old_vp[3]);
	rs->active_target = NULL;
}
