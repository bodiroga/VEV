#ifndef BBOX_H
#define BBOX_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include "trfm3D.h"

typedef struct _BBox {

	float    xMin;
	float    xMax;
	float    yMin;
	float    yMax;
	float    zMin;
	float    zMax;

	// VBO
	GLuint  vbo_id;    // Vertex Buffer Object id
	GLuint  idxvbo_id; // Index Vertex Buffer Object id
	int     vbo_uptodate;

} BBox;

void     DestroyBBox( BBox **theBBox );
BBox    *CreateVoidBBox( );
BBox    *CreateBBox( float xmin, float ymin, float zmin, float xmax, float ymax, float zmax );

void PrintBBox(const BBox * b);

/**
 * Set void BBox
 *
 * @param theBBox the BBox
 */
void SetVoidBBox(BBox *theBBox);

/**
 * Copy source bounding box to target
 *
 * @param target
 * @param source
 */

void CopyBBox(BBox * target, const BBox * source);

/**
 * TransformBBox: given an axis aligned BBOX and a transformation, calculate
 * the new BBox coordinates
 *
 * @param newBBox    where the new BBox goes
 * @param oldBBox    pointer to the old BBox
 * @param T a transformation
 *
 * \note: newBBox and oldBBox can point to the same BBox safely
 */

void TransformBBox ( BBox * newBBox,
					 const BBox * oldBBox,
					 const trfm3D * T);

/**
 * BoxBox: Given two BBoxes, modify the first, so that it includes both of them
 *
 * @param oneBBoxA: a BBox, and where the final BBox will be put
 * @param oneBBoxB: a const BBox
 *
 */

void  BoxBox( BBox *oneBBoxA, const BBox *oneBBoxB );

#endif /* BBOX_H */
