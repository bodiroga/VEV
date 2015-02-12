#include "trfmStack.h"
#include <stdlib.h>

static struct trfmV *create_trfmV() {

	struct trfmV *newVector;
	int i;
	newVector = malloc(sizeof(*newVector));
	newVector->m = 8; // start with 8 elements
	newVector->ptr = malloc(newVector->m * sizeof(*(newVector->ptr)));
	newVector->i = 0;
	for(i = 0; i < newVector->m; ++i)
		newVector->ptr[i] = CreateTrfm3D();
	return newVector;
}

static void destroy_trfmV(struct trfmV **theVector) {

	struct trfmV *oneVector;
	int i;

	oneVector = *theVector;
	for(i = 0; i < oneVector->m; ++i)
		DestroyTrfm3D(&(oneVector->ptr[i]));
	free(oneVector->ptr);
	oneVector->m = 0;
	oneVector->i = 0;
	free(oneVector);
	*theVector = NULL;
}

static void resize_trfmV(struct trfmV *theVector) {
	// double vector size
	trfm3D **new_ptr;
	int i;

	new_ptr = malloc(2 * theVector->m * (sizeof(*new_ptr)));
	// copy old elements
	for(i = 0; i < theVector->i; ++i) {
		new_ptr[i] = theVector->ptr[i];
	}
	// add space for new elements
	for(i = theVector->i; i < 2 * theVector->m; ++i)
		new_ptr[i] = CreateTrfm3D();
	free(theVector->ptr);
	theVector->ptr = new_ptr;
	theVector->m *= 2;
}

static int can_grow_trfmV(struct trfmV *theVector) {
	return theVector->i < theVector->m;
}

static void push_trfmV(struct trfmV *theVector, trfm3D * T) {
	if (!can_grow_trfmV(theVector))
		resize_trfmV(theVector);
	SetCopyTrfm3D(theVector->ptr[theVector->i++], T);
}

static trfm3D *pop_trfmV(struct trfmV *theVector) {
	if (!theVector->i) return NULL;
	return theVector->ptr[--theVector->i];
}

trfmStack *CreateTrfmStack() {

	trfmStack *theStack;

	theStack = malloc(sizeof(*theStack));

	theStack->V = create_trfmV();
	theStack->top = CreateTrfm3D();

	return theStack;
}

void DestroyTrfmStack(trfmStack **theStack) {
	trfmStack *oneStack = *theStack;

	destroy_trfmV(&(oneStack->V));
	DestroyTrfm3D(&(oneStack->top));
	free(oneStack);
	*theStack = NULL;
}

void PushTrfmStack(trfmStack *st) {
	push_trfmV(st->V, st->top);
}

trfm3D *PopTrfmStack(trfmStack *st) {
	SetCopyTrfm3D(st->top, pop_trfmV(st->V));
	return st->top;
}

void LoadIdentityTrfmStack(trfmStack *st) {
	SetUnitTrfm3D(st->top);
}

void LoadTrfmStack(trfmStack *st, trfm3D * T) {
	SetCopyTrfm3D(st->top, T);
}

void MultTrfmStack(trfmStack *st, trfm3D * T) {
	CompositionTrfm3D(st->top, T);
}

trfm3D *TopTrfmStack(trfmStack *st) {
	return st->top;
}

GLfloat *OpenGLTrfmStack(trfmStack *st) {
	GetGLMatrixTrfm3D(st->top, &(st->m[0]));
	return &(st->m[0]);
}

void PrintTopTrfmStack(trfmStack *st) {
	PrintTrfm3D(st->top);
}
