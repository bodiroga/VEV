#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "tools.h"
#include "bbox.h"

void     DestroyBBox( BBox **theBBox ) {
	BBox    *thisBBox = *theBBox;
	if( ! thisBBox ) return;
	// reclaim openGL buffers
	if (thisBBox->vbo_id)
		glDeleteBuffers(1, &thisBBox->vbo_id);
	if (thisBBox->idxvbo_id)
		glDeleteBuffers(1, &thisBBox->idxvbo_id);
	free( thisBBox );
	*theBBox = NULL;
}

BBox    *CreateVoidBBox( ) {
	BBox    *newBBox;
	newBBox = malloc( sizeof(*newBBox) * 1 );
	SetVoidBBox(newBBox);
	newBBox->vbo_id = 0;
	newBBox->idxvbo_id = 0;
	return newBBox;
}

BBox    *CreateBBox( float xmin, float ymin, float zmin, float xmax, float ymax, float zmax ) {
	BBox    *newBBox;
	newBBox = malloc( sizeof(*newBBox) * 1 );
	newBBox->xMin = xmin;
	newBBox->xMax = xmax;
	newBBox->yMin = ymin;
	newBBox->yMax = ymax;
	newBBox->zMin = zmin;
	newBBox->zMax = zmax;
	newBBox->vbo_id = 0;
	newBBox->idxvbo_id = 0;
	newBBox->vbo_uptodate = 0;
	return newBBox;
}

void CopyBBox(BBox * target, const BBox * source) {
	target->xMin = source->xMin;
	target->xMax = source->xMax;
	target->yMin = source->yMin;
	target->yMax = source->yMax;
	target->zMin = source->zMin;
	target->zMax = source->zMax;
	target->vbo_uptodate = 0;
}

void SetVoidBBox(BBox *theBBox) {
	theBBox->xMax = -FLT_MAX;
	theBBox->xMin =  FLT_MAX;
	theBBox->yMax = -FLT_MAX;
	theBBox->yMin =  FLT_MAX;
	theBBox->zMax = -FLT_MAX;
	theBBox->zMin =  FLT_MAX;
	theBBox->vbo_uptodate = 0;
}

/* Given a trfm3D transformation, calculate the new axis aligned BBox

   Algorithm based on:
   Transforming Axis-Aligned Bounding Boxes
   by Jim Arvo
   from "Graphics Gems", Academic Press, 1990
*/

void TransformBBox ( BBox * newBBox,
					 const BBox * oldBBox,
					 const trfm3D * T) {

	float  a, b;
	float  Amin[3], Amax[3];
	float  Bmin[3], Bmax[3];
	int    i, j;

	static float M[16]; // OpenGL matrix. Column-major mode !

	GetGLMatrixTrfm3D(T, M); // Copy transf. to array

	/*Copy box geomBox into a min array and a max array for easy reference.*/

	Amin[0] = oldBBox->xMin;  Amax[0] = oldBBox->xMax;
	Amin[1] = oldBBox->yMin;  Amax[1] = oldBBox->yMax;
	Amin[2] = oldBBox->zMin;  Amax[2] = oldBBox->zMax;

	/* Take care of translation by beginning at T. */

	Bmin[0] = Bmax[0] = M[12]; // T->trX
	Bmin[1] = Bmax[1] = M[13]; // T->trY
	Bmin[2] = Bmax[2] = M[14]; // T->trZ

	/* Now find the extreme points by considering the product of the */
	/* min and max with each component of M.  */

	for( i = 0; i < 3; i++ )
		for( j = 0; j < 3; j++ ) {
			a = M[i + 4*j] * Amin[j]; // M.element[i][j] * Amin[j];
			b = M[i + 4*j] * Amax[j]; // M.element[i][j] * Amax[j];
			if( a < b )  {
				Bmin[i] += a;
				Bmax[i] += b;
			}
			else {
				Bmin[i] += b;
				Bmax[i] += a;
			}
		}

	/* Copy the result into the new box. */

	newBBox->xMin = Bmin[0];
	newBBox->yMin = Bmin[1];
	newBBox->zMin = Bmin[2];

	newBBox->xMax = Bmax[0];
	newBBox->yMax = Bmax[1];
	newBBox->zMax = Bmax[2];

	newBBox->vbo_uptodate = 0;
}


void  BoxBox( BBox *bba, const BBox *bbb ) {

	if (bbb->xMin < bba->xMin) bba->xMin = bbb->xMin;
	if (bbb->yMin < bba->yMin) bba->yMin = bbb->yMin;
	if (bbb->zMin < bba->zMin) bba->zMin = bbb->zMin;

	if (bbb->xMax > bba->xMax) bba->xMax = bbb->xMax;
	if (bbb->yMax > bba->yMax) bba->yMax = bbb->yMax;
	if (bbb->zMax > bba->zMax) bba->zMax = bbb->zMax;

	bba->vbo_uptodate = 0;
}

void PrintBBox(const BBox * b) {
	printf("PMin = (%.2f %.2f %.2f) ", b->xMin, b->yMin, b->zMin);
	printf("PMax = (%.2f %.2f %.2f)\n", b->xMax, b->yMax, b->zMax);
}
