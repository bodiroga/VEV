#include "bsphere.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#ifndef TRUE
#define TRUE   1
#define FALSE  0
#endif

void     DestroyBSphere( BSphere **theBSphere ) {
	BSphere *thisBSphere = *theBSphere;
	if( ! thisBSphere ) return;
	free( thisBSphere );
	*theBSphere = NULL;
}

BSphere *CreateBSphere( float x, float y, float z, float radius ) {
	BSphere *newBSphere;
	newBSphere = malloc( sizeof(*newBSphere) * 1 );
	newBSphere->radius  = radius;
	newBSphere->sphereX = x;
	newBSphere->sphereY = y;
	newBSphere->sphereZ = z;
	return newBSphere;
}

void  PlaceBSphere( BSphere *thisBSphere, float x, float y, float z) {
	thisBSphere->sphereX = x;
	thisBSphere->sphereY = y;
	thisBSphere->sphereZ = z;
}
