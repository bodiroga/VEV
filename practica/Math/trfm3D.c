#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include "tools.h"
#include "trfm3D.h"

/*! This static structure can only be used in "Add***" functions
  Do not use elsewhere because it can be nested within different functions
*/
static GLfloat LOCAL_M[16];

void DestroyTrfm3D( trfm3D **theTrfm3D ) {
	trfm3D  *thisTrfm3D = *theTrfm3D;
	if( ! thisTrfm3D ) return;
	free( thisTrfm3D );

	*theTrfm3D = NULL;
}

trfm3D *CreateTrfm3D() {
	trfm3D  *unitTrfm;

	unitTrfm = malloc( sizeof(*unitTrfm) * 1 );

	unitTrfm->r1X = 1.0f;
	unitTrfm->r1Y = 0.0f;
	unitTrfm->r1Z = 0.0f;
	unitTrfm->r2X = 0.0f;
	unitTrfm->r2Y = 1.0f;
	unitTrfm->r2Z = 0.0f;
	unitTrfm->r3X = 0.0f;
	unitTrfm->r3Y = 0.0f;
	unitTrfm->r3Z = 1.0f;
	unitTrfm->scl = 1.0f;
	unitTrfm->trX = 0.0f;
	unitTrfm->trY = 0.0f;
	unitTrfm->trZ = 0.0f;
	unitTrfm->dX  = 0.0f;
	unitTrfm->dY  = 0.0f;
	unitTrfm->dZ  = 0.0f;
	unitTrfm->dW  = 1.0f;

	return unitTrfm;
}

trfm3D *CreateCopyTrfm3D( const trfm3D *thisTrfm3D ) {
	trfm3D *newTrfm3D;

	newTrfm3D = malloc( sizeof(*newTrfm3D) * 1 );

	newTrfm3D->r1X = thisTrfm3D->r1X;
	newTrfm3D->r1Y = thisTrfm3D->r1Y;
	newTrfm3D->r1Z = thisTrfm3D->r1Z;
	newTrfm3D->r2X = thisTrfm3D->r2X;
	newTrfm3D->r2Y = thisTrfm3D->r2Y;
	newTrfm3D->r2Z = thisTrfm3D->r2Z;
	newTrfm3D->r3X = thisTrfm3D->r3X;
	newTrfm3D->r3Y = thisTrfm3D->r3Y;
	newTrfm3D->r3Z = thisTrfm3D->r3Z;
	newTrfm3D->scl = thisTrfm3D->scl;
	newTrfm3D->trX = thisTrfm3D->trX;
	newTrfm3D->trY = thisTrfm3D->trY;
	newTrfm3D->trZ = thisTrfm3D->trZ;
	newTrfm3D->dX  = thisTrfm3D->dX;
	newTrfm3D->dY  = thisTrfm3D->dY;
	newTrfm3D->dZ  = thisTrfm3D->dZ;
	newTrfm3D->dW  = thisTrfm3D->dW;

	return newTrfm3D;
}

// Aux
void PrintTrfm3D( const trfm3D *thisTrfm3D ) {
	printf( "% 8.4f scale\n", thisTrfm3D->scl );
	printf( "% 8.4f % 8.4f % 8.4f  %8.4f\n",
			thisTrfm3D->r1X, thisTrfm3D->r2X, thisTrfm3D->r3X, thisTrfm3D->trX );
	printf( "% 8.4f % 8.4f % 8.4f  %8.4f\n",
			thisTrfm3D->r1Y, thisTrfm3D->r2Y, thisTrfm3D->r3Y, thisTrfm3D->trY );
	printf( "% 8.4f % 8.4f % 8.4f  %8.4f\n",
			thisTrfm3D->r1Z, thisTrfm3D->r2Z, thisTrfm3D->r3Z, thisTrfm3D->trZ );
	printf( "% 8.1f % 8.1f % 8.1f  %8.4f\n",
			thisTrfm3D->dX, thisTrfm3D->dY, thisTrfm3D->dZ, thisTrfm3D->dW );
	printf("-------- -------- --------  ---\n");
}


////////////////////////////////////////////////////////////////////////////////////
// Transform points, vectors, planes ...

// @@ TODO. Transform a point
//
// input/output in (*xTr, *yTr, *zTr)

void Trfm3DTransformPoint( const trfm3D *thisTrfm3D, float *xTr, float *yTr, float *zTr ) {

	float x = *xTr;
	float y = *yTr;
	float z = *zTr;

	*xTr = thisTrfm3D->scl*(thisTrfm3D->r1X*x + thisTrfm3D->r2X*y + thisTrfm3D->r3X*z) + thisTrfm3D->trX;
	*yTr = thisTrfm3D->scl*(thisTrfm3D->r1Y*x + thisTrfm3D->r2Y*y + thisTrfm3D->r3Y*z) + thisTrfm3D->trY;
	*zTr = thisTrfm3D->scl*(thisTrfm3D->r1Z*x + thisTrfm3D->r2Z*y + thisTrfm3D->r3Z*z) + thisTrfm3D->trZ;


}

// @@ TODO. Transform a vector.
//
// input/output in (*xTr, *yTr, *zTr)
// Hint: Vectors don't translate

void TransformVectorTrfm3D( const trfm3D *thisTrfm3D, float *xTr, float *yTr, float *zTr ) {

	float x = *xTr;
	float y = *yTr;
	float z = *zTr;

	*xTr = thisTrfm3D->scl*(thisTrfm3D->r1X*x + thisTrfm3D->r2X*y + thisTrfm3D->r3X*z);
	*yTr = thisTrfm3D->scl*(thisTrfm3D->r1Y*x + thisTrfm3D->r2Y*y + thisTrfm3D->r3Y*z);
	*zTr = thisTrfm3D->scl*(thisTrfm3D->r1Z*x + thisTrfm3D->r2Z*y + thisTrfm3D->r3Z*z);

}

void TransformNormalTrfm3D(const trfm3D *thisTrfm3D, float *nx, float *ny, float *nz) {

	static trfm3D auxTrfm;

	float xx, xy, xz;
	float factor;

	// thisTrfm3D  == M = S*R*T = S*R (no translations to normals)

	// auxTrfm = (M^{-1})^{T} == S^{-1}*R

	auxTrfm.r1X = thisTrfm3D->r1X;
	auxTrfm.r1Y = thisTrfm3D->r1Y;
	auxTrfm.r1Z = thisTrfm3D->r1Z;
	auxTrfm.r2X = thisTrfm3D->r2X;
	auxTrfm.r2Y = thisTrfm3D->r2Y;
	auxTrfm.r2Z = thisTrfm3D->r2Z;
	auxTrfm.r3X = thisTrfm3D->r3X;
	auxTrfm.r3Y = thisTrfm3D->r3Y;
	auxTrfm.r3Z = thisTrfm3D->r3Z;
	auxTrfm.scl = 1.0f / thisTrfm3D->scl; // S^{-1}
	auxTrfm.trX = 0.0f;
	auxTrfm.trY = 0.0f;
	auxTrfm.trZ = 0.0f;

	// X = N*aurTrfm = N*S^{-1}*R

	xx = *nx;
	xy = *ny;
	xz = *nz;
	TransformVectorTrfm3D(&auxTrfm, &xx, &xy, &xz);

	if ( thisTrfm3D->scl != 1.0) {

		// normalize X
		factor = AGA_MOD_INV(xx, xy, xz);

		xx*= factor;
		xy*= factor;
		xz*= factor;
	}
	*nx = xx;
	*ny = xy;
	*nz = xz;
}

void TransformPlaneTrfm3D(const trfm3D * thisTrfm3D, float *nx, float *ny, float *nz, float *d) {

	static trfm3D auxTrfm;
	float xx, xy, xz;
	float d1;
	float factor;

	xx = *nx;
	xy = *ny;
	xz = *nz;

	// thisTrfm3D  == M = S*R*T = S*R (no translations to normals)

	// auxTrfm = (M^{-1})^{T} == S^{-1}*R

	auxTrfm.r1X = thisTrfm3D->r1X;
	auxTrfm.r1Y = thisTrfm3D->r1Y;
	auxTrfm.r1Z = thisTrfm3D->r1Z;
	auxTrfm.r2X = thisTrfm3D->r2X;
	auxTrfm.r2Y = thisTrfm3D->r2Y;
	auxTrfm.r2Z = thisTrfm3D->r2Z;
	auxTrfm.r3X = thisTrfm3D->r3X;
	auxTrfm.r3Y = thisTrfm3D->r3Y;
	auxTrfm.r3Z = thisTrfm3D->r3Z;
	auxTrfm.scl = 1.0f / thisTrfm3D->scl; // S^{-1}
	auxTrfm.trX = 0.0f;
	auxTrfm.trY = 0.0f;
	auxTrfm.trZ = 0.0f;

	// X = N*aurTrfm = N*S^{-1}*R

	xx = *nx;
	xy = *ny;
	xz = *nz;
	TransformVectorTrfm3D(&auxTrfm, &xx, &xy, &xz);

	// d1 =  d + T*X

	d1 = *d + DOT_PRODUCT(thisTrfm3D->trX, thisTrfm3D->trY, thisTrfm3D->trZ,
						  xx, xy, xz);

	if ( thisTrfm3D->scl != 1.0) {

		// normalize X
		factor = AGA_MOD_INV(xx, xy, xz);

		xx*= factor;
		xy*= factor;
		xz*= factor;
		d1*= factor;
	}
	*nx = xx;
	*ny = xy;
	*nz = xz;
	*d = d1;
}

void ProjectPointTrfm3D( const trfm3D *thisTrfm3D, float *xTr, float *yTr, float *zTr ) {

	float tmpX,   tmpY,   tmpZ, tmpW;
	float scale;

	scale = thisTrfm3D->scl;

	//Px' = Px*r1X + Py*r2X + Pz*r3X + trX
	tmpX = *xTr * thisTrfm3D->r1X * scale + *yTr * thisTrfm3D->r2X * scale + *zTr * thisTrfm3D->r3X * scale + thisTrfm3D->trX;

	//Py' = Px*r1Y*scale + Py*r2Y*scale + Pz*r3Y*scale + trY
	tmpY = *xTr * thisTrfm3D->r1Y * scale + *yTr * thisTrfm3D->r2Y * scale + *zTr * thisTrfm3D->r3Y * scale + thisTrfm3D->trY;

	//Pz' = Px*r1Z*scale + Py*r2Z*scale + Pz*r3Z*scale + trZ
	tmpZ = *xTr * thisTrfm3D->r1Z * scale + *yTr * thisTrfm3D->r2Z * scale + *zTr * thisTrfm3D->r3Z * scale + thisTrfm3D->trZ;

	//Pw' = Px * dx + Py * dy + Pz * dz + dw
	tmpW = *xTr * thisTrfm3D->dX + *yTr * thisTrfm3D->dY + *zTr * thisTrfm3D->dZ + thisTrfm3D->dW;

	if(DIST_IS_ZERO(tmpW)) {
		fprintf(stderr, "[W] ProjectPointTrfm3D: zero W\n");
	} else {
		*xTr = tmpX / tmpW;
		*yTr = tmpY / tmpW;
		*zTr = tmpZ / tmpW;
	}
}

////////////////////////////////////////////////////////////////////////////////////
// Set functions

void SetUnitTrfm3D( trfm3D *thisTrfm3D ) {

	thisTrfm3D->r1X = 1.0f;
	thisTrfm3D->r1Y = 0.0f;
	thisTrfm3D->r1Z = 0.0f;
	thisTrfm3D->r2X = 0.0f;
	thisTrfm3D->r2Y = 1.0f;
	thisTrfm3D->r2Z = 0.0f;
	thisTrfm3D->r3X = 0.0f;
	thisTrfm3D->r3Y = 0.0f;
	thisTrfm3D->r3Z = 1.0f;
	thisTrfm3D->scl = 1.0f;
	thisTrfm3D->trX = 0.0f;
	thisTrfm3D->trY = 0.0f;
	thisTrfm3D->trZ = 0.0f;
	thisTrfm3D->dX  = 0.0f;
	thisTrfm3D->dY  = 0.0f;
	thisTrfm3D->dZ  = 0.0f;
	thisTrfm3D->dW  = 1.0f;
}

void SetCopyTrfm3D( trfm3D *cloned, const trfm3D *source ) {

	cloned->r1X = source->r1X;
	cloned->r1Y = source->r1Y;
	cloned->r1Z = source->r1Z;
	cloned->r2X = source->r2X;
	cloned->r2Y = source->r2Y;
	cloned->r2Z = source->r2Z;
	cloned->r3X = source->r3X;
	cloned->r3Y = source->r3Y;
	cloned->r3Z = source->r3Z;
	cloned->scl = source->scl;
	cloned->trX = source->trX;
	cloned->trY = source->trY;
	cloned->trZ = source->trZ;
	cloned->dX  = source->dX;
	cloned->dY  = source->dY;
	cloned->dZ  = source->dZ;
	cloned->dW  = source->dW;
}

void SetRot_X_Trfm3D( trfm3D *thisTrfm3D, float angle ) {

	thisTrfm3D->r1X = 1.0f;
	thisTrfm3D->r1Y = 0.0f;
	thisTrfm3D->r1Z = 0.0f;
	thisTrfm3D->r2X = 0.0f;
	thisTrfm3D->r2Y =   (float) cos( (double) angle );
	thisTrfm3D->r2Z =   (float) sin( (double) angle );
	thisTrfm3D->r3X = 0.0f;
	thisTrfm3D->r3Y = - (float) sin( (double) angle );
	thisTrfm3D->r3Z =   (float) cos( (double) angle );
	thisTrfm3D->scl = 1.0f;
	thisTrfm3D->trX = 0.0f;
	thisTrfm3D->trY = 0.0f;
	thisTrfm3D->trZ = 0.0f;
	thisTrfm3D->dX  = 0.0f;
	thisTrfm3D->dY  = 0.0f;
	thisTrfm3D->dZ  = 0.0f;
	thisTrfm3D->dW  = 1.0f;
}

void SetRot_Y_Trfm3D( trfm3D *thisTrfm3D, float angle ) {

	thisTrfm3D->r1X =   (float) cos( (double) angle );
	thisTrfm3D->r1Y = 0.0f;
	thisTrfm3D->r1Z = - (float) sin( (double) angle );
	thisTrfm3D->r2X = 0.0f;
	thisTrfm3D->r2Y = 1.0f;
	thisTrfm3D->r2Z = 0.0f;
	thisTrfm3D->r3X =   (float) sin( (double) angle );
	thisTrfm3D->r3Y = 0.0f;
	thisTrfm3D->r3Z =   (float) cos( (double) angle );
	thisTrfm3D->scl = 1.0f;
	thisTrfm3D->trX = 0.0f;
	thisTrfm3D->trY = 0.0f;
	thisTrfm3D->trZ = 0.0f;
	thisTrfm3D->dX  = 0.0f;
	thisTrfm3D->dY  = 0.0f;
	thisTrfm3D->dZ  = 0.0f;
	thisTrfm3D->dW  = 1.0f;
}

void SetRot_Z_Trfm3D( trfm3D *thisTrfm3D, float angle ) {

	thisTrfm3D->r1X =   (float) cos( (double) angle );
	thisTrfm3D->r1Y =   (float) sin( (double) angle );
	thisTrfm3D->r1Z = 0.0f;
	thisTrfm3D->r2X = - (float) sin( (double) angle );
	thisTrfm3D->r2Y =   (float) cos( (double) angle );
	thisTrfm3D->r2Z = 0.0f;
	thisTrfm3D->r3X = 0.0f;
	thisTrfm3D->r3Y = 0.0f;
	thisTrfm3D->r3Z = 1.0f;
	thisTrfm3D->scl = 1.0f;
	thisTrfm3D->trX = 0.0f;
	thisTrfm3D->trY = 0.0f;
	thisTrfm3D->trZ = 0.0f;
	thisTrfm3D->dX  = 0.0f;
	thisTrfm3D->dY  = 0.0f;
	thisTrfm3D->dZ  = 0.0f;
	thisTrfm3D->dW  = 1.0f;
}

/*
  @@ TODO: Rotate by angle theta around an arbitrary axis r

  Positive angles are anticlockwise looking down the axis
  towards the origin.
  Assume right hand coordinate system.
*/


void SetRotVecTrfm3D(trfm3D *thisTrfm, float rx, float ry, float rz,
					 float theta ) {

	thisTrfm->r1X = (float) cos((double) theta) + rx*rx*(1.0f - (float) cos((double) theta));
	thisTrfm->r1Y = rx*ry*(1.0f - (float) cos((double) theta)) - rz*(float) sin((double) theta);
	thisTrfm->r1Z = rx*rz*(1.0f - (float) cos((double) theta)) + ry*(float) sin((double) theta);
	thisTrfm->r2X = ry*rx*(1.0f - (float) cos((double) theta)) + rz*(float) sin((double) theta);
	thisTrfm->r2Y = (float) cos((double) theta) +ry*ry*(1.0f - (float) cos((double) theta));
	thisTrfm->r2Z = ry*rz*(1.0f - (float) cos((double) theta)) - rx*(float) sin((double) theta);
	thisTrfm->r3X = rz*rx*(1.0f - (float) cos((double) theta)) - ry*(float) sin((double) theta);
	thisTrfm->r3Y = rz*ry*(1.0f - (float) cos((double) theta)) + rx*(float) sin((double) theta);
	thisTrfm->r3Z = (float) cos((double) theta) + rz*rz*(1.0f - (float) cos((double) theta));
	thisTrfm->scl = 1.0f;
	thisTrfm->trX = 0.0f;
	thisTrfm->trY = 0.0f;
	thisTrfm->trZ = 0.0f;
	thisTrfm->dX  = 0.0f;
	thisTrfm->dY  = 0.0f;
	thisTrfm->dZ  = 0.0f;
	thisTrfm->dW  = 1.0f;

}


void SetTransTrfm3D( trfm3D *thisTrfm3D, float tx, float ty, float tz ) {

	thisTrfm3D->r1X = 1.0f;
	thisTrfm3D->r1Y = 0.0f;
	thisTrfm3D->r1Z = 0.0f;
	thisTrfm3D->r2X = 0.0f;
	thisTrfm3D->r2Y = 1.0f;
	thisTrfm3D->r2Z = 0.0f;
	thisTrfm3D->r3X = 0.0f;
	thisTrfm3D->r3Y = 0.0f;
	thisTrfm3D->r3Z = 1.0f;
	thisTrfm3D->scl = 1.0f;
	thisTrfm3D->trX = tx;
	thisTrfm3D->trY = ty;
	thisTrfm3D->trZ = tz;
	thisTrfm3D->dX  = 0.0f;
	thisTrfm3D->dY  = 0.0f;
	thisTrfm3D->dZ  = 0.0f;
	thisTrfm3D->dW  = 1.0f;
}

// @@ TODO: Rotate angle radians about an axis defined by vector and located at point
//
// Note: vector must be unit vector

void SetRotAxisTrfm3D(trfm3D *thisTrfm, float vectorX, float vectorY, float vectorZ,
					  float pointX,  float pointY,  float pointZ,
					  float angle ) {

	float rx, ry, rz, mod2;

	rx = vectorX - pointX;
	ry = vectorY - pointY;
	rz = vectorZ - pointZ;

	mod2 = rx*rx + ry*ry + rz*rz;

	if (mod2 > VECTOR_EPSILON) {
		rx = rx / (sqrt(mod2));
		ry = ry / (sqrt(mod2));
		rz = ry / (sqrt(mod2));
	} else {
		rx = 0.0f;
		ry = 0.0f;
		rz = 0.0f;
	}

	thisTrfm->r1X = (float) cos((double) angle) + rx*rx*(1.0f - (float) cos((double) angle));
	thisTrfm->r1Y = rx*ry*(1.0f - (float) cos((double) angle)) - rz*(float) sin((double) angle);
	thisTrfm->r1Z = rx*rz*(1.0f - (float) cos((double) angle)) + ry*(float) sin((double) angle);
	thisTrfm->r2X = ry*rx*(1.0f - (float) cos((double) angle)) + rz*(float) sin((double) angle);
	thisTrfm->r2Y = (float) cos((double) angle) +ry*ry*(1.0f - (float) cos((double) angle));
	thisTrfm->r2Z = ry*rz*(1.0f - (float) cos((double) angle)) - rx*(float) sin((double) angle);
	thisTrfm->r3X = rz*rx*(1.0f - (float) cos((double) angle)) - ry*(float) sin((double) angle);
	thisTrfm->r3Y = rz*ry*(1.0f - (float) cos((double) angle)) + rx*(float) sin((double) angle);
	thisTrfm->r3Z = (float) cos((double) angle) + rz*rz*(1.0f - (float) cos((double) angle));
	thisTrfm->scl = 1.0f;
	thisTrfm->trX = 0.0f;
	thisTrfm->trY = 0.0f;
	thisTrfm->trZ = 0.0f;
	thisTrfm->dX  = 0.0f;
	thisTrfm->dY  = 0.0f;
	thisTrfm->dZ  = 0.0f;
	thisTrfm->dW  = 1.0f;

}

void SetScaleTrfm3D( trfm3D *thisTrfm3D, float scale ) {
	thisTrfm3D->r1X = 1.0f;
	thisTrfm3D->r1Y = 0.0f;
	thisTrfm3D->r1Z = 0.0f;
	thisTrfm3D->r2X = 0.0f;
	thisTrfm3D->r2Y = 1.0f;
	thisTrfm3D->r2Z = 0.0f;
	thisTrfm3D->r3X = 0.0f;
	thisTrfm3D->r3Y = 0.0f;
	thisTrfm3D->r3Z = 1.0f;
	thisTrfm3D->scl = scale;
	thisTrfm3D->trX = 0.0f;
	thisTrfm3D->trY = 0.0f;
	thisTrfm3D->trZ = 0.0f;
	thisTrfm3D->dX  = 0.0f;
	thisTrfm3D->dY  = 0.0f;
	thisTrfm3D->dZ  = 0.0f;
	thisTrfm3D->dW  = 1.0f;
}

void SetFrustumTrfm3D( trfm3D *thisTrfm,
					   float left, float right,
					   float top, float bottom,
					   float near, float far) {
	float x, y, a, b, c, d;

	x = (2.0f * near) / (right - left);
	y = (2.0f * near) / (top - bottom);

	a = (right + left) / (right - left);
	b = (top + bottom) / (top - bottom);
	c = -(far + near) / (far - near);
	d = -(2.0f * far * near) / (far - near);

	thisTrfm->scl = 1.0f;
	thisTrfm->r1X = x;    thisTrfm->r1Y = 0.0f;  thisTrfm->r1Z = a;     thisTrfm->trX = 0.0f;
	thisTrfm->r2X = 0.0f; thisTrfm->r2Y = y;     thisTrfm->r2Z = b;     thisTrfm->trY = 0.0f;
	thisTrfm->r3X = 0.0f; thisTrfm->r3Y = 0.0f;  thisTrfm->r3Z = c;     thisTrfm->trZ = d;
	thisTrfm->dX  = 0.0f; thisTrfm->dY  = 0.0f;  thisTrfm->dZ  = -1.0f; thisTrfm->dW  = 0.0f;
}


void NormalizeTrfm3D( trfm3D *thisTrfm) {

	/*   Column1=Normalized(CrossProduct(Column2,Column3)); */
	/*   Column2=Normalized(CrossProduct(Column3,Column1)); */
	/*   Normalize(Column3); */

	crossVV(&thisTrfm->r1X, &thisTrfm->r2X, &thisTrfm->r3X,
			thisTrfm->r1Y, thisTrfm->r2Y, thisTrfm->r3Y,
			thisTrfm->r1Z, thisTrfm->r2Z, thisTrfm->r3Z);
	VectorNormalize(&thisTrfm->r1X, &thisTrfm->r2X, &thisTrfm->r3X);

	crossVV(&thisTrfm->r1Y, &thisTrfm->r2Y, &thisTrfm->r3Y,
			thisTrfm->r1Z, thisTrfm->r2Z, thisTrfm->r3Z,
			thisTrfm->r1X, thisTrfm->r2X, thisTrfm->r3X);
	VectorNormalize(&thisTrfm->r1Y, &thisTrfm->r2Y, &thisTrfm->r3Y);

	VectorNormalize(&thisTrfm->r1Z, &thisTrfm->r2Z, &thisTrfm->r3Z);
}


////////////////////////////////////////////////////////////////////////////////////
// Add functions

void AddRot_X_Trfm3D( trfm3D *thisTrfm3D, float angle ) {

	static trfm3D localT;
	SetRot_X_Trfm3D( &localT, angle );
	CompositionTrfm3D( thisTrfm3D, &localT );
}

void AddRot_Y_Trfm3D( trfm3D *thisTrfm3D, float angle ) {

	static trfm3D localT;
	SetRot_Y_Trfm3D( &localT, angle );
	CompositionTrfm3D( thisTrfm3D, &localT );
}

void AddRot_Z_Trfm3D( trfm3D *thisTrfm3D, float angle ) {

	static trfm3D localT;
	SetRot_Z_Trfm3D( &localT, angle );
	CompositionTrfm3D( thisTrfm3D, &localT );
}

void AddRotVecTrfm3D( trfm3D *thisTrfm3D, float rx, float ry, float rz,
					  float theta ) {

	static trfm3D localT;
	SetRotVecTrfm3D(&localT, rx, ry, rz, theta );
	CompositionTrfm3D( thisTrfm3D, &localT );
}

// @@TODO: Given a trfm T, create another trfm T2 which performs "RotAxis" and
//         compose in T. After this call:
//
//         T = T * T2

void AddRotAxisTrfm3D( trfm3D *thisTrfm3D,
					   float vectorX, float vectorY, float vectorZ,
					   float pointX,  float pointY,  float pointZ,
					   float angle) {
	static trfm3D localT;
	SetRotAxisTrfm3D(&localT, vectorX, vectorY, vectorZ, pointX, pointY, pointZ, angle);
	CompositionTrfm3D(thisTrfm3D, &localT);
}

void AddTransTrfm3D( trfm3D *thisTrfm3D, float tx, float ty, float tz ) {

	thisTrfm3D->trX += tx * thisTrfm3D->scl * thisTrfm3D->r1X;
	thisTrfm3D->trX += ty * thisTrfm3D->scl * thisTrfm3D->r2X;
	thisTrfm3D->trX += tz * thisTrfm3D->scl * thisTrfm3D->r3X;

	thisTrfm3D->trY += tx * thisTrfm3D->scl * thisTrfm3D->r1Y;
	thisTrfm3D->trY += ty * thisTrfm3D->scl * thisTrfm3D->r2Y;
	thisTrfm3D->trY += tz * thisTrfm3D->scl * thisTrfm3D->r3Y;

	thisTrfm3D->trZ += tx * thisTrfm3D->scl * thisTrfm3D->r1Z;
	thisTrfm3D->trZ += ty * thisTrfm3D->scl * thisTrfm3D->r2Z;
	thisTrfm3D->trZ += tz * thisTrfm3D->scl * thisTrfm3D->r3Z;
}

void AddScaleTrfm3D( trfm3D *thisTrfm3D, float scale ) {
	thisTrfm3D->scl *= scale;
}

////////////////////////////////////////////////////////////////////////////////////
// Compositions

// Compute target*add and leave the result in target

void CompositionTrfm3D(trfm3D *target, const trfm3D *add ) {

	double    newR1X, newR1Y, newR1Z;
	double    newR2X, newR2Y, newR2Z;
	double    newR3X, newR3Y, newR3Z;

	double    newDX, newDY, newDZ, newDW;

	double    newTrX, newTrY, newTrZ;

	// New rotation Matrix

	newR1X = newR1Y = newR1Z = 0.0;

	newR1X += (double) target->r1X * (double) add->r1X;
	newR1X += (double) target->r2X * (double) add->r1Y;
	newR1X += (double) target->r3X * (double) add->r1Z;
	newR1X += (double) target->trX * (double) add->dX ;

	newR1Y += (double) target->r1Y * (double) add->r1X;
	newR1Y += (double) target->r2Y * (double) add->r1Y;
	newR1Y += (double) target->r3Y * (double) add->r1Z;
	newR1Y += (double) target->trY * (double) add->dX ;

	newR1Z += (double) target->r1Z * (double) add->r1X;
	newR1Z += (double) target->r2Z * (double) add->r1Y;
	newR1Z += (double) target->r3Z * (double) add->r1Z;
	newR1Z += (double) target->trZ * (double) add->dX ;

	newR2X = newR2Y = newR2Z = 0.0;

	newR2X += (double) target->r1X * (double) add->r2X;
	newR2X += (double) target->r2X * (double) add->r2Y;
	newR2X += (double) target->r3X * (double) add->r2Z;
	newR2X += (double) target->trX * (double) add->dY ;

	newR2Y += (double) target->r1Y * (double) add->r2X;
	newR2Y += (double) target->r2Y * (double) add->r2Y;
	newR2Y += (double) target->r3Y * (double) add->r2Z;
	newR2Y += (double) target->trY * (double) add->dY ;

	newR2Z += (double) target->r1Z * (double) add->r2X;
	newR2Z += (double) target->r2Z * (double) add->r2Y;
	newR2Z += (double) target->r3Z * (double) add->r2Z;
	newR2Z += (double) target->trZ * (double) add->dY ;

	newR3X = newR3Y = newR3Z = 0.0;

	newR3X += (double) target->r1X * (double) add->r3X;
	newR3X += (double) target->r2X * (double) add->r3Y;
	newR3X += (double) target->r3X * (double) add->r3Z;
	newR3X += (double) target->trX * (double) add->dZ ;

	newR3Y += (double) target->r1Y * (double) add->r3X;
	newR3Y += (double) target->r2Y * (double) add->r3Y;
	newR3Y += (double) target->r3Y * (double) add->r3Z;
	newR3Y += (double) target->trY * (double) add->dZ ;

	newR3Z += (double) target->r1Z * (double) add->r3X;
	newR3Z += (double) target->r2Z * (double) add->r3Y;
	newR3Z += (double) target->r3Z * (double) add->r3Z;
	newR3Z += (double) target->trZ * (double) add->dZ ;

	newDX = newDY = newDZ = newDW = 0.0;

	newDX += (double) target->dX * (double) add->r1X;
	newDX += (double) target->dY * (double) add->r1Y;
	newDX += (double) target->dZ * (double) add->r1Z;
	newDX += (double) target->dW * (double) add->dX;

	newDY += (double) target->dX * (double) add->r2X;
	newDY += (double) target->dY * (double) add->r2Y;
	newDY += (double) target->dZ * (double) add->r2Z;
	newDY += (double) target->dW * (double) add->dY;

	newDZ += (double) target->dX * (double) add->r3X;
	newDZ += (double) target->dY * (double) add->r3Y;
	newDZ += (double) target->dZ * (double) add->r3Z;
	newDZ += (double) target->dW * (double) add->dZ;

	newDW += (double) target->dX * (double) add->trX;
	newDW += (double) target->dY * (double) add->trY;
	newDW += (double) target->dZ * (double) add->trZ;
	newDW += (double) target->dW * (double) add->dW;

	// New Translation

	newTrX = 0;
	newTrX += (double) target->scl * (double) target->r1X * (double) add->trX;
	newTrX += (double) target->scl * (double) target->r2X * (double) add->trY;
	newTrX += (double) target->scl * (double) target->r3X * (double) add->trZ;
	newTrX += (double) target->trX * (double) add->dW;

	newTrY = 0;
	newTrY += (double) target->scl * (double) target->r1Y * (double) add->trX;
	newTrY += (double) target->scl * (double) target->r2Y * (double) add->trY;
	newTrY += (double) target->scl * (double) target->r3Y * (double) add->trZ;
	newTrY += (double) target->trY * (double) add->dW;

	newTrZ = 0;
	newTrZ += (double) target->scl * (double) target->r1Z * (double) add->trX;
	newTrZ += (double) target->scl * (double) target->r2Z * (double) add->trY;
	newTrZ += (double) target->scl * (double) target->r3Z * (double) add->trZ;
	newTrZ += (double) target->trZ * (double) add->dW;

	// New scale

	target->scl = add->scl * target->scl;

	// Copy values

	target->r1X = newR1X;
	target->r1Y = newR1Y;
	target->r1Z = newR1Z;

	target->r2X = newR2X;
	target->r2Y = newR2Y;
	target->r2Z = newR2Z;

	target->r3X = newR3X;
	target->r3Y = newR3Y;
	target->r3Z = newR3Z;

	target->trX = newTrX;
	target->trY = newTrY;
	target->trZ = newTrZ;

	target->dX = newDX;
	target->dY = newDY;
	target->dZ = newDZ;
	target->dW = newDW;

}

////////////////////////////////////////////////////////////////////////////////////
// OpenGL related

void GetGLMatrixTrfm3D( const trfm3D *thisTrfm3D, float *glmatrix ) {

	glmatrix[0]  = thisTrfm3D->r1X * thisTrfm3D->scl;
	glmatrix[1]  = thisTrfm3D->r1Y * thisTrfm3D->scl;
	glmatrix[2]  = thisTrfm3D->r1Z * thisTrfm3D->scl;
	glmatrix[3]  = thisTrfm3D->dX;
	glmatrix[4]  = thisTrfm3D->r2X * thisTrfm3D->scl;
	glmatrix[5]  = thisTrfm3D->r2Y * thisTrfm3D->scl;
	glmatrix[6]  = thisTrfm3D->r2Z * thisTrfm3D->scl;
	glmatrix[7]  = thisTrfm3D->dY;
	glmatrix[8]  = thisTrfm3D->r3X * thisTrfm3D->scl;
	glmatrix[9]  = thisTrfm3D->r3Y * thisTrfm3D->scl;
	glmatrix[10] = thisTrfm3D->r3Z * thisTrfm3D->scl;
	glmatrix[11] = thisTrfm3D->dZ;
	glmatrix[12] = thisTrfm3D->trX;
	glmatrix[13] = thisTrfm3D->trY;
	glmatrix[14] = thisTrfm3D->trZ;
	glmatrix[15] = thisTrfm3D->dW;
}

/* void SetGLMatrixTrfm3D(const trfm3D *thisTrfm3D) { */

/*   GetGLMatrixTrfm3D(thisTrfm3D, &LOCAL_M[0]); */
/*   glLoadMatrixf(&LOCAL_M[0]); */
/* } */

/* void MultGLMatrixTrfm3D(const trfm3D *thisTrfm3D) { */

/*   GetGLMatrixTrfm3D(thisTrfm3D, &LOCAL_M[0]); */
/*   glMultMatrixf(&LOCAL_M[0]); */
/* } */

////////////////////////////////////////////////////////////////////////////////////
// Misc

int cmpTrfm3D(trfm3D *A, trfm3D *B) {

	if (A == NULL && B == NULL) return 0;
	if (A == NULL) return -1;
	if (B == NULL) return -1;

	if (!DIST_IS_ZERO(A->r1X - B->r1X)) return -2;
	if (!DIST_IS_ZERO(A->r1Y - B->r1Y)) return -2;
	if (!DIST_IS_ZERO(A->r1Z - B->r1Z)) return -2;

	if (!DIST_IS_ZERO(A->r2X - B->r2X)) return -2;
	if (!DIST_IS_ZERO(A->r2Y - B->r2Y)) return -2;
	if (!DIST_IS_ZERO(A->r2Z - B->r2Z)) return -2;

	if (!DIST_IS_ZERO(A->r3X - B->r3X)) return -2;
	if (!DIST_IS_ZERO(A->r3Y - B->r3Y)) return -2;
	if (!DIST_IS_ZERO(A->r3Z - B->r3Z)) return -2;

	if (!DIST_IS_ZERO(A->trX - B->trX)) return -3;
	if (!DIST_IS_ZERO(A->trY - B->trY)) return -3;
	if (!DIST_IS_ZERO(A->trZ - B->trZ)) return -3;

	if (!DIST_IS_ZERO(A->scl - B->scl)) return -4;

	return 0;
}


void SetLocal2WorldTrfm3D(trfm3D *thisTrfm3D,
						  float Px, float Py, float Pz,
						  float Rx, float Ry, float Rz,
						  float Ux, float Uy, float Uz,
						  float Dx, float Dy, float Dz) {
	thisTrfm3D->r1X = Rx;
	thisTrfm3D->r1Y = Ry;
	thisTrfm3D->r1Z = Rz;
	thisTrfm3D->r2X = Ux;
	thisTrfm3D->r2Y = Uy;
	thisTrfm3D->r2Z = Uz;
	thisTrfm3D->r3X = Dx;
	thisTrfm3D->r3Y = Dy;
	thisTrfm3D->r3Z = Dz;
	thisTrfm3D->scl = 1.0;
	thisTrfm3D->trX = Px;
	thisTrfm3D->trY = Py;
	thisTrfm3D->trZ = Pz;

	thisTrfm3D->dX = 0.0f;
	thisTrfm3D->dY = 0.0f;
	thisTrfm3D->dZ = 0.0f;
	thisTrfm3D->dW = 1.0f;
}

void SetWorld2LocalTrfm3D(trfm3D *thisTrfm3D,
						  float Px, float Py, float Pz,
						  float Rx, float Ry, float Rz,
						  float Ux, float Uy, float Uz,
						  float Dx, float Dy, float Dz) {

	thisTrfm3D->r1X = Rx;
	thisTrfm3D->r1Y = Ux;
	thisTrfm3D->r1Z = Dx;

	thisTrfm3D->r2X = Ry;
	thisTrfm3D->r2Y = Uy;
	thisTrfm3D->r2Z = Dy;

	thisTrfm3D->r3X = Rz;
	thisTrfm3D->r3Y = Uz;
	thisTrfm3D->r3Z = Dz;

	thisTrfm3D->scl = 1.0;

	thisTrfm3D->trX = -1*DOT_PRODUCT(Rx, Ry, Rz, Px, Py, Pz);
	thisTrfm3D->trY = -1*DOT_PRODUCT(Ux, Uy, Uz, Px, Py, Pz);
	thisTrfm3D->trZ = -1*DOT_PRODUCT(Dx, Dy, Dz, Px, Py, Pz);

	thisTrfm3D->dX = 0.0f;
	thisTrfm3D->dY = 0.0f;
	thisTrfm3D->dZ = 0.0f;
	thisTrfm3D->dW = 1.0f;

}

void SetWorld2LocalFrameTrfm3D(trfm3D *thisTrfm3D, const trfm3D *frameTrfm) {

	thisTrfm3D->r1X = frameTrfm->r1X;
	thisTrfm3D->r1Y = frameTrfm->r2X;
	thisTrfm3D->r1Z = frameTrfm->r3X;

	thisTrfm3D->r2X = frameTrfm->r1Y;
	thisTrfm3D->r2Y = frameTrfm->r2Y;
	thisTrfm3D->r2Z = frameTrfm->r3Y;

	thisTrfm3D->r3X = frameTrfm->r1Z;
	thisTrfm3D->r3Y = frameTrfm->r2Z;
	thisTrfm3D->r3Z = frameTrfm->r3Z;

	thisTrfm3D->scl = 1.0;

	thisTrfm3D->trX = 0.0;
	thisTrfm3D->trX -= frameTrfm->r1X*frameTrfm->trX;
	thisTrfm3D->trX -= frameTrfm->r1Y*frameTrfm->trY;
	thisTrfm3D->trX -= frameTrfm->r1Z*frameTrfm->trZ;

	thisTrfm3D->trY = 0.0;
	thisTrfm3D->trY -= frameTrfm->r2X*frameTrfm->trY;
	thisTrfm3D->trY -= frameTrfm->r2Y*frameTrfm->trY;
	thisTrfm3D->trY -= frameTrfm->r2Z*frameTrfm->trZ;

	thisTrfm3D->trZ = 0.0;
	thisTrfm3D->trZ -= frameTrfm->r3X*frameTrfm->trX;
	thisTrfm3D->trZ -= frameTrfm->r3Y*frameTrfm->trY;
	thisTrfm3D->trZ -= frameTrfm->r3Z*frameTrfm->trZ;

	thisTrfm3D->dX = 0.0f;
	thisTrfm3D->dY = 0.0f;
	thisTrfm3D->dZ = 0.0f;
	thisTrfm3D->dW = 1.0f;

}


void Set3PointsTrfm3D( trfm3D *thisTrfm3D,
					   float p1X, float p1Y, float p1Z,
					   float p2X, float p2Y, float p2Z,
					   float p3X, float p3Y, float p3Z,
					   float q1X, float q1Y, float q1Z,
					   float q2X, float q2Y, float q2Z,
					   float q3X, float q3Y, float q3Z ) {

	trfm3D *tmp;

	tmp = CreateTrfm3D();
	Set3PntStdTrfm3D( thisTrfm3D,
					  p1X, p1Y, p1Z,
					  p2X, p2Y, p2Z,
					  p3X, p3Y, p3Z );
	Set3PntStdTrfm3D( tmp,
					  q1X, q1Y, q1Z,
					  q2X, q2Y, q2Z,
					  q3X, q3Y, q3Z );
	SetInverseTrfm3D( tmp );
	CompositionTrfm3D( thisTrfm3D, tmp );
	DestroyTrfm3D( &tmp );
}

void Set3PntStdTrfm3D( trfm3D *thisTrfm3D,
					   float px, float py, float pz,
					   float qx, float qy, float qz,
					   float rx, float ry, float rz ) {
	// it is assumed P != Q != R != P
	float    alpha;
	float    beta;
	float    gamma;
	double   hypotenuse;
	float    q1x,  q1y,  q1z;
	float    q2x,  q2y,  q2z;
	float    r3x,  r3y,  r3z;

	// *** 1 *** move P to the origin
	//           P, Q, R move to P1, Q1, R1
	// P1 = O  Q1 = Q - P
	SetTransTrfm3D( thisTrfm3D, -px, -py, -pz );
	// compute coordinates of Q1
	q1x = qx;       q1y = qy;       q1z = qz;
	Trfm3DTransformPoint( thisTrfm3D, &q1x, &q1y, &q1z );


	// *** 2 *** rotate -alpha around Z
	//           P1, Q1, R1 rotate to P2, Q2, R2
	//           note that : P=P1=P2  &&  Q2 will be on plane XZ
	// compute alpha :
	//   project vector O-Q1  onto plane xy : (q1x, q1y, 0)
	//   X axis
	hypotenuse = sqrt( (double) ( q1x*q1x + q1y*q1y ) );
	if( hypotenuse > DISTANCE_EPSILON ) {
		alpha      = (float) acos( q1x / hypotenuse );
		if( q1y < 0.0 ) alpha = -alpha;
		AddRot_Z_Trfm3D( thisTrfm3D, -alpha );
	}
	// compute coordinates of Q2
	q2x = qx;       q2y = qy;       q2z = qz;
	Trfm3DTransformPoint( thisTrfm3D, &q2x, &q2y, &q2z );


	// *** 3 *** rotate beta around Y
	//           P2, Q2, R2 rotate to P3, Q3, R3
	//           note that : P=P1=P2=P3  &&  Q3 will be on axis X
	// compute beta
	//   vector O-Q2 ( q2x, 0, q2z )   note that q2y is 0.0 (Q2 on XZ)
	//   x positive axis
	hypotenuse = sqrt( (double) ( q2x*q2x + q2z*q2z ) );
	beta       = (float) acos( q2x / hypotenuse );
	if( q2z < 0.0 ) beta = -beta;
	AddRot_Y_Trfm3D( thisTrfm3D, beta );
	// compute coordinates of R3
	r3x = rx;       r3y = ry;       r3z = rz;
	Trfm3DTransformPoint( thisTrfm3D, &r3x, &r3y, &r3z );


	// *** 4 *** rotate -gamma around X
	//           P3, Q3, R3 rotate to P4, Q4, R4
	//           note that : P=P1=P2=P3=P4  &&
	//                       Q3=Q4 will be on axis X
	//                       R4 will be on plane XZ
	// compute gamma
	//   projection of vector O-R3 onto plane YZ
	//   y positive axis
	hypotenuse = sqrt( (double) ( r3y*r3y + r3z*r3z ) );
	gamma      = (float) acos( r3y / hypotenuse );
	if( r3z < 0.0 ) gamma = -gamma;
	AddRot_X_Trfm3D( thisTrfm3D, -gamma );
}

void SetInverseTrfm3D( trfm3D *thisTrfm3D ) {

	static trfm3D auxTrfm;


	//            | sR   T |
	//      M   = |  0   1 |
	//
	//                 |(1/s)R^{T}  -(1/s)R^{T}*T |
	//      inv(M)   = |     0              1     |
	//

	// Compute inv(R) = R^{T}

	if (DIST_IS_ZERO(thisTrfm3D->dX) || DIST_IS_ZERO(thisTrfm3D->dY) ||
		DIST_IS_ZERO(thisTrfm3D->dZ) || DIST_IS_ZERO(1.0f - thisTrfm3D->dW)) {
		fprintf(stderr, "[E] Can't compute inverse on a projective trfm3D!\n");
		exit(1);
	}

	if (DIST_IS_ZERO(thisTrfm3D->scl)) {
		fprintf(stderr, "[E] Can't compute inverse: source trfm3D has zero scale!\n");
		exit(1);
	}

	auxTrfm.r1X = thisTrfm3D->r1X;
	auxTrfm.r1Y = thisTrfm3D->r2X;
	auxTrfm.r1Z = thisTrfm3D->r3X;

	auxTrfm.r2X = thisTrfm3D->r1Y;
	auxTrfm.r2Y = thisTrfm3D->r2Y;
	auxTrfm.r2Z = thisTrfm3D->r3Y;

	auxTrfm.r3X = thisTrfm3D->r1Z;
	auxTrfm.r3Y = thisTrfm3D->r2Z;
	auxTrfm.r3Z = thisTrfm3D->r3Z;

	// Compute inv(S) = 1/s

	auxTrfm.scl = 1.0f / thisTrfm3D->scl;

	// Compute inv(T) = -(1/s)R^{T}*T

	auxTrfm.trX = thisTrfm3D->trX*auxTrfm.r1X + thisTrfm3D->trY*auxTrfm.r2X + thisTrfm3D->trZ*auxTrfm.r3X;
	auxTrfm.trX*= -auxTrfm.scl;
	auxTrfm.trY = thisTrfm3D->trX*auxTrfm.r1Y + thisTrfm3D->trY*auxTrfm.r2Y + thisTrfm3D->trZ*auxTrfm.r3Y;
	auxTrfm.trY*= -auxTrfm.scl;
	auxTrfm.trZ = thisTrfm3D->trX*auxTrfm.r1Z + thisTrfm3D->trY*auxTrfm.r2Z + thisTrfm3D->trZ*auxTrfm.r3Z;
	auxTrfm.trZ*= -auxTrfm.scl;

	// Store result

	auxTrfm.dX = 0.0f;
	auxTrfm.dY = 0.0f;
	auxTrfm.dZ = 0.0f;
	auxTrfm.dW = 1.0f;

	SetCopyTrfm3D( thisTrfm3D, &auxTrfm );
}

void AbsTrfm3D (trfm3D *thisTrfm3D ) {

	thisTrfm3D->r1X = fabs(thisTrfm3D->r1X);
	thisTrfm3D->r1Y = fabs(thisTrfm3D->r1Y);
	thisTrfm3D->r1Z = fabs(thisTrfm3D->r1Z);
	thisTrfm3D->r2X = fabs(thisTrfm3D->r2X);
	thisTrfm3D->r2Y = fabs(thisTrfm3D->r2Y);
	thisTrfm3D->r2Z = fabs(thisTrfm3D->r2Z);
	thisTrfm3D->r3X = fabs(thisTrfm3D->r3X);
	thisTrfm3D->r3Y = fabs(thisTrfm3D->r3Y);
	thisTrfm3D->r3Z = fabs(thisTrfm3D->r3Z);
	thisTrfm3D->scl = fabs(thisTrfm3D->scl);
	thisTrfm3D->trX = fabs(thisTrfm3D->trX);
	thisTrfm3D->trY = fabs(thisTrfm3D->trY);
	thisTrfm3D->trZ = fabs(thisTrfm3D->trZ);
}
