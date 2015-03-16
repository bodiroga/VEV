#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdio.h>
#include "plane.h"
#include "line.h"

#define AGA_TR          0
#define AGA_TR_NORMAL   1
#define AGA_TR_TEX      2

typedef struct _Triangle {

	int     type; // AGA_TR, AGA_TR_NORMAL, AGA_TR_TEX. Default AGA_TR

	/*!
	  \name Coordinate of vertices
	*/
	//@{
	float   vertices[9]; // 3 vertices * 3 coords
	//@}

	/*!
	  \name Surface normals at vertives
	  \note NULL if there are no Surface Normals at vertices
	*/
	//@{
	float   normals[9]; // 3 vertices * 3 coords
	//@}

	/*!
	  \name Texture coordinates at vertices
	  \note NULL if there are no Texture Coordinates at vertices
	*/
	//
	//@{
	float   tex_coords[6]; // 3 vertices * 2 coords
	//@}

	/*!
	  \name Triangle Normal vector components and plane ecuation

	  PNor*X = planeD
	*/
	//@{
	plane   *PNor;
	//@}
} triangle;

void     DestroyTriangle( triangle **theTriangle );

/*!

  Creates a new triangle with no texture coordinates

  \param vertN: number of vertices
  \param xx: array of X coordinates of the vertices
  \param yy: array of Y coordinates of the vertices
  \param zz: array of Z coordinates of the vertices

  \return a pointer to the new created triangle

*/

triangle *CreateTriangle( int vertN, float *xx, float *yy, float *zz );


void DrawGLTriangle( triangle *thisTriangle );


//! Return the triangle bounding box limits in its (local) modeling reference system

void IncBoxTriangle( triangle *thisTriangle, float *xmin, float *xmax,
					 float *ymin, float *ymax, float *zmin, float *zmax );

//! Set the normals of the vertices of the triangle

void AddVertsNormalsTriangle( triangle *thisTriangle, float *xx, float *yy, float *zz );

//! Set the texture coordinates of the vertices of the triangle
void AddVertsTextureTriangle( triangle *thisTriangle, float *u, float *v );

//! Calculate the normals of the vertices
void SetTrianglePlane( triangle *thisTriangle );


//! Translates the triangle a vector (x,y,z)

/*!
  This function performs two steps
  - Translate all the vertices of the triangle
  - calculates the new distance of the plane (triangle#planeD)
*/

void TranslateTriangle( triangle *thisTriangle, float x, float y, float z );

// TRUE if segment AB crosses the triangle ; returns the point where they Cut

/*!   Returns TRUE if segment AB cuts the triangle
  \return If they cut, the cutting point is stored in CUT
  and landa will be set to the cut position ( 0 at A, 1 at B )
*/

int GetTriangleSegmentCut( triangle *thisTriangle, float  ax,   float  ay,   float  az,
						   float  bx,   float  by,   float  bz,
						   float *cutX, float *cutY, float *cutZ,
						   float *landa );

//! given A that is in the plane defined by triangle, TRUE if A is inside the triangle

/*
  Assumes that point A is within the plane that contains the triangle
  \return TRUE if the point is inside the triangle

*/

int GetPointInTriangle( triangle *thisTriangle, float ax, float ay, float az );

void PrintTriangle (triangle *thisTriangle);

#endif /* TRIANGLE_H */
