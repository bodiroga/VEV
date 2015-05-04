#include "tools.h"
#include "scene.h"
#include "avatar.h"

void DestroyAvatar ( Avatar **theAvatar ) {

	Avatar  *thisAvatar = *theAvatar;
	if( ! thisAvatar ) return;

	DestroyBSphere(&thisAvatar->bsph);
	free( thisAvatar );

	*theAvatar = NULL;
}

Avatar *CreateAvatar( camera *theCamera, float radius) {

	Avatar  *newAvatar;
	BSphere *newBSphere;

	newAvatar = malloc( sizeof(*newAvatar) * 1 );
	newBSphere = CreateBSphere( theCamera->Ex,
								theCamera->Ey,
								theCamera->Ez,
								radius);

	newAvatar->cam          = theCamera;
	newAvatar->bsph         = newBSphere;
	newAvatar->walk         = 1;

	return newAvatar;
}

int WalkOrFlyAvatar(Avatar *theAvatar, int walkOrFly) {
	int walk = theAvatar->walk;
	theAvatar->walk = walkOrFly;
	return walk;
}

// TODO:
//
// AdvanceAvatar: advance avatar 'step' units if possible.

int AdvanceAvatar(Avatar *theAvatar, float step) {

	camera *thisCamera = theAvatar->cam;
	BSphere *bsph = theAvatar->bsph;
	trfm3D *transformacion;

	float newX, newY, newZ;
	float oldX, oldY, oldZ;
	float transX, transY, transZ;
	Node *rootNode;
	rootNode = RootNodeScene();

	transformacion = CreateTrfm3D();

	oldX = thisCamera->Ex; newX = thisCamera->Ex;
	oldY = thisCamera->Ey; newY = thisCamera->Ey;
	oldZ = thisCamera->Ez; newZ = thisCamera->Ez;

	transX = step * -thisCamera->Dx;
	if (theAvatar->walk == 0)
		transY = step * -thisCamera->Dy;
	else
		transY = 0.0;
	transZ = step * -thisCamera->Dz;

	SetTransTrfm3D(transformacion, transX, transY, transZ);

	Trfm3DTransformPoint(transformacion, &newX, &newY, &newZ);

	PlaceBSphere(bsph, newX, newY, newZ);

	if (CollisionBSphereNode(rootNode, bsph) == NULL) {
		if (theAvatar->walk == 1) {
			WalkCamera(thisCamera, step);
		} else {
			FlyCamera(thisCamera, step);
		}
		return 1;
	} else {
		PlaceBSphere(bsph, oldX, oldY, oldZ);
	}
	return 0;

}

void LeftRightAvatar(Avatar *theAvatar, float angle) {
	if (theAvatar->walk)
		ViewYWorldCamera(theAvatar->cam, angle);
	else
		YawCamera(theAvatar->cam, angle);
}

void UpDownAvatar(Avatar *theAvatar, float angle) {
	PitchCamera(theAvatar->cam, angle);
}
