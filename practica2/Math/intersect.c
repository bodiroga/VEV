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

	if (bba->xMin > bbb->xMax || bbb->xMin > bba->xMax) return IREJECT;
	if (bba->yMin > bbb->yMax || bbb->yMin > bba->yMax) return IREJECT;
	if (bba->zMin > bbb->zMax || bbb->zMin > bba->zMax) return IREJECT;

	return IINTERSECT;
}

// @@ TODO: test if a BBox and a plane intersect.
//! Returns :
//   +IREJECT outside
//   -IREJECT inside
//    IINTERSECT intersect

int  BBoxPlaneIntersect (BBox *theBBox, plane *thePlane) {

	float Vmin_x, Vmin_y, Vmin_z;
	float Vmax_x, Vmax_y, Vmax_z;

	int min_side;
	int max_side;

	if (thePlane->nx >= 0.0) {
		Vmin_x = theBBox->xMin;
		Vmax_x = theBBox->xMax;
	} else {
		Vmin_x = theBBox->xMax;
		Vmax_x = theBBox->xMin;
	}

	if (thePlane->ny >= 0.0) {
		Vmin_y = theBBox->yMin;
		Vmax_y = theBBox->yMax;
	} else {
		Vmin_y = theBBox->yMax;
		Vmax_y = theBBox->yMin;
	}

	if (thePlane->nz >= 0.0) {
		Vmin_z = theBBox->zMin;
		Vmax_z = theBBox->zMax;
	} else {
		Vmin_z = theBBox->zMax;
		Vmax_z = theBBox->zMin;
	}

	min_side = WhichSidePlane(thePlane, Vmin_x, Vmin_y, Vmin_z);
	if (min_side > 0) return IREJECT;  // Vmin on possitive side: bbox outside
	max_side = WhichSidePlane(thePlane, Vmax_x, Vmax_y, Vmax_z);
	if (max_side < 0) return -IREJECT;  // Vmax on negative side: bbox inside
	return IINTERSECT; // Intersecting
}


// @@ TODO: test if two BSpheres intersect.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereIntersectBSphere(BSphere *bsa, BSphere *bsb ) {

	float lx, ly, lz;
	float ls, rs;

	lx = bsa->sphereX - bsb->sphereX;
	ly = bsa->sphereY - bsb->sphereY;
	lz = bsa->sphereZ - bsb->sphereZ;

	ls = DOT_PRODUCT(lx, ly, lz, lx, ly, lz);
	rs = bsa->radius + bsb->radius;
	if (ls > (rs * rs)) return IREJECT; // Disjoint
	return IINTERSECT; // Intersect
}

// @@ TODO: test if a BSpheres intersects a plane.
//! Returns :
//   -IREJECT outside
//   +IREJECT inside
//    IINTERSECT intersect

int BSpherePlaneIntersect( BSphere *bs, plane *pl ) {

	float dist;

	dist = PlaneSignedDistance(pl, bs->sphereX, bs->sphereY, bs->sphereZ);
	if (fabs(dist) < bs->radius)
		return IINTERSECT; // Intersect
	if (dist < 0)
		return -IREJECT; // BSphere inside plane
	return +IREJECT; // BSphere outside plane
}

// @@ TODO: test if a BSpheres intersect a BBox.
//! Returns :
//    IREJECT don't intersect
//    IINTERSECT intersect

int BSphereBBoxIntersect(BSphere *sphere, BBox *box ) {

	float d;
	float aux;
	float r;

	r = sphere->radius;
	d = 0;

	aux = sphere->sphereX - box->xMin;
	if (aux < 0) {
		if (aux < -r) return IREJECT;
		d += aux*aux;
	} else {
		aux = sphere->sphereX - box->xMax;
		if (aux > 0) {
			if (aux > r) return IREJECT;
			d += aux*aux;
		}
	}

	aux = (sphere->sphereY - box->yMin);
	if (aux < 0) {
		if (aux < -r) return IREJECT;
		d += aux*aux;
	} else {
		aux = sphere->sphereY - box->yMax;
		if (aux > 0) {
			if (aux > r) return IREJECT;
			d += aux*aux;
		}
	}

	aux = sphere->sphereZ - box->zMin;
	if (aux < 0) {
		if (aux < -r) return IREJECT;
		d += aux*aux;
	} else {
		aux = sphere->sphereZ - box->zMax;
		if (aux > 0) {
			if (aux > r) return IREJECT;
			d += aux*aux;
		}
	}
	if (d > r * r) return IREJECT;
	return IINTERSECT;
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
