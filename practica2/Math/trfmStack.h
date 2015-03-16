#ifndef MATRIXSTACK_H
#define MATRIXSTACK_H

#include <GL/glew.h> // gor GLfloat
#include <GL/glut.h> // gor GLfloat
#include "trfm3D.h"

/**
 * @file   trfmStack.h
 *
 * @brief  Emulation of the openGL matrix stack.
 */


struct trfmV {
	trfm3D **ptr;
	int m;
	int i; // past-end index
};

typedef struct _trfmStack {
	struct trfmV *V;
	trfm3D *top;
	float m[16];
} trfmStack;


/**
 * Create a stack for storing trfm3D objects
 *
 * @return the new stack (NULL if error)
 */
trfmStack *CreateTrfmStack();

/**
 * Destroy a trfm3D stack and all trfm3D objects it stores
 *
 * @param theStack a pointer to a pointer of a termStack object.
 */

void DestroyTrfmStack(trfmStack **theStack);


/**
 * Push a copy of the top element into the stack
 *
 * @param st the stack
 */
void PushTrfmStack(trfmStack *st);

/**
 * Remove one element of the stack, and place it on top
 *
 * @param st the stack
 * @return the removed element, which is now on top.
 */
trfm3D *PopTrfmStack(trfmStack *st);

/**
 * Get the top of the stack
 *
 * @param st the stack
 *
 * @return the top element
 */
trfm3D *TopTrfmStack(trfmStack *st);

/**
 * Set the top element as the identity trfm
 *
 * @param st the stack
 */
void LoadIdentityTrfmStack(trfmStack *st);

/**
 * Load a trfm3D to the top element
 *
 * @param st the stack
 */
void LoadTrfmStack(trfmStack *st, trfm3D * T);

/**
 * Compose top element with given trfm3D, so that
 *
 * new_top = top * T
 *
 * @param st the stack
 * @param T the new trfm3D
 */
void MultTrfmStack(trfmStack *st, trfm3D * T);

/**
 * Get a, openGL matrix representation of the top element.
 *
 * @param st the stack
 *
 * @return a vector of 16 elements representing the oprenGL 4x4 matrix of the
 * top element.
 */
GLfloat *OpenGLTrfmStack(trfmStack *st);

/**
 * Print the top trfm3D
 *
 * @param st the stack
 */
void PrintTopTrfmStack(trfmStack *st);

#endif
