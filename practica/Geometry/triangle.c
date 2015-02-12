#include <GL/glut.h>
#include "tools.h"
#include "triangle.h"
#include <string.h>
#include <assert.h>


void DestroyTriangle( triangle **theTriangle ) {

	triangle *thisTriangle = *theTriangle;
	if( ! thisTriangle ) return;

	DestroyPlane(&thisTriangle->PNor);
	free( thisTriangle );

	*theTriangle = NULL;
}

void SetTrianglePlane( triangle *thisTriangle ) {

	float     ux,  uy,  uz;
	float     vx,  vy,  vz;
	float     nx,  ny,  nz;
	int       ierr;

	// u = v1 - v0
	ux = thisTriangle->vertices[3]     - thisTriangle->vertices[0];
	uy = thisTriangle->vertices[3 + 1] - thisTriangle->vertices[1];
	uz = thisTriangle->vertices[3 + 2] - thisTriangle->vertices[2];

	// v = v2 - v0
	vx = thisTriangle->vertices[3*2]     - thisTriangle->vertices[0];
	vy = thisTriangle->vertices[3*2 + 1] - thisTriangle->vertices[1];
	vz = thisTriangle->vertices[3*2 + 2] - thisTriangle->vertices[2];

	// n = u x v
	ierr = FALSE;
	crossVV(&ierr,
			&nx, &ny, &nz,
			ux, uy, uz,
			vx, vy, vz);

	VectorNormalize( &ierr, &nx, &ny, &nz );

	thisTriangle->PNor = CreatePlaneCoef(nx, ny, nz,
										 -(nx*thisTriangle->vertices[0] +
										   nx*thisTriangle->vertices[1] +
										   nx*thisTriangle->vertices[2]));
}

triangle *CreateTriangle( int vertN, float *xx, float *yy, float *zz ) {

	triangle     *newTriangle;
	int                      i;
	newTriangle = malloc( sizeof(*newTriangle) * 1 );

	newTriangle->type = AGA_TR;

	for(i=0; i<3; ++i) {
		newTriangle->vertices[3*i] = xx[i];
		newTriangle->vertices[3*i + 1] = yy[i];
		newTriangle->vertices[3*i + 2] = zz[i];
	}

	newTriangle->PNor = CreateVoidPlane();
	SetTrianglePlane( newTriangle );

	/*
	//printf( "N %f %f %f \n", newTriangle->thisTriangle->PNor->nx, newTriangle->thisTriangle->PNor->ny,
	newTriangle->thisTriangle->PNor->nz );
	for ( i=0; i<vertN; i++ ) {
	//printf( "V %f %f %f \n", newTriangle->coorX[i], newTriangle->coorY[i],
	newTriangle->coorZ[i] );
	}
	*/

	return newTriangle;
}

void IncBoxTriangle( triangle *thisTriangle, float *minX, float *maxX,
					 float *minY, float *maxY, float *minZ, float *maxZ ) {

	int      i;

	for( i=0; i < 3 ; ++i ) {
		if( thisTriangle->vertices[3*i] < *minX) *minX = thisTriangle->vertices[3*i];
		if( thisTriangle->vertices[3*i] > *maxX) *maxX = thisTriangle->vertices[3*i];

		if( thisTriangle->vertices[3*i+1] < *minY) *minY = thisTriangle->vertices[3*i+1];
		if( thisTriangle->vertices[3*i+1] > *maxY) *maxY = thisTriangle->vertices[3*i+1];

		if( thisTriangle->vertices[3*i+2] < *minZ) *minZ = thisTriangle->vertices[3*i+2];
		if( thisTriangle->vertices[3*i+2] > *maxZ) *maxZ = thisTriangle->vertices[3*i+2];
	}
}

void AddVertsNormalsTriangle( triangle *thisTriangle, float *xx, float *yy, float *zz ) {

	int i;

	thisTriangle->type |= AGA_TR_NORMAL;

	for(i=0; i<3; ++i) {
		thisTriangle->normals[3*i]     = xx[i];
		thisTriangle->normals[3*i + 1] = yy[i];
		thisTriangle->normals[3*i + 2] = zz[i];
	}
}

void AddVertsTextureTriangle( triangle *thisTriangle, float *u, float *v ) {

	int i;

	thisTriangle->type |= AGA_TR_TEX;

	for(i=0; i<3; ++i) {
		thisTriangle->tex_coords[2*i]     = u[i];
		thisTriangle->tex_coords[2*i + 1] = v[i];
	}
}


void TranslateTriangle( triangle *thisTriangle, float x, float y, float z ) {

	int i;

	for( i=0; i<3; i++ ) {
		thisTriangle->vertices[3*i] += x;
		thisTriangle->vertices[3*i + 1] += y;
		thisTriangle->vertices[3*i + 2] += z;
	}
	TranslatePlane(thisTriangle->PNor, thisTriangle->vertices[0], thisTriangle->vertices[1], thisTriangle->vertices[2]);
}

int RayIntersectTriangle( triangle *thisTriangle, line *l, float *u, float *v, float *t) {

	return IntersectTriangleRay(thisTriangle->vertices[0], thisTriangle->vertices[1], thisTriangle->vertices[2],
								thisTriangle->vertices[3], thisTriangle->vertices[4], thisTriangle->vertices[5],
								thisTriangle->vertices[6], thisTriangle->vertices[7], thisTriangle->vertices[8],
								l, u, v, t);
}

void DrawGLTriangle( triangle *thisTriangle ) {

	int  i;

	glNormal3f( thisTriangle->PNor->nx, thisTriangle->PNor->ny, thisTriangle->PNor->nz );
	glBegin( GL_TRIANGLES);
	for(i=0; i<3; ++i) {
		if (thisTriangle->type & AGA_TR_NORMAL) {
			glNormal3fv(&thisTriangle->normals[3*i]);
		}
		if (thisTriangle->type & AGA_TR_TEX) {
			glTexCoord2fv(&thisTriangle->tex_coords[2*i]);
		}
		glVertex3fv(&thisTriangle->vertices[3*i]);
	}
	glEnd();
}


/* int GetTriangleSegmentCut( triangle *thisTriangle, float  ax,   float  ay,   float  az, */
/*			  float  bx,   float  by,   float  bz, */
/*			  float *cutX, float *cutY, float *cutZ, */
/*			  float *cutLanda ) */
/* { */
/*   // Returns TRUE if segment AB cuts the triangle */

/*   // If they cut, the cutting point is stored in CUT */
/*   //              and landa will be set to the cut position ( 0 at A, 1 at B ) */

/*   float  A, B, C, D; */
/*   int    semiSpaceA, semiSpaceB; */
/*   float  landa; */

/*   A = thisTriangle->PNor->nx; */
/*   B = thisTriangle->PNor->ny; */
/*   C = thisTriangle->PNor->nz; */
/*   D = thisTriangle->PNor->d; */

/*   // Check whether the segment crosses the triangle plane */
/*   semiSpaceA = WhichSidePlane(thisTriangle->PNor, ax, ay, az); */
/*   semiSpaceB = WhichSidePlane(thisTriangle->PNor, bx, by, bz); */
/*   if( semiSpaceA > 0 ) { */
/*     if( semiSpaceB > 0 ) return FALSE; */
/*   } */
/*   else { */
/*     if( semiSpaceB < 0f ) return FALSE; */
/*   } */

/*   // Check if the segment cuts the triangle´s plane */
/*   // this two equations must be verified by point C */
/*   // n.c + D = 0 and  c = a + ab * landa */
/*   // c is in plane    c is in the line ( landa=0->c=a landa=1->c=b ) */
/*   // */
/*   // so,   n.( a + ab * landa ) + D = 0 */
/*   // and,  landa = - ( n.a + D  ) / (n.ab) */
/*   //             = -  semiSpaceA  / (n.ab) */
/*   // we have already checked that the denominator will never be zero */
/*   landa = -semiSpaceA / ( A * (bx-ax) + B * (by-ay) + C * (bz-az) ); */
/*   // if the cutting point c is outside, then there is no cut */
/*   if( landa < 0.0f || landa > 1.0f ) return FALSE; */

/*   // Check whether C is inside the triangle */
/*   *cutX = ax + (bx-ax) * landa; */
/*   *cutY = ay + (by-ay) * landa; */
/*   *cutZ = az + (bz-az) * landa; */
/*   *cutLanda = landa; */
/*   return GetPointInTriangle( thisTriangle, *cutX, *cutY, *cutZ ); */
/* } */

/* int GetPointInTriangle( triangle *thisTriangle, float ax, float ay, float az ) */
/* { */
/*   // Assumes that point A is within the plane that contains the triangle */
/*   // Returns TRUE if the point is inside the triangle */
/*   float   *polx, *poly; */

/*   // by default we use projection onto XY plane to solve the point in problem */
/*   // However, if the triangle normal has an small Z component we will use another plane */
/*   polx = thisTriangle->coorX; */
/*   poly = thisTriangle->coorY; */
/*   if( thisTriangle->thisTriangle->PNor->nz > 0.1f || thisTriangle->thisTriangle->PNor->nz < -0.1f ) { */
/*     // use XY projection */
/*   } */
/*   else { */
/*     if( thisTriangle->thisTriangle->PNor->ny > 0.1f || thisTriangle->thisTriangle->PNor->ny < -0.1f ) { */
/*       // use XZ projection */
/*       poly = thisTriangle->coorZ; */
/*       ay   = az; */
/*     } */
/*     else { */
/*       // use YZ projection */
/*       polx = thisTriangle->coorZ; */
/*       ax   = az; */
/*     } */
/*   } */

/*   return PointInTriangle2D( thisTriangle->numVertices, polx, poly, ax, ay ); */
/* } */


void PrintTriangle (triangle *thisTriangle) {

	int i;

	for(i=0; i < 3; ++i) {
		printf("(%.2f, %.2f, %.2f) ", thisTriangle->vertices[3*i], thisTriangle->vertices[3*i+1], thisTriangle->vertices[3*i+2]);
	}
	printf("\n");
	if (thisTriangle->type & AGA_TR_NORMAL) {
		for(i=0; i < 3; ++i) {
			printf("(%.2f, %.2f, %.2f) ", thisTriangle->normals[3*i], thisTriangle->normals[3*i+1], thisTriangle->normals[3*i+2]);
		}
		printf("\n");
	}
	if (thisTriangle->type & AGA_TR_NORMAL) {
		for(i=0; i < 3; ++i) {
			printf("(%.2f, %.2f) ", thisTriangle->tex_coords[2*i], thisTriangle->tex_coords[2*i+1]);
		}
		printf("\n");
	}
	printf("(%.2f, %.2f, %.2f, %.2f)\n", thisTriangle->PNor->nx, thisTriangle->PNor->ny, thisTriangle->PNor->nz, thisTriangle->PNor->d);

}
