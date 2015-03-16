#ifndef POLYGON_H
#define POLYGON_H

#include <stdio.h>

typedef struct _Polygon {

	int     numVertices;

	/*!
	  \name Coordinate of vertices
	*/
	//@{
	float   *coorX;
	float   *coorY;
	float   *coorZ;
	//@}

	/*!
	  \name Surface normals at vertives
	  \note NULL if there are no Surface Normals at vertices
	*/
	//@{
	float   *SNorX;
	float   *SNorY;
	float   *SNorZ;
	//@}

	/*!
	  \name Texture coordinates at vertices
	  \note NULL if there are no Texture Coordinates at vertices
	*/
	//
	//@{
	float   *txtU;
	float   *txtV;
	//@}

	/*!
	  \name Polygon Normal vector components and plane ecuation

	  PNor*X = planeD
	*/
	//@{
	float    PNorX, PNorY, PNorZ;
	float    planeD;
	//@}
} polygon;

void     DestroyPolygon( polygon **thePolygon );

/*!

  Creates a new polygon with no texture coordinates

  \param vertN: number of vertices
  \param xx: array of X coordinates of the vertices
  \param yy: array of Y coordinates of the vertices
  \param zz: array of Z coordinates of the vertices

  \return a pointer to the new created polygon

*/

polygon *CreatePolygon( int vertN, float *xx, float *yy, float *zz );


//! Return the polygon bounding box limits in its (local) modeling reference system

void IncBoxPolygon( polygon *thisPolygon, float *xmin, float *xmax,
					float *ymin, float *ymax, float *zmin, float *zmax );

//! Set the normals of the vertices of the polygon

void AddVertsNormals( polygon *thisPolygon, float *xx, float *yy, float *zz );

//! Set the texture coordinates of the vertices of the polygon
void AddVertsTexture( polygon *thisPolygon, float *u, float *v );

//! Calculate the normals of the vertices
void SetPolygonNormal( polygon *thisPolygon );


//! Translates the polygon a vector (x,y,z)

/*!
  This function performs two steps
  - Translate all the vertices of the polygon
  - calculates the new distance of the plane (polygon#planeD)
*/

void TranslatePolygon( polygon *thisPolygon, float x, float y, float z );

// TRUE if segment AB crosses the polygon ; returns the point where they Cut

/*!   Returns TRUE if segment AB cuts the polygon
  \return If they cut, the cutting point is stored in CUT
  and landa will be set to the cut position ( 0 at A, 1 at B )
*/

int GetPolygonSegmentCut( polygon *thisPolygon, float  ax,   float  ay,   float  az,
						  float  bx,   float  by,   float  bz,
						  float *cutX, float *cutY, float *cutZ,
						  float *landa );

//! given A that is in the plane defined by polygon, TRUE if A is inside the polygon

/*
  Assumes that point A is within the plane that contains the polygon
  \return TRUE if the point is inside the polygon

*/

int GetPointInPolygon( polygon *thisPolygon, float ax, float ay, float az );

void PrintPolygon (polygon *thisPolygon);

#endif /* POLYGON_H */
