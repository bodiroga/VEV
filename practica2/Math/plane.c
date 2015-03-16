#include "plane.h"
#include "tools.h"


plane *CreatePlane(float a, float b, float c, float d) {

	plane *thisPlane;
	float coef;

	thisPlane = malloc( sizeof(*thisPlane) * 1);

	coef = AGA_MOD_INV(a, b, c);
	thisPlane->nx = a * coef;
	thisPlane->ny = b * coef;
	thisPlane->nz = c * coef;
	thisPlane->d = d * coef;
	thisPlane->is_norm = 1;

	return thisPlane;
}

plane *CreatePlaneCoef(float nx, float ny, float nz, float d) {

	plane *thisPlane;

	thisPlane = malloc( sizeof(*thisPlane) * 1);

	thisPlane->nx = nx;
	thisPlane->ny = ny;
	thisPlane->nz = nz;
	thisPlane->d = d;
	thisPlane->is_norm = 0;

	return thisPlane;
}

// Create a plane aligned with the XY plane: n=(0,0,1) d=0

plane *CreateVoidPlane() {

	plane *thisPlane;

	thisPlane = malloc( sizeof(*thisPlane) * 1);

	thisPlane->nx = 0;
	thisPlane->ny = 0;
	thisPlane->nz = 1;
	thisPlane->d = 0;
	thisPlane->is_norm = 1;

	return thisPlane;
}

void DestroyPlane(plane **thePlane) {
	plane  *thisPlane = *thePlane;
	if( ! thisPlane ) return;
	free( thisPlane );

	*thePlane = NULL;
}


void NormalizePlane(plane *pl) {

	float coef;

	if (pl->is_norm) return; // already normalized
	coef = AGA_MOD_INV(pl->nx, pl->ny, pl->nz);
	pl->nx *= coef;
	pl->ny *= coef;
	pl->nz *= coef;
	pl->d  *= coef;
	pl->is_norm = 1;
}

// Note: first, normalize plane if necessary

void TranslatePlane(plane *pl, float x, float y, float z) {

	if (!pl->is_norm)
		NormalizePlane(pl);
	pl->d = - DOT_PRODUCT(pl->nx, pl->ny, pl->nz,
						  x, y, z);
}

// Note: don't check for zero. Use DISTANCE_EPSILON instead

int WhichSidePlane(plane * pl,
				   float Px,
				   float Py,
				   float Pz) {

	float result;

	result = DOT_PRODUCT(pl->nx, pl->ny, pl->nz,
						 Px, Py, Pz);
	result -= pl->d;

	if (result < - DISTANCE_EPSILON ) return -1; // // onNegativeSide (inside)
	if (result > DISTANCE_EPSILON ) return +1;  //onPossitiveSide (outside)
	return 0; // onPlane
}

// Note: first, normalize plane if necessary

float PlaneSignedDistance(plane * pl,
						  float Px,
						  float Py,
						  float Pz) {
	float result;

	if (!pl->is_norm)
		NormalizePlane(pl);

	result = DOT_PRODUCT(pl->nx, pl->ny, pl->nz,
						 Px, Py, Pz);
	result -= pl->d;

	return result;
}

float PlaneDistance(plane * pl,
					float Px,
					float Py,
					float Pz) {

	return fabs(PlaneSignedDistance(pl, Px, Py, Pz));
}
