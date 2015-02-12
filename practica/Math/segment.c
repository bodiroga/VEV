#include <assert.h>
#include "segment.h"
#include "tools.h"

segment *CreateSegment(float ox, float oy, float oz, float dx, float dy, float dz, float a, float b) {

	segment *thisSegment;

	thisSegment = malloc( sizeof(*thisSegment) * 1);

	thisSegment->ox = ox;
	thisSegment->oy = oy;
	thisSegment->oz = oz;

	thisSegment->dx = dx;
	thisSegment->dy = dy;
	thisSegment->dz = dz;

	thisSegment->a = a;
	thisSegment->b = b;

	return thisSegment;
}

segment *CreateSegmentPP(float Ax, float Ay, float Az, float Bx, float By, float Bz) {

	float ABx, ABy, ABz;

	ABx = Bx - Ax;
	ABy = By - Ay;
	ABz = Bz - Az;
	assert(!(V_IS_ZERO(ABx, ABy, ABz)));
	return CreateSegment(Ax, Ay, Az, ABx, ABy, ABz, 0.0, 1.0);
}

void DestroySegment(segment **theSegment) {
	segment  *thisSegment = *theSegment;
	if( ! thisSegment ) return;
	free( thisSegment );

	*theSegment = NULL;
}

// u0 = D*(P-O) / D*D , where * == dot product

float ParamDistanceSegment(segment * theSegment, float Px, float Py, float Pz) {

	float dd;
	float pox, poy, poz;
	float u0;

	dd = DOT_PRODUCT(theSegment->dx, theSegment->dy, theSegment->dz,
					 theSegment->dx, theSegment->dy, theSegment->dz);
	assert(dd > DISTANCE_EPSILON);

	pox = Px - theSegment->ox;
	poy = Py - theSegment->oy;
	poz = Pz - theSegment->oz;

	u0 = DOT_PRODUCT(theSegment->dx, theSegment->dy, theSegment->dz, pox, poy, poz) / dd;
	if (u0 < theSegment->a) return theSegment->a;
	if (u0 > theSegment->b) return theSegment->b;
	return u0;
}

// d = mod(P - (O + uD))
// Non u = ParamDistance(theSegment, float Px, float Py, float Pz)

float DistanceSegmentP(segment * theSegment, float Px, float Py, float Pz) {

	float wx, wy, wz;
	float u0;

	u0 = ParamDistanceSegment(theSegment, Px, Py, Pz);
	if (u0 == theSegment->a) {
		wx = Px - theSegment->ox;
		wy = Py - theSegment->oy;
		wz = Pz - theSegment->oz;
	} else {
		wx = Px - (theSegment->ox + u0*theSegment->dx);
		wy = Py - (theSegment->oy + u0*theSegment->dy);
		wz = Pz - (theSegment->oz + u0*theSegment->dz);
	}
	return AGA_MOD(wx, wy, wz);
}
