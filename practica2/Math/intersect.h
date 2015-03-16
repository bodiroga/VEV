#ifndef INTERSECT_H
#define INTERSECT_H

#include "plane.h"
#include "bbox.h"
#include "bsphere.h"
#include "line.h"

#define IREJECT 1
#define IINTERSECT 0

/**
 * Check whether two BBoxes intersect
 *
 * @param bba: const BBox
 * @param bbb: const BBox
 *
 * @return IINTERSECT: intersects, IREJECT: don't intersect
 */

int  BBoxBBoxIntersect(BBox *bba, BBox *bbb );

/**
 * Check whether a BBox intersects a given plane
 *
 * @param theBBox pointer to BBox (can't be NULL)
 * @param thePlane pointer to a plane (can't be NULL)
 *
 * @return  IINTERSECT  plane intersects box
 +IREJECT box is outside the plane
 -IREJECT box is inside the plane
*/

int BBoxPlaneIntersect(BBox *theBBox, plane *thePlane);

/**
 * Check whether two BSpheres intersect
 *
 * @param oneBSphereA pointer to Bsphere
 * @param oneBSphereB pointer to Bsphere
 *
 * @return IINTERSECT: intersect; IREJECT: don't intersect
 */
int BSphereIntersectBSphere( BSphere *oneBSphereA, BSphere *oneBSphereB );

/**
 * Check whether a BSphere intersects with a plane
 *
 * @param oneBSphereA pointer to Bsphere
 * @param oneBBox pointer to plane
 *
 * @return  IINTERSECT  plane intersects bsphere
 +IREJECT bsphere is outside the plane
 -IREJECT bsphere is inside the plane
*/
int BSpherePlaneIntersect( BSphere *oneBSphere, plane *oneBBox );

/**
 * Check whether a BSphere intersects with a BBox
 *
 * @param oneBSphereA pointer to Bsphere
 * @param oneBBox pointer to BBox
 *
 * @return IINTERSECT: intersect; IREJECT: don't intersect
 */
int BSphereBBoxIntersect( BSphere *oneBSphere, BBox *oneBBox );

/**
 * Find whether a line (ray) intersects with a triangle defined by three points (P0, P1, P2).
 *
 * @param P0x x coord of P0
 * @param P0y y coord of P0
 * @param P0z z coord of P0
 * @param P1x x coord of P1
 * @param P1y y coord of P1
 * @param P1z z coord of P1
 * @param P2x x coord of P2
 * @param P2y y coord of P2
 * @param P2z z coord of P2
 * @param l the line
 * @param u baricentric coordinate of intersection point. See result.
 * @param v baricentric coordinate of intersection point. See result.
 * @param t line parameter for intersection point. Can be used as a signed distance from line to triangle.
 *
 * @return IINTERSECT: intersect; IREJECT: don't intersect
 *         when IINTERSECT, set (u, v, t) so that the point inside the triangle is
 *         P_intersect = (1 - u - v)P0 + uP1 + vP2
 *         P_intersect = l->o + t * l->d
 */

int IntersetTriangleRay(float P0x, float P0y, float P0z,
						float P1x, float P1y, float P1z,
						float P2x, float P2y, float P2z,
						line *l,
						float *u, float *v, float *t);


const char *intersect_string(int intersect);

#endif
