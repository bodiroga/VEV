#ifndef PLANE_H
#define PLANE_H

/*!
  \file plane.h

  Planes

  The plane internally is stored by means of a unit length normal N and a
  distance d

  The eq. of the plane is:

  N*X = d

*/

typedef struct _Plane {

	//! Plane normal
	float nx, ny, nz;
	//! Distance
	float d;
	//! whether the plane is normalized
	int   is_norm;
} plane;


/**
 * Create a plane aligned with the XY plane: n=(0,0,1) d=0
 *
 *
 * @return the new normalized plane
 */
plane *CreateVoidPlane();


/*!
  Given 4 algebraic coefficients (a,b,c,l) , create a plane with eq.

  N*X = d

  where N is a unit length normal N and d a distance

  \return the new normalized plane

*/

plane *CreatePlane(float a, float b, float c, float l);

/*!
  Given 4 algebraic coefficients (a,b,c,l) , create a plane with eq.

  ax + by + cz = l

  Note that the plane is not normalized

  \return the new (no normalized) plane
*/

plane *CreatePlaneCoef(float a, float b, float c, float d);

void DestroyPlane(plane **thePlane);

/**
 * Normalize the plane (normalize plane normal and set distance accordingly)
 *
 * @param thePlane the plane
 */

void NormalizePlane(plane *thePlane);

//! Decides in which side a point lies respect to a plane (in plane, in
//! possitive side, in negative side)
/*!
  \param pl: A pointer to a plane (can't be NULL)
  \param Px: pointer to the x component of point
  \param Py: pointer to the y component of point
  \param Pz: pointer to the z component of point

  \return
  - 0  point lies on the plane
  - +1 point lies outside the plane (possitive side)
  - -1 point lies inside the plane (negative side)

  \note the plane must not be normalized

*/

int WhichSidePlane(plane * pl, float Px, float Py, float Pz);

/**
 * Translates a plane so that it passes through (Px, Py, Pz)
 *
 * @param pl a plane
 * @param x x coord
 * @param y y coord
 * @param z z coord
 *
 * \note the plane is normalized
 */

void TranslatePlane(plane *pl, float x, float y, float z);

/**
 * Calculates the distance of a Point w.r.t a plane.
 *
 * @param pl a plane
 * @param Px x coord of P
 * @param Py y coord of P
 * @param Pz z coord of P
 *
 * @return the distance to the plane. Always positive.
 *
 * \note the plane is normalized
 */

float PlaneDistance(plane * pl, float Px, float Py, float Pz);

/**
 * Calculates the signed distance of a Point w.r.t a plane. If the return value
 * is negative, it tells that the point is on the back-side of the plane.
 *
 * @param pl a plane
 * @param Px x coord of P
 * @param Py y coord of P
 * @param Pz z coord of P
 *
 * @return the signed distance to the plane.
 *
 * \note the plane is normalized
 */

float PlaneSignedDistance(plane * pl, float Px, float Py, float Pz);

#endif
