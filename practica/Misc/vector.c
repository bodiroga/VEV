#include "vector.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct asvector {
	void *ptr;
	size_t N;        // number of elements in the vector
	size_t capacity; // how many elements before resizing
	unsigned char b; // how many bytes per element
};

static struct asvector *createVector(unsigned char b) {
	struct asvector * v = malloc(sizeof(*v));
	v->ptr = NULL;
	v->b = b;
	v->N = 0;
	v->capacity = 0;
	return v;
}

static struct asvector *createVectorV(unsigned char b, void *ptr, size_t N) {
	struct asvector * v = malloc(sizeof(*v));
	v->ptr = malloc(N * b);
	memcpy(v->ptr, ptr, N * b);
	v->b = b;
	v->N = N;
	v->capacity = N;
	return v;
}

static struct asvector *createVectorN(unsigned char b, size_t c) {
	struct asvector * v = malloc(sizeof(*v));
	v->ptr = malloc(c * b);
	v->b = b;
	v->N = 0;
	v->capacity = c;
	return v;
}

static void destroyVector(struct asvector **vv) {
	struct asvector *v = *vv;
	if (v->ptr) free(v->ptr);
	free(v);
	*vv = NULL;
}

static void resizeVector(struct asvector *v) {
	void *nptr;

	if (!v->capacity) v->capacity = 1;
	nptr = malloc(v->capacity * 2 * v->b);
	memcpy(nptr, v->ptr, v->N * v->b);
	free(v->ptr);
	v->capacity *= 2;
	v->ptr = nptr;
}

static void pushVector(struct asvector *v, void *elemPtr) {
	if (v->N == v->capacity)
		resizeVector(v);
	memcpy(v->ptr + v->N * v->b, elemPtr, v->b);
	v->N++;
}

static void popVector(struct asvector *v) {
	if (v->N) --v->N;
}

static void topVector(struct asvector *v, void *elemPtr) {
	memcpy(elemPtr, v->ptr + (v->N - 1) * v->b, v->b);
}

static void atVector(struct asvector *v, size_t i, void *elemPtr) {
	memcpy(elemPtr, v->ptr + i * v->b, v->b);
}

static void *atPtrVector(struct asvector *v, size_t i) {
	return (v->ptr + i * v->b);
}

static size_t sizeVector(struct asvector *v) {
	return v->N;
}

VectorI *CreateVectorI() {
	VectorI *vi = malloc(sizeof(*vi));
	vi->v = createVector(sizeof(int));
	return vi;
}

VectorI *CreateVectorIv(int *ptr, size_t N) {
	VectorI *vi = malloc(sizeof(*vi));
	vi->v = createVectorV(sizeof(int), ptr, N);
	return vi;
}

VectorI *CreateVectorIN(size_t N) {
	VectorI *vi = malloc(sizeof(*vi));
	vi->v = createVectorN(sizeof(int), N);
	return vi;
}

void DestroyVectorI(VectorI **vvi) {
	VectorI *vi = *vvi;
	destroyVector(&(vi->v));
	*vvi = NULL;
}

void pushVectorI(VectorI *vi, int a) {
	pushVector(vi->v, &a);
}

int popVectorI(VectorI *vi) {
	int r;
	topVector(vi->v, &r);
	popVector(vi->v);
	return r;
}

int atVectorI(VectorI *vi, size_t i) {
	int r;
	atVector(vi->v, i, &r);
	return r;
}

int *atPtrVectorI(VectorI *vi, size_t i) {
	return ( int *) atPtrVector(vi->v, i);
}

size_t sizeVectorI(VectorI *vi) {
	return sizeVector(vi->v);
}

void printVectorI(VectorI *vi) {
	int i, m;
	int *f = (int *) vi->v->ptr;
	for(i = 0; i < vi->v->N; ++i) {
		if (i %3 == 0) printf("\n");
		printf("%d ", f[i]);
	}
	printf("\n");
}

VectorF *CreateVectorF() {
	VectorF *vf = malloc(sizeof(*vf));
	vf->v = createVector(sizeof(float));
	return vf;
}

VectorF *CreateVectorFv(float *ptr, size_t N) {
	VectorF *vf = malloc(sizeof(*vf));
	vf->v = createVectorV(sizeof(float), ptr, N);
	return vf;
}

VectorF *CreateVectorFN(size_t N) {
	VectorF *vf = malloc(sizeof(*vf));
	vf->v = createVectorN(sizeof(float), N);
	return vf;
}

void DestroyVectorF(VectorF **vvf) {
	VectorF *vf = *vvf;
	destroyVector(&(vf->v));
	*vvf = NULL;
}

void pushVectorF(VectorF *vf, float a) {
	pushVector(vf->v, &a);
}

float popVectorF(VectorF *vf) {
	float r;
	topVector(vf->v, &r);
	popVector(vf->v);
	return r;
}

float atVectorF(VectorF *vf, size_t i) {
	float r;
	atVector(vf->v, i, &r);
	return r;
}

float *atPtrVectorF(VectorF *vf, size_t i) {
	return ( float *) atPtrVector(vf->v, i);
}


size_t sizeVectorF(VectorF *vf) {
	return sizeVector(vf->v);
}

void printVectorF(VectorF *vf) {
	int i, m;
	float *f = (float *) vf->v->ptr;
	for(i = 0; i < vf->v->N; ++i) {
		if (i %3 == 0) printf("\n");
		printf("%4.2f ", f[i]);
	}
	printf("\n");
}

/* int main() { */

/*   VectorI *iv; */
/*   VectorF *fv; */
/*   int i; */
/*   iv = CreateVectorIN(111); */
/*   for(i = 0; i < 111; ++i) */
/*	pushVectorI(iv, i); */

/*   printVectorI(iv); */

/*   fv = CreateVectorFN(12); */
/*   for(i = 0; i < 12; ++i) */
/*	pushVectorF(fv, i); */
/*   printVectorF(fv); */

/*   DestroyVectorI(&iv); */
/*   DestroyVectorF(&fv); */
/*   return 0; */
/* } */
