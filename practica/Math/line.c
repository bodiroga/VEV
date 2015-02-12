#include <assert.h>
#include "line.h"
#include "tools.h"

// @@ TODO: Create a line given P (ox, oy, oz) and d (dx, dy, dz)

line *CreateLine(float ox, float oy, float oz, float dx, float dy, float dz) {

	line *thisLine;

	thisLine = malloc( sizeof(*thisLine) * 1);

	return thisLine;
}

// @@ TODO: Create a line which passes through two points A and B
//
// Hint: use UnitVectorPP
// Note: Check than A and B are no too close!

line *CreateLinePP(float Ax, float Ay, float Az, float Bx, float By, float Bz) {

}

void DestroyLine(line **theLine) {
	line  *thisLine = *theLine;
	if( ! thisLine ) return;
	free( thisLine );
	*theLine = NULL;
}

// @@ TODO: Give the point corresponding to parameter t. Leave output in Px,Py,Pz

void PointInLine(line *theLine, float t, float *Px, float *Py, float *Pz) {
}

// @@ TODO: Calculate the parameter 'u0' of the line point nearest to P
//
// u0 = D*(P-O) / D*D , where * == dot product

float ParamDistance(line * theLine, float Px, float Py, float Pz) {

}

// @@ TODO: Calculate the minimum distance 'dist' from line to P
//
// dist = mod(P - (O + uD))
// Where u = ParamDistance(theLine, float Px, float Py, float Pz)

float DistanceP(line * theLine, float Px, float Py, float Pz) {

}

void PrintLine(line *theLine) {
	printf("O:(%.2f, %.2f, %.2f) d:(%.2f, %.2f, %.2f)\n",
		   theLine->ox, theLine->oy, theLine->oz,
		   theLine->dx, theLine->dy, theLine->dz);
}
