#include "tools.h"
#include "polygon.h"
#include <string.h>
#include <assert.h>

// Changes from v00 to v01
//
// vector normalization is now carried out with a procedure
// from tools.c : VectorNormalize(...)
//

void DestroyPolygon( polygon **thePolygon ) {

	polygon *thisPolygon = *thePolygon;
	if( ! thisPolygon ) return;

	free( thisPolygon->coorX );
	free( thisPolygon->coorY );
	free( thisPolygon->coorZ );
	if( thisPolygon->txtU ) free( thisPolygon->txtU );
	if( thisPolygon->txtV ) free( thisPolygon->txtV );
	if( thisPolygon->SNorX ) free( thisPolygon->SNorX );
	if( thisPolygon->SNorY ) free( thisPolygon->SNorY );
	if( thisPolygon->SNorZ ) free( thisPolygon->SNorZ );
	free( thisPolygon );

	*thePolygon = NULL;
}

polygon *CreatePolygon( int vertN, float *xx, float *yy, float *zz ) {

	polygon     *newPolygon;
	//int                      i;

	newPolygon = malloc( sizeof(*newPolygon) * 1 );

	newPolygon->coorX = NULL;
	newPolygon->coorY = NULL;
	newPolygon->coorZ = NULL;
	newPolygon->SNorX = NULL;
	newPolygon->SNorY = NULL;
	newPolygon->SNorZ = NULL;
	newPolygon->txtU  = NULL;
	newPolygon->txtV  = NULL;
	newPolygon->planeD  = 0.0;
	newPolygon->numVertices = vertN;
	if (!vertN)
		return newPolygon;
	newPolygon->coorX = malloc( sizeof(*(newPolygon->coorX)) * vertN );
	newPolygon->coorY = malloc( sizeof(*(newPolygon->coorY)) * vertN );
	newPolygon->coorZ = malloc( sizeof(*(newPolygon->coorZ)) * vertN );

	// better with memcpy ?
	memcpy(newPolygon->coorX, xx, vertN * sizeof(*xx));
	memcpy(newPolygon->coorY, yy, vertN * sizeof(*yy));
	memcpy(newPolygon->coorZ, zz, vertN * sizeof(*zz));
	/*   for ( i=0; i<vertN; i++ ) { */
	/*     newPolygon->coorX[i] = xx[i]; */
	/*     newPolygon->coorY[i] = yy[i]; */
	/*     newPolygon->coorZ[i] = zz[i]; */
	/*   } */

	SetPolygonNormal( newPolygon );

	newPolygon->planeD = - (newPolygon->PNorX * xx[0] +
							newPolygon->PNorY * yy[0] +
							newPolygon->PNorZ * zz[0]   );

	/*
	//printf( "N %f %f %f \n", newPolygon->PNorX, newPolygon->PNorY,
	newPolygon->PNorZ );
	for ( i=0; i<vertN; i++ ) {
	//printf( "V %f %f %f \n", newPolygon->coorX[i], newPolygon->coorY[i],
	newPolygon->coorZ[i] );
	}
	*/

	return newPolygon;
}

void IncBoxPolygon( polygon *thisPolygon, float *minX, float *maxX,
					float *minY, float *maxY, float *minZ, float *maxZ ) {

	int      iii, nnn;
	float   *cx,  *cy,  *cz;

	nnn = thisPolygon->numVertices;
	cx  = thisPolygon->coorX;
	cy  = thisPolygon->coorY;
	cz  = thisPolygon->coorZ;

	for( iii=0; iii < nnn ; iii++ ) {
		if( *cx < *minX ) *minX = *cx;
		if( *cx > *maxX ) *maxX = *cx;
		if( *cy < *minY ) *minY = *cy;
		if( *cy > *maxY ) *maxY = *cy;
		if( *cz < *minZ ) *minZ = *cz;
		if( *cz > *maxZ ) *maxZ = *cz;
		cx++; cy++; cz++;
	}
}

void AddVertsNormals( polygon *thisPolygon, float *xx, float *yy, float *zz ) {

	//int      iii;
	int nnn;

	nnn = thisPolygon->numVertices;
	thisPolygon->SNorX = malloc( sizeof(*(thisPolygon->SNorX)) * nnn );
	thisPolygon->SNorY = malloc( sizeof(*(thisPolygon->SNorY)) * nnn );
	thisPolygon->SNorZ = malloc( sizeof(*(thisPolygon->SNorZ)) * nnn );

	memcpy(thisPolygon->SNorX, xx, nnn * sizeof(*xx));
	memcpy(thisPolygon->SNorY, yy, nnn * sizeof(*yy));
	memcpy(thisPolygon->SNorZ, zz, nnn * sizeof(*zz));

	/*   for ( iii=0; iii<nnn; iii++ ) { */
	/*     thisPolygon->SNorX[iii] = xx[iii]; */
	/*     thisPolygon->SNorY[iii] = yy[iii]; */
	/*     thisPolygon->SNorZ[iii] = zz[iii]; */
	/*   } */
}

void AddVertsTexture( polygon *thisPolygon, float *u, float *v ) {

	//int      iii;
	int  nnn;

	nnn = thisPolygon->numVertices;
	thisPolygon->txtU = malloc( sizeof(*(thisPolygon->txtU)) * nnn );
	thisPolygon->txtV = malloc( sizeof(*(thisPolygon->txtV)) * nnn );

	memcpy(thisPolygon->txtU, u, nnn * sizeof(*u));
	memcpy(thisPolygon->txtV, v, nnn * sizeof(*v));

	/*   for ( iii=0; iii<nnn; iii++ ) { */
	/*     thisPolygon->txtU[iii] = u[iii]; */
	/*     thisPolygon->txtV[iii] = v[iii]; */
	/*   } */
}

void SetPolygonNormal( polygon *thisPolygon ) {

	float    *px, *py, *pz;
	float     ux,  uy,  uz;
	float     vx,  vy,  vz;
	float     nx,  ny,  nz;
	int       nnn;
	int       ierr;

	vx = vy = vz = 0;

	nnn = thisPolygon->numVertices;
	px  = thisPolygon->coorX;
	py  = thisPolygon->coorY;
	pz  = thisPolygon->coorZ;

	/*   for ( iii = 0; iii < nnn; iii++ ) { */
	/*     i1 = iii+1; if( i1 == nnn ) i1 = 0; */
	/*     vx -= ( pz[i1] + pz[iii] ) * ( py[i1] - py[iii] ); */
	/*     vy -= ( px[i1] + px[iii] ) * ( pz[i1] - pz[iii] ); */
	/*     vz -= ( py[i1] + py[iii] ) * ( px[i1] - px[iii] ); */
	/*   } */

	assert(nnn > 1);

	ux = px[1] - px[0];
	uy = py[1] - py[0];
	uz = pz[1] - pz[0];

	vx = px[2] - px[0];
	vy = py[2] - py[0];
	vz = pz[2] - pz[0];

	ierr = FALSE;
	crossVV(&ierr,
			&nx, &ny, &nz,
			ux, uy, uz,
			vx, vy, vz);

	VectorNormalize( &ierr, &nx, &ny, &nz );

	thisPolygon->PNorX = nx;
	thisPolygon->PNorY = ny;
	thisPolygon->PNorZ = nz;
}

void TranslatePolygon( polygon *thisPolygon, float x, float y, float z ) {

	int i;

	for( i=0; i<thisPolygon->numVertices; i++ ) {
		thisPolygon->coorX[i] += x;
		thisPolygon->coorY[i] += y;
		thisPolygon->coorZ[i] += z;
	}
	thisPolygon->planeD =  - ( thisPolygon->PNorX * thisPolygon->coorX[0] +
							   thisPolygon->PNorY * thisPolygon->coorY[0] +
							   thisPolygon->PNorZ * thisPolygon->coorZ[0]);

}

int GetPolygonSegmentCut( polygon *thisPolygon, float  ax,   float  ay,   float  az,
						  float  bx,   float  by,   float  bz,
						  float *cutX, float *cutY, float *cutZ,
						  float *cutLanda ) {

	// Returns TRUE if segment AB cuts the polygon
	// If they cut, the cutting point is stored in CUT
	//              and landa will be set to the cut position ( 0 at A, 1 at B )

	float    A, B, C, D;
	float    semiSpaceA, semiSpaceB;
	float    landa;

	A = thisPolygon->PNorX;
	B = thisPolygon->PNorY;
	C = thisPolygon->PNorZ;
	D = thisPolygon->planeD;

	// Check whether the segment crosses the polygon plane
	semiSpaceA = A * ax + B * ay + C * az + D;
	semiSpaceB = A * bx + B * by + C * bz + D;
	if( semiSpaceA > 0.0f ) {
		if( semiSpaceB > 0.0f ) return FALSE;
		// this is done to avoid a dividing by zero in the next step
		if( semiSpaceA <  DISTANCE_EPSILON ) return FALSE;
	}
	else {
		if( semiSpaceB < 0.0f ) return FALSE;
		if( semiSpaceA > -DISTANCE_EPSILON ) return FALSE;
	}

	// Check if the segment cuts the polygon´s plane
	// this two equations must be verified by point C
	// n.c + D = 0 and  c = a + ab * landa
	// c is in plane    c is in the line ( landa=0->c=a landa=1->c=b )
	//
	// so,   n.( a + ab * landa ) + D = 0
	// and,  landa = - ( n.a + D  ) / (n.ab)
	//             = -  semiSpaceA  / (n.ab)
	// we have already checked that the denominator will never be zero
	landa = -semiSpaceA / ( A * (bx-ax) + B * (by-ay) + C * (bz-az) );
	// if the cutting point c is outside, then there is no cut
	if( landa < 0.0f || landa > 1.0f ) return FALSE;

	// Check whether C is inside the polygon
	*cutX = ax + (bx-ax) * landa;
	*cutY = ay + (by-ay) * landa;
	*cutZ = az + (bz-az) * landa;
	*cutLanda = landa;
	return GetPointInPolygon( thisPolygon, *cutX, *cutY, *cutZ );
}

int GetPointInPolygon( polygon *thisPolygon, float ax, float ay, float az ) {

	// Assumes that point A is within the plane that contains the polygon
	// Returns TRUE if the point is inside the polygon
	float   *polx, *poly;

	// by default we use projection onto XY plane to solve the point in problem
	// However, if the polygon normal has an small Z component we will use another plane
	polx = thisPolygon->coorX;
	poly = thisPolygon->coorY;
	if( thisPolygon->PNorZ > 0.1f || thisPolygon->PNorZ < -0.1f ) {
		// use XY projection
	}
	else {
		if( thisPolygon->PNorY > 0.1f || thisPolygon->PNorY < -0.1f ) {
			// use XZ projection
			poly = thisPolygon->coorZ;
			ay   = az;
		}
		else {
			// use YZ projection
			polx = thisPolygon->coorZ;
			ax   = az;
		}
	}

	return PointInPolygon2D( thisPolygon->numVertices, polx, poly, ax, ay );
}

void PrintPolygon (polygon *thisPolygon) {

	int i;

	printf("v:%d:", thisPolygon->numVertices);
	for(i=0; i < thisPolygon->numVertices; ++i) {
		printf("(%.2f, %.2f, %.2f) ", thisPolygon->coorX[i], thisPolygon->coorY[i], thisPolygon->coorZ[i]);
	}
	printf("\n");
	if (thisPolygon->SNorX) {
		for(i=0; i < thisPolygon->numVertices; ++i) {
			printf("(%.2f, %.2f, %.2f) ", thisPolygon->SNorX[i], thisPolygon->SNorY[i], thisPolygon->SNorZ[i]);
		}
		printf("\n");
	}
	if (thisPolygon->txtU) {
		for(i=0; i < thisPolygon->numVertices; ++i) {
			printf("(%.2f, %.2f) ", thisPolygon->txtU[i], thisPolygon->txtV[i]);
		}
		printf("\n");
	}
	printf("(%.2f, %.2f, %.2f, %.2f)\n", thisPolygon->PNorX, thisPolygon->PNorY, thisPolygon->PNorZ, thisPolygon->planeD);

}
