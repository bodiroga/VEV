#ifndef BSPHERE_H
#define BSPHERE_H

typedef struct _BSphere {

	float    sphereX;
	float    sphereY;
	float    sphereZ;
	float    radius;
} BSphere;

void     DestroyBSphere( BSphere **theBSphere );
// Create a new BSphere located at (x,y,z) and radius 'radius'
BSphere *CreateBSphere( float x, float y, float z, float radius );
// Place an existing BSphere at (x,y,z)
void     PlaceBSphere( BSphere *thisBSphere, float x, float y, float z);

#endif /* BSPHERE_H */
