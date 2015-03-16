#include <assert.h>
#include "line.h"
#include "tools.h"

// @@ TODO: Create a line given P (ox, oy, oz) and d (dx, dy, dz)

line *CreateLine(float ox, float oy, float oz, float dx, float dy, float dz) {

	line *thisLine;

	thisLine = malloc( sizeof(*thisLine) * 1);

	thisLine->ox = ox;
	thisLine->oy = oy;
	thisLine->oz = oz;

	thisLine->dx = dx;
	thisLine->dy = dy;
	thisLine->dz = dz;

	return thisLine;
}

// @@ TODO: Create a line which passes through two points A and B
//
// Hint: use UnitVectorPP
// Note: Check than A and B are no too close!

line *CreateLinePP(float Ax, float Ay, float Az, float Bx, float By, float Bz) {

	float dx, dy, dz;
	float mod;

	mod = UnitVectorPP(&dx, &dy, &dz,
					   Ax, Ay, Az,
					   Bx, By, Bz);

	// check that A and B are not the same point
	assert(mod > DISTANCE_EPSILON);

	return CreateLine(Ax, Ay, Az, dx, dy, dz);
}

void DestroyLine(line **theLine) {
	line  *thisLine = *theLine;
	if( ! thisLine ) return;
	free( thisLine );
	*theLine = NULL;
}

// @@ TODO: Give the point corresponding to parameter t. Leave output in Px,Py,Pz

void PointInLine(line *theLine, float t, float *Px, float *Py, float *Pz) {
	*Px = theLine->ox + t*theLine->dx;
	*Py = theLine->oy + t*theLine->dy;
	*Pz = theLine->oz + t*theLine->dz;
}

// @@ TODO: Calculate the parameter 'u0' of the line point nearest to P
//
// u0 = D*(P-O) / D*D , where * == dot product

float ParamDistance(line * theLine, float Px, float Py, float Pz) {

	float pox, poy, poz;
	float u0;
	float dd;

	dd = DOT_PRODUCT(theLine->dx, theLine->dy, theLine->dz,
					 theLine->dx, theLine->dy, theLine->dz);
	assert(dd > DISTANCE_EPSILON * DISTANCE_EPSILON);

	pox = Px - theLine->ox;
	poy = Py - theLine->oy;
	poz = Pz - theLine->oz;

	u0 = DOT_PRODUCT(theLine->dx, theLine->dy, theLine->dz, pox, poy, poz) / dd;
	return u0;
}

// @@ TODO: Calculate the minimum distance 'dist' from line to P
//
// dist = mod(P - (O + uD))
// Where u = ParamDistance(theLine, float Px, float Py, float Pz)

float DistanceP(line * theLine, float Px, float Py, float Pz) {

	float wx, wy, wz;
	float u0;

	u0 = ParamDistance(theLine, Px, Py, Pz);

	wx = Px - (theLine->ox + u0*theLine->dx);
	wy = Py - (theLine->oy + u0*theLine->dy);
	wz = Pz - (theLine->oz + u0*theLine->dz);

	return AGA_MOD(wx, wy, wz);
}

void PrintLine(line *theLine) {
	printf("O:(%.2f, %.2f, %.2f) d:(%.2f, %.2f, %.2f)\n",
		   theLine->ox, theLine->oy, theLine->oz,
		   theLine->dx, theLine->dy, theLine->dz);
}
