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

	// p(u) = o + ud
	// o serán las coordenadas de A, el origen del vector
	// d se calculará usando UnitVectorPP de tools.c

	float Dx, Dy, Dz;
	float distance;

	distance = UnitVectorPP(&Dx, &Dy, &Dz, Ax, Ay, Az, Bx, By, Bz);

	if (distance > DISTANCE_EPSILON)
		return CreateLine(Ax, Ay, Az, Dx, Dy, Dz);
	else
		return CreateLine(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

void DestroyLine(line **theLine) {
	line  *thisLine = *theLine;
	if( ! thisLine ) return;
	free( thisLine );
	*theLine = NULL;
}

// @@ TODO: Give the point corresponding to parameter t. Leave output in Px,Py,Pz

void PointInLine(line *theLine, float t, float *Px, float *Py, float *Pz) {

	*Px = theLine->ox + t*(theLine->dx);
	*Py = theLine->oy + t*(theLine->dy);
	*Pz = theLine->oz + t*(theLine->dz);

}

// @@ TODO: Calculate the parameter 'u0' of the line point nearest to P
//
// u0 = D*(P-O) / D*D , where * == dot product

float ParamDistance(line * theLine, float Px, float Py, float Pz) {

	float resta_x, resta_y, resta_z;
	float producto_numerador, producto_denominador;

	resta_x = Px - theLine->ox;
	resta_y = Py - theLine->oy;
	resta_z = Pz - theLine->oz;

	producto_numerador = DOT_PRODUCT(theLine->dx, theLine->dy, theLine->dz, resta_x, resta_y, resta_z);

	producto_denominador = DOT_PRODUCT(theLine->dx, theLine->dy, theLine->dz, theLine->dx, theLine->dy, theLine->dz);

	return producto_numerador/producto_denominador;

}

// @@ TODO: Calculate the minimum distance 'dist' from line to P
//
// dist = mod(P - (O + uD))
// Where u = ParamDistance(theLine, float Px, float Py, float Pz)

float DistanceP(line * theLine, float Px, float Py, float Pz) {

	float u, distance_x, distance_y, distance_z;

	u = ParamDistance(theLine, Px, Py, Pz);

	distance_x = Px - (theLine->ox + u*theLine->dx);
	distance_y = Py - (theLine->oy + u*theLine->dy);
	distance_z = Pz - (theLine->oz + u*theLine->dz);

	return AGA_MOD(distance_x, distance_y, distance_z);


}

void PrintLine(line *theLine) {
	printf("O:(%.2f, %.2f, %.2f) d:(%.2f, %.2f, %.2f)\n",
		   theLine->ox, theLine->oy, theLine->oz,
		   theLine->dx, theLine->dy, theLine->dz);
}
