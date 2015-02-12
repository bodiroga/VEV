#include "intersect.h"
#include "tools.h"

/* | algo           | difficulty | */
/* |----------------+------------| */
/* | BSPhereBSPhere |          1 | */
/* | BSPherePlane   |          2 | */
/* | BBoxBBox       |          3 | */
/* | BSphereBBox    |          5 | */
/* | BBoxPlane      |          5 | */


// @@ TODO: test if two BBoxes intersect.
//! Returns :
//    IINTERSECT intersect
//    IREJECT don't intersect

int  BBoxBBoxIntersect(BBox *bba, BBox *bbb ) {

}

// @@ TODO: test if a BBox and a plane intersect.
//! Returns :
//   +IREJECT outside
//   -IREJECT inside
//    IINTERSECT intersect

int  BBoxPlaneIntersect (BBox *theBBox, plane *thePlane) {

}


// @@ TODO: test if two BSpheres intersect.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereIntersectBSphere(BSphere *bsa, BSphere *bsb ) {

}

// @@ TODO: test if a BSpheres intersects a plane.
//! Returns :
//   -IREJECT outside
//   +IREJECT inside
//    IINTERSECT intersect

int BSpherePlaneIntersect( BSphere *bs, plane *pl ) {

}

// @@ TODO: test if a BSpheres intersect a BBox.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereBBoxIntersect(BSphere *sphere, BBox *box ) {

}


int IntersectTriangleRay(float P0x, float P0y, float P0z,
						 float P1x, float P1y, float P1z,
						 float P2x, float P2y, float P2z,
						 line *l,
						 float *u, float *v, float *t) {
	float e1[3];
	float e2[3];
	float p[3];
	float s[3];
	float q[3];
	float a, f;
	float lu, lv, lt;

	*u=0;
	*v=0;
	*t=0;

	// e1 = P1 - P0
	e1[0] = P1x - P0x;
	e1[1] = P1y - P0x;
	e1[2] = P1z - P0z;

	// e2 = P2 - P0
	e2[0] = P2x - P0x;
	e2[1] = P2y - P0y;
	e2[2] = P2z - P0z;

	crossVV(&p[0], &p[1], &p[2],
			l->dx, l->dy, l->dz,
			e2[0], e2[1], e2[2]);

	a = DOT_PRODUCT(e1[0], e1[1], e1[2],
					p[0], p[1], p[2]);

	if (a > -DISTANCE_EPSILON && a < DISTANCE_EPSILON) return IREJECT;

	f = 1.0 / a;

	// s = l->o - P0
	s[0] = l->ox - P0x;
	s[1] = l->oy - P0y;
	s[2] = l->oz - P0z;

	lu = f * DOT_PRODUCT(s[0], s[1], s[2],
						 p[0], p[1], p[2]);

	if (lu < 0.0 || lu > 1.0) return IREJECT;

	crossVV(&q[0], &q[1], &q[2],
			s[0], s[1], s[2],
			e1[0], e1[1], e1[2]);

	lv = f * DOT_PRODUCT(l->dx, l->dy, l->dz,
						 q[0], q[1], q[2]);

	if (lv < 0.0 || lv > 1.0) return IREJECT;

	lt = f * DOT_PRODUCT(e2[0], e2[1], e2[2],
						 q[0], q[1], q[2]);

	*u = lu;
	*v = lv;
	*t = lt;
	return IINTERSECT;
}

/* IREJECT 1 */
/* IINTERSECT 0 */

const char *intersect_string(int intersect) {

	static const char *iint = "IINTERSECT";
	static const char *prej = "IREJECT";
	static const char *mrej = "-IREJECT";
	static const char *error = "IERROR";

	const char *result = error;

	switch (intersect) {
	case IINTERSECT:
		result = iint;
		break;
	case +IREJECT:
		result = prej;
		break;
	case -IREJECT:
		result = mrej;
		break;
	}
	return result;
}
