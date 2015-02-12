#include <GL/glut.h>

#include "tools.h"
#include "animation.h"

// It is possible to add other controls that animate
// certain parameters from frame to frame
// To add new animations, define similar functions
// in animation.h
//
//	int GetAnimationCamera();
//	int    AnimationCamera();
//	int GetAnimationXXX();
//	int    AnimationXXX();

static void OurAnimation( void ) {

	glutPostRedisplay();
	if( GetAnimationCamera() )	AnimationCamera();
	//	if( GetAnimationXXX() )		AnimationXX();
}

void EvaluateAnimation() {

	if( GetAnimationCamera() //	||
		//	GetAnimationXXX()	||
		//	GetAnimationYYYYY()	||
		) {
		glutIdleFunc( OurAnimation );
	}
	else {
		glutIdleFunc( NULL );
	}
}
