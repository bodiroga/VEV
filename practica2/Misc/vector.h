// Simple classes for handling int and float vectors

#include <stddef.h>

struct asvector; // forward declaration

typedef struct _intVector {
	struct asvector *v;
} VectorI;

typedef struct _floatVector {
	struct asvector *v;
} VectorF;

VectorI *CreateVectorI();

/**
 * Create an empty VectorI with capacity N.
 *
 * @param N the capacity of the vector
 *
 * @return the new vector
 */
VectorI *CreateVectorIN(size_t N);

VectorI *CreateVectorIv(int *ptr, size_t N);

void DestroyVectorI(VectorI **vvi);
void pushVectorI(VectorI *vi, int a);
int popVectorI(VectorI *vi);
int atVectorI(VectorI *vi, size_t i);
int *atPtrVectorI(VectorI *vi, size_t i);
size_t sizeVectorI(VectorI *vi);
void printVectorI(VectorI *vi);

VectorF *CreateVectorF();
VectorF *CreateVectorFN(size_t N); // create vector with capacity N
VectorF *CreateVectorFv(float *ptr, size_t N);
void DestroyVectorF(VectorF **vvf);
void pushVectorF(VectorF *vf, float a);
float popVectorF(VectorF *vf);
float atVectorF(VectorF *vf, size_t i);
float *atPtrVectorF(VectorF *vf, size_t i);
size_t sizeVectorF(VectorF *vf);
void printVectorF(VectorF *vf);
