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
// AdvanceAvatar: see if avatar can advance 'step' units.

int AdvanceAvatar(Avatar *theAvatar, float step) {

	camera *thisCamera = theAvatar->cam;
	BSphere *bsph = theAvatar->bsph;
	float newX, newY, newZ;
	Node *rootNode;

	rootNode = RootNodeScene();
	if (theAvatar->walk)
		WalkCamera(thisCamera, step);
	else
		FlyCamera(thisCamera, step);
	return 1;
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
