#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "camera.h"
#include "tools.h"
#include "intersect.h"
#include "scene.h"

static trfm3D LOCAL_Trfm;
static trfm3D CAM_Trfm;

// Forward references of auxiliarly functions

static void UpdateCameraFrame (camera *thisCamera);
static void UpdateCameraProjection ( camera *thisCamera );
static void UpdateFrustumPlanes(camera * thisCamera);
static void set_view_trfm( camera *thisCamera);
static void init_camera_raw(camera *thisCamera);

void DestroyCamera ( camera **theCamera ) {
	int i;
	camera  *thisCamera = *theCamera;

	if( ! thisCamera ) return;
	if (thisCamera->name)
		free(thisCamera->name);
	for (i=0; i<MAX_CLIP_PLANES; ++i) {
		DestroyPlane(&(thisCamera->fPlanes[i]));
	}
	DestroyTrfm3D(&(thisCamera->viewTrfm));
	DestroyTrfm3D(&(thisCamera->projTrfm));
	free( thisCamera );

	*theCamera = NULL;
}

// Set initial position, view matrix and projection matrix.

static void init_camera_raw(camera *thisCamera) {
	// position
	thisCamera->Ex = 0.0f; thisCamera->Ey = 0.0f; thisCamera->Ez = 0.0f;
	// look at
	thisCamera->AtX = 0.0f; thisCamera->AtY = 0.0f; thisCamera->AtZ = -1.0f;
	// up vector
	thisCamera->UpX = 0.0f; thisCamera->UpY = 1.0f; thisCamera->UpZ = 0.0f;
	// View matrix
	thisCamera->Rx = 1.0f;  thisCamera->Ry = 0.0f;  thisCamera->Rz = 0.0f;
	thisCamera->Ux = 0.0f;  thisCamera->Uy = 1.0f;  thisCamera->Uz = 0.0f;
	thisCamera->Dx = 0.0f;  thisCamera->Dy = 0.0f;  thisCamera->Dz = 1.0f;
	set_view_trfm(thisCamera);
	// Projection
	thisCamera->fovy = 60.0f * DEGREE_TO_RAD;
	thisCamera->near = 0.1f; thisCamera->far = 1500.0f;
	thisCamera->aspectRatio = 1.0f;
	thisCamera->left = -0.034451; thisCamera->bottom = -0.026795;
	thisCamera->right = 0.034451; thisCamera->top = 0.026795;
	SetFrustumTrfm3D(thisCamera->projTrfm,
					 thisCamera->left, thisCamera->right,
					 thisCamera->top, thisCamera->bottom,
					 thisCamera->near, thisCamera->far);
}

camera *CreateOriginCamera() {
	int i;
	camera *thisCamera;
	thisCamera = malloc( sizeof(*thisCamera) * 1 );
	thisCamera->name = NULL;
	thisCamera->fovy = 60.0f * DEGREE_TO_RAD;
	thisCamera->near = 0.1f;
	thisCamera->far = 1500.0f;
	thisCamera->aspectRatio = 1.0f;
	thisCamera->viewTrfm = CreateTrfm3D();
	thisCamera->projTrfm = CreateTrfm3D();
	for(i = 0; i < MAX_CLIP_PLANES; ++i)
		thisCamera->fPlanes[i] = CreateVoidPlane();
	init_camera_raw(thisCamera);
	return thisCamera;
}

void CopyDataCamera( camera *thisCamera, camera *source ) {
	thisCamera->Ex   = source->Ex;
	thisCamera->Ey   = source->Ey;
	thisCamera->Ez   = source->Ez;
	thisCamera->AtX  = source->AtX;
	thisCamera->AtY  = source->AtY;
	thisCamera->AtZ  = source->AtZ;
	thisCamera->UpX  = source->UpX;
	thisCamera->UpY  = source->UpY;
	thisCamera->UpZ  = source->UpZ;
	thisCamera->fovy = source->fovy;
	thisCamera->near = source->near;
	thisCamera->far  = source->far;

	UpdateCameraFrame( thisCamera );
	UpdateCameraProjection( thisCamera );
}

void InitConicCamera( camera *thisCamera, float fovy, float aspect,
					  float near, float far) {
	thisCamera->near = near;
	thisCamera->far = far;
	thisCamera->fovy = fovy;
	thisCamera->aspectRatio = aspect;
	UpdateCameraProjection(thisCamera);
}

// Set Camera parameters
void  SetCamera( camera *thisCamera,
				 float viewX, float viewY, float viewZ,
				 float atX,   float atY,   float atZ,
				 float upX,   float upY,   float upZ ) {

	float    vaX,   vaY,   vaZ;
	float    prodEsc;

	VectorNormalize( &upX, &upY, &upZ );

	thisCamera->Ex    = viewX;
	thisCamera->Ey    = viewY;
	thisCamera->Ez    = viewZ;
	thisCamera->AtX      = atX;
	thisCamera->AtY      = atY;
	thisCamera->AtZ      = atZ;
	thisCamera->UpX      = upX;
	thisCamera->UpY      = upY;
	thisCamera->UpZ      = upZ;

	// View to At vector
	vaX = thisCamera->AtX - thisCamera->Ex;
	vaY = thisCamera->AtY - thisCamera->Ey;
	vaZ = thisCamera->AtZ - thisCamera->Ez;

	if (V_IS_ZERO(vaX, vaY, vaZ)) {//printf("%f,%f,%f\n",vaX,vaY,vaZ);
		fprintf(stderr, "[W] while setting camera (%s): V & A are too near\n", thisCamera->name ? thisCamera->name : "noname" );
		thisCamera->AtX += 1.0f;
		thisCamera->AtY += 1.0f;
		thisCamera->AtZ += 1.0f;
		vaX = thisCamera->AtX - thisCamera->Ex;
		vaY = thisCamera->AtY - thisCamera->Ey;
		vaZ = thisCamera->AtZ - thisCamera->Ez;
	}
	VectorNormalize( &vaX, &vaY, &vaZ );

	// Check if va & up are colinear
	prodEsc = vaX * upX + vaY * upY + vaZ * upZ;
	if( fabs( prodEsc ) > 0.99f ) {
		fprintf(stderr, "[W] while setting camera (%s): up is nearly parallel to VA\n", thisCamera->name ? thisCamera->name : "noname" );
		upX += 1.0f;
		prodEsc = vaX * upX + vaY * upY + vaZ * upZ;
		if( fabs( prodEsc ) > 0.99f ) upY += 1.0;
	}

	UpdateCameraFrame( thisCamera );
}

void SetCameraName(camera *thisCamera, char *cameraName) {

	if (thisCamera->name) {
		free(thisCamera->name);
	}
	thisCamera->name = strdup(cameraName);
}

////////////////////////////////////////////////////////////7
// Projection stuff


// @@ TODO:
// * Given (near, far, aspectRatio, fovy ), calculate the values of
//   frustum (top, bottom, right, left).
// * Also, update projection matrix (projTrfm)

static void UpdateCameraProjection ( camera *thisCamera ) {

	thisCamera->top = thisCamera->near*tan(thisCamera->fovy/2);
	thisCamera->bottom = -thisCamera->top;
	thisCamera->right = thisCamera->aspectRatio * thisCamera->top;
	thisCamera->left = -thisCamera->right;

	SetFrustumTrfm3D(thisCamera->projTrfm, thisCamera->left, thisCamera->right, thisCamera->top,
			thisCamera->bottom, thisCamera->near, thisCamera->far);

	UpdateFrustumPlanes(thisCamera);
}

// get position of the camera
void GetCameraPosition(camera *thisCamera,
					   float *posX, float *posY, float *posZ) {
	*posX = thisCamera->Ex;
	*posY = thisCamera->Ey;
	*posZ = thisCamera->Ez;
}


// change camera aspect ratio

void SetCameraAspectRatio( camera *thisCamera, int w, int h ) {

	if( h > 0 )
		thisCamera->aspectRatio = (float) w / (float) h;    // width/height
	else
		thisCamera->aspectRatio = 1.0f;

	UpdateCameraProjection( thisCamera );
}

// change camera fovy

void SetCameraFovy(camera *thisCamera, float angle) {
	thisCamera->fovy=angle;
	UpdateCameraProjection( thisCamera );
}

float SetCameraFar(camera *thisCamera, float far) {
	float aux = thisCamera->far;
	thisCamera->far = far;
	UpdateCameraProjection( thisCamera );
	return aux;
}

float GetCameraFar(camera *thisCamera) {
	return thisCamera->far;
}

float SetCameraNear(camera *thisCamera, float near) {
	float aux = thisCamera->near;
	thisCamera->near = near;
	UpdateCameraProjection( thisCamera );
	return aux;
}

float GetCameraNear(camera *thisCamera) {
	return thisCamera->near;
}


// Get Projection transformation

trfm3D *CameraProjectionTrfm(camera *thisCamera) {
	return thisCamera->projTrfm;
}

// Get Projection openGL matrix

void CameraOpenGLProjection(camera *thisCamera, float *gmatrix) {
	GetGLMatrixTrfm3D(thisCamera->projTrfm, gmatrix);
}

// Get view openGL matrix

void CameraOpenGLView(camera *thisCamera, float *gmatrix) {
	GetGLMatrixTrfm3D(thisCamera->viewTrfm, gmatrix);
}

//////////////////////////////////////////////////////////////////////////
// Camera rotation / movement

static void set_view_trfm( camera *thisCamera) {
	SetWorld2LocalTrfm3D(thisCamera->viewTrfm,
						 thisCamera->Ex, thisCamera->Ey, thisCamera->Ez,
						 thisCamera->Rx, thisCamera->Ry, thisCamera->Rz,
						 thisCamera->Ux, thisCamera->Uy, thisCamera->Uz,
						 thisCamera->Dx, thisCamera->Dy, thisCamera->Dz);

	UpdateFrustumPlanes(thisCamera);
}

// @@ TODO:
/**
 * Calculate (U, R, D) vectors of the camera frame given (E, At, Up)
 *
 * @param thisCamera the camera.
 */

static void UpdateCameraFrame (camera *thisCamera) {

	float Fx, Fy, Fz;
	float Up_x_aux, Up_y_aux, Up_z_aux;

	Fx = thisCamera->Ex - thisCamera->AtX;
	Fy = thisCamera->Ey - thisCamera->AtY;
	Fz = thisCamera->Ez - thisCamera->AtZ;

	VectorNormalize(&Fx, &Fy, &Fz);

	Up_x_aux = thisCamera->UpX;
	Up_y_aux = thisCamera->UpY;
	Up_z_aux = thisCamera->UpZ;

	VectorNormalize(&Up_x_aux, &Up_y_aux, &Up_z_aux);

	thisCamera->Rx = Up_y_aux*Fz - Up_z_aux*Fy;
	thisCamera->Ry = Up_z_aux*Fx - Up_x_aux*Fz;
	thisCamera->Rz = Up_x_aux*Fy - Up_y_aux*Fx;

	thisCamera->Ux = Fy*thisCamera->Rz - Fz*thisCamera->Ry;
	thisCamera->Uy = Fz*thisCamera->Rx - Fx*thisCamera->Rz;
	thisCamera->Uz = Fx*thisCamera->Ry - Fy*thisCamera->Rx;

	thisCamera->Dx = Fx;
	thisCamera->Dy = Fy;
	thisCamera->Dz = Fz;

	set_view_trfm(thisCamera);
}

trfm3D *ViewTrfmCamera( camera *thisCamera ) {
	return thisCamera->viewTrfm;
}


// @@ TODO:
// Move the camera "step" units ahead. Fly mode.
//
// thisCamera         -> the camera
// step               -> number of units to fly (can be negative)

void FlyCamera(camera *thisCamera, float step) {

	trfm3D *transformacion;

	float tx = step * -thisCamera->Dx;
	float ty = step * -thisCamera->Dy;
	float tz = step * -thisCamera->Dz;

	transformacion = CreateTrfm3D();

	SetTransTrfm3D(transformacion, tx, ty, tz);

	Trfm3DTransformPoint(transformacion, &thisCamera->Ex, &thisCamera->Ey, &thisCamera->Ez);
	Trfm3DTransformPoint(transformacion, &thisCamera->AtX, &thisCamera->AtY, &thisCamera->AtZ);

	set_view_trfm(thisCamera);

}

// @@ TODO:
// Move the camera "step" units ahead. Walk mode.
//
// thisCamera         -> the camera
// step               -> number of units to walk (can be negative)

void WalkCamera(camera *thisCamera, float step) {

	trfm3D *transformacion;

	float tx = step * -thisCamera->Dx;
	float ty = 0;	// Ponemos un cero para que no se traslade en la componente y
	float tz = step * -thisCamera->Dz;

	transformacion = CreateTrfm3D();

	SetTransTrfm3D(transformacion, tx, ty, tz);

	Trfm3DTransformPoint(transformacion, &thisCamera->Ex, &thisCamera->Ey, &thisCamera->Ez);
	Trfm3DTransformPoint(transformacion, &thisCamera->AtX, &thisCamera->AtY, &thisCamera->AtZ);

	set_view_trfm(thisCamera);
}

void  PanXCamera( camera *thisCamera, float step ) {
	thisCamera->Ex = thisCamera->Ex + step*thisCamera->Rx;
	thisCamera->Ey = thisCamera->Ey + step*thisCamera->Ry;
	thisCamera->Ez = thisCamera->Ez + step*thisCamera->Rz;

	thisCamera->AtX = thisCamera->AtX + step*thisCamera->Rx;
	thisCamera->AtY = thisCamera->AtY + step*thisCamera->Ry;
	thisCamera->AtZ = thisCamera->AtZ + step*thisCamera->Rz;
	set_view_trfm(thisCamera);

}

void  PanYCamera( camera *thisCamera, float step ) {
	thisCamera->Ex = thisCamera->Ex + step*thisCamera->Ux;
	thisCamera->Ey = thisCamera->Ey + step*thisCamera->Uy;
	thisCamera->Ez = thisCamera->Ez + step*thisCamera->Uz;

	thisCamera->AtX = thisCamera->AtX + step*thisCamera->Ux;
	thisCamera->AtY = thisCamera->AtY + step*thisCamera->Uy;
	thisCamera->AtZ = thisCamera->AtZ + step*thisCamera->Uz;
	set_view_trfm(thisCamera);

}

void  PitchCamera( camera *thisCamera, float angle ) {

	SetRotAxisTrfm3D(&LOCAL_Trfm,
					 thisCamera->Rx, thisCamera->Ry, thisCamera->Rz,
					 thisCamera->Ex, thisCamera->Ey, thisCamera->Ez,
					 angle);
	Trfm3DTransformPoint(&LOCAL_Trfm,
						 &thisCamera->AtX, &thisCamera->AtY, &thisCamera->AtZ );
	TransformVectorTrfm3D(&LOCAL_Trfm,
						  &thisCamera->UpX, &thisCamera->UpY, &thisCamera->UpZ );
	UpdateCameraFrame(thisCamera);
}

void  YawCamera( camera *thisCamera, float angle ) {

	SetRotAxisTrfm3D(&LOCAL_Trfm,
					 thisCamera->Ux, thisCamera->Uy, thisCamera->Uz,
					 thisCamera->Ex, thisCamera->Ey, thisCamera->Ez,
					 angle);
	Trfm3DTransformPoint(&LOCAL_Trfm,
						 &thisCamera->AtX, &thisCamera->AtY, &thisCamera->AtZ );
	TransformVectorTrfm3D(&LOCAL_Trfm,
						  &thisCamera->UpX, &thisCamera->UpY, &thisCamera->UpZ );
	UpdateCameraFrame(thisCamera);
}

void  RollCamera( camera *thisCamera, float angle ) {

	SetRotAxisTrfm3D(&LOCAL_Trfm,
					 thisCamera->Dx, thisCamera->Dy, thisCamera->Dz,
					 thisCamera->Ex, thisCamera->Ey, thisCamera->Ez,
					 -angle);
	Trfm3DTransformPoint(&LOCAL_Trfm,
						 &thisCamera->AtX, &thisCamera->AtY, &thisCamera->AtZ );
	TransformVectorTrfm3D(&LOCAL_Trfm,
						  &thisCamera->UpX, &thisCamera->UpY, &thisCamera->UpZ );
	UpdateCameraFrame(thisCamera);
}

void  ArvOverUnderCamera( camera *thisCamera, float angle ) {
	SetRotAxisTrfm3D(&LOCAL_Trfm,
					 thisCamera->Rx, thisCamera->Ry, thisCamera->Rz,
					 thisCamera->AtX, thisCamera->AtY, thisCamera->AtZ,
					 angle);
	Trfm3DTransformPoint(&LOCAL_Trfm,
						 &thisCamera->Ex, &thisCamera->Ey, &thisCamera->Ez );
	TransformVectorTrfm3D(&LOCAL_Trfm,
						  &thisCamera->UpX, &thisCamera->UpY, &thisCamera->UpZ );
	UpdateCameraFrame(thisCamera);
}

void  ArcLeftRightCamera( camera *thisCamera, float angle ) {
	SetRotAxisTrfm3D(&LOCAL_Trfm,
					 thisCamera->Ux, thisCamera->Uy, thisCamera->Uz,
					 thisCamera->AtX, thisCamera->AtY, thisCamera->AtZ,
					 angle);
	Trfm3DTransformPoint(&LOCAL_Trfm,
						 &thisCamera->Ex, &thisCamera->Ey, &thisCamera->Ez );
	TransformVectorTrfm3D(&LOCAL_Trfm,
						  &thisCamera->UpX, &thisCamera->UpY, &thisCamera->UpZ );
	UpdateCameraFrame(thisCamera);
}

// Rotate camera around world's Y axis

void ViewYWorldCamera( camera *thisCamera, float angle ) {
	SetRotAxisTrfm3D(&LOCAL_Trfm,
					 0.0, 1.0, 0.0,
					 thisCamera->Ex, thisCamera->Ey, thisCamera->Ez,
					 angle);
	Trfm3DTransformPoint(&LOCAL_Trfm,
						 &thisCamera->AtX, &thisCamera->AtY, &thisCamera->AtZ );
	TransformVectorTrfm3D(&LOCAL_Trfm,
						  &thisCamera->UpX, &thisCamera->UpY, &thisCamera->UpZ );
	UpdateCameraFrame(thisCamera);
}

// @@ TODO: Frustum culling (look at camera.h for parameter descriptions and return
//          values)

int FrustumCull(camera *thisCamera,
				BBox *theBBox, unsigned int *planesBitM) {


	int hay_interseccion = 0;
	int dentro = 1;
	int plano;

	for(plano = 0; plano < 6; plano++) {
		if (BBoxPlaneIntersect(theBBox, thisCamera->fPlanes[plano]) == IINTERSECT)
			hay_interseccion = 1;
		else if (BBoxPlaneIntersect(theBBox, thisCamera->fPlanes[plano]) == IREJECT)
			dentro = 0;
	}

	if (dentro && hay_interseccion) {
		return 0;
	} else if (dentro && !hay_interseccion) {
		return 1;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////////////
// Hemendik aurrera ez

void SetCamTrfm(camera *thisCamera, float px, float py,float pz) {

	CAM_Trfm.r1X = thisCamera->Rx;
	CAM_Trfm.r1Y = thisCamera->Ry;
	CAM_Trfm.r1Z = thisCamera->Rz;
	CAM_Trfm.r2X = thisCamera->Ux;
	CAM_Trfm.r2Y = thisCamera->Uy;
	CAM_Trfm.r2Z = thisCamera->Uz;
	CAM_Trfm.r3X = thisCamera->Dx;
	CAM_Trfm.r3Y = thisCamera->Dy;
	CAM_Trfm.r3Z = thisCamera->Dz;
	CAM_Trfm.scl = 1.;
	CAM_Trfm.trX = px;
	CAM_Trfm.trY = py;
	CAM_Trfm.trZ = pz;
}

void GetCamTrfm(trfm3D *L_Trfm, camera *thisCamera, float *px, float *py,float *pz) {

	thisCamera->Rx=L_Trfm->r1X ;
	thisCamera->Ry=L_Trfm->r1Y ;
	thisCamera->Rz=L_Trfm->r1Z ;

	thisCamera->Ux=L_Trfm->r2X ;
	thisCamera->Uy=L_Trfm->r2Y ;
	thisCamera->Uz=L_Trfm->r2Z ;
	thisCamera->UpX=L_Trfm->r2X ;
	thisCamera->UpY=L_Trfm->r2Y ;
	thisCamera->UpZ=L_Trfm->r2Z ;

	thisCamera->Dx=L_Trfm->r3X ;
	thisCamera->Dy=L_Trfm->r3Y ;
	thisCamera->Dz=L_Trfm->r3Z ;

	*px = L_Trfm->trX ;
	*py = L_Trfm->trY;
	*pz = L_Trfm->trZ;
}

static void UpdateFrustumPlanes(camera * thisCamera) {

	static GLfloat M[16];
	static trfm3D T;
	plane *p;

	// V: view transformation
	// P: perspective transformation
	// T = P*V

	SetCopyTrfm3D(&T, thisCamera->projTrfm); // T = P;
	CompositionTrfm3D(&T, thisCamera->viewTrfm); // T = P*V
	GetGLMatrixTrfm3D(&T, M);

	// Extract the planes parameters

	// left plane
	p = thisCamera->fPlanes[0];

	p->nx = -M[3]  - M[0];  // -(m_41 + m_11)
	p->ny = -M[7]  - M[4];  // -(m_42 + m_12)
	p->nz = -M[11] - M[8];  // -(m_43 + m_13)
	p->d =   M[15] + M[12]; //  (m_44 + m_14), because d in plane is really (-d)
	p->is_norm = 0;
	p = thisCamera->fPlanes[1];

	// right plane

	p->nx = -M[3]  + M[0];  // -(m_41 - m_11)
	p->ny = -M[7]  + M[4];  // -(m_42 - m_12)
	p->nz = -M[11] + M[8];  // -(m_43 - m_13)
	p->d =   M[15] - M[12]; //  (m_44 - m_14) because d in plane is really (-d)
	p->is_norm = 0;
	p = thisCamera->fPlanes[2];

	// bottom plane

	p->nx = -M[3]  - M[1];  // -(m_41 + m_21)
	p->ny = -M[7]  - M[5];  // -(m_42 + m_22)
	p->nz = -M[11] - M[9];  // -(m_43 + m_23)
	p->d =   M[15] + M[13]; //  (m_44 + m_24) because d in plane is really (-d)
	p->is_norm = 0;
	p = thisCamera->fPlanes[3];

	// top plane

	p->nx = -M[3]  + M[1];   // -(m_41 - m_21)
	p->ny = -M[7]  + M[5];   // -(m_42 - m_22)
	p->nz = -M[11] + M[9];   // -(m_43 - m_23)
	p->d =   M[15] - M[13];  //  (m_44 - m_24) because d in plane is really (-d)
	p->is_norm = 0;
	p = thisCamera->fPlanes[4];

	// near plane

	p->nx = -M[3]  - M[2];  // -(m_41 + m_31)
	p->ny = -M[7]  - M[6];  // -(m_42 + m_32)
	p->nz = -M[11] - M[10]; // -(m_43 + m_33)
	p->d =   M[15] + M[14]; //  (m_44 + m_34) because d in plane is really (-d)
	p->is_norm = 0;
	p = thisCamera->fPlanes[5];

	// far plane

	p->nx = -M[3]  + M[2];  // -(m_41 - m_31)
	p->ny = -M[7]  + M[6];  // -(m_42 - m_32)
	p->nz = -M[11] + M[10]; // -(m_43 - m_33)
	p->d =   M[15] - M[14]; //  (m_44 - m_34) because d in plane is really (-d)
	p->is_norm = 0;
	// It is not neccesary to normailze the planes for frustum calculation
}

void  DrawGLAtFeedbackCamera( camera *thisCamera, float factor ) {

	//
	//                 A  At point
	//               l . l
	//              l  .  l
	//             l   .   l
	//        X2  x    M    x  X1
	//                 .
	//                 .
	//                 .
	//                 .
	//                 .
	//                 .
	//  ---------------V--------------------->
	//                 |                      Xcam
	//                 |
	//                 |
	//                 |
	//                 v
	//                   Zcam
	//
	int		 lightingFlag;
	GLfloat  oldwidth;
	float	 atX, atY, atZ;
	float	 mX,  mY,  mZ;	// point M
	float	 iiX, iiY, iiZ;	// vector parallel to camI, from M to X1
	float	 jjX, jjY, jjZ; // vector parallel to camJ
	float	 pX,  pY,  pZ;	// point
	float	 mod;

	glGetFloatv(GL_LINE_WIDTH, &oldwidth);
	lightingFlag = glIsEnabled( GL_LIGHTING );
	if( lightingFlag ) glDisable( GL_LIGHTING );

	atX = thisCamera->AtX;
	atY = thisCamera->AtY;
	atZ = thisCamera->AtZ;

	/* mod = GetVALength( thisCamera ); */
	/* if( mod < 4.9999f ) { */
	/*	glColor3f( 1.0f, 0.0f, 0.0f ); */
	/*	glLineWidth( 2.0f ); */
	/* } */
	/* else if( mod <  25.0f ) { */
	/*	glColor3f( 1.0f, 1.0f, 0.0f ); */
	/*	//glLineWidth( 1.0f ); */
	/* } */
	/* else { */
	/*	glColor3f( 1.0f, 1.0f, 1.0f ); */
	/*	glLineWidth( 2.0f ); */
	/* } */

	mod = 1;
	mod = mod * 0.7f;

	mX = thisCamera->Ex + ( - thisCamera->Dx ) * mod;
	mY = thisCamera->Ey + ( - thisCamera->Dy ) * mod;
	mZ = thisCamera->Ez + ( - thisCamera->Dz ) * mod;

	mod = mod * 0.02f * factor;
	iiX = thisCamera->Rx * mod;
	iiY = thisCamera->Ry * mod;
	iiZ = thisCamera->Rz * mod;

	jjX = thisCamera->Ux * mod;
	jjY = thisCamera->Uy * mod;
	jjZ = thisCamera->Uz * mod;


	glBegin( GL_LINE_STRIP );

	pX = mX + iiX + jjX;
	pY = mY + iiY + jjY;
	pZ = mZ + iiZ + jjZ;
	glVertex3f( pX, pY, pZ );

	glVertex3f( atX, atY, atZ );

	pX = mX - iiX + jjX;
	pY = mY - iiY + jjY;
	pZ = mZ - iiZ + jjZ;
	glVertex3f( pX, pY, pZ );

	glVertex3f( atX, atY, atZ );

	pX = mX + iiX - jjX;
	pY = mY + iiY - jjY;
	pZ = mZ + iiZ - jjZ;
	glVertex3f( pX, pY, pZ );

	glVertex3f( atX, atY, atZ );

	pX = mX - iiX - jjX;
	pY = mY - iiY - jjY;
	pZ = mZ - iiZ - jjZ;
	glVertex3f( pX, pY, pZ );

	glEnd();

	glLineWidth( oldwidth );

	if( lightingFlag ) glEnable( GL_LIGHTING );
}

void  PrintCamera( camera *thisCamera ) {

	printf("*** Camera: %s\n", thisCamera->name ? thisCamera->name : "(noname)");
	printf("View Point    : %f %f %f\n", thisCamera->Ex, thisCamera->Ey, thisCamera->Ez );
	printf("Look At Point : %f %f %f\n", thisCamera->AtX,   thisCamera->AtY,   thisCamera->AtZ );
	printf("Up Vector     : %f %f %f\n", thisCamera->UpX,   thisCamera->UpY,   thisCamera->UpZ );
	printf("Fovy %f Near %f Far %f\n",  thisCamera->fovy * RAD_TO_DEGREE,
		   thisCamera->near, thisCamera->far );
	printf("Frame\n");
	printf("R : %f %f %f\n", thisCamera->Rx, thisCamera->Ry, thisCamera->Rz );
	printf("U : %f %f %f\n", thisCamera->Ux, thisCamera->Uy, thisCamera->Uz );
	printf("D : %f %f %f\n", thisCamera->Dx, thisCamera->Dy, thisCamera->Dz );
	printf("Ortho x,y,z min  %f %f %f\n", thisCamera->left, thisCamera->bottom, thisCamera->near);
	printf("      x,y,z max  %f %f %f\n", thisCamera->right, thisCamera->top, thisCamera->far);
}
