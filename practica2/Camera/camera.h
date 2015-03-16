#ifndef CAMERA_H
#define CAMERA_H

#define AGA_CAM_PARALLEL  1
#define AGA_CAM_CONIC     2

#define MAX_CLIP_PLANES   6

#include "plane.h"
#include "bbox.h"
#include "trfm3D.h"

/**

 * we consider a rigth handed reference system for the camera

 - V point where the camera is placed (world coordinates)
 - A  point the camera is looking at  (world coordinates)
 - up vector : unit vector, perpendicular to AV (world componnents)
 - origin camera reference system : at V
 - D camera : defined by vector from A to V (penetrates the viewer's eye)
 - U camera : defined by up vector
 - R camera : looking from V towards A goes rigthwards
*/


typedef struct _Camera {

	char     *name; //!< Name of the Camera

	/** @name View transformation
	 */

	float    Ex; //!< View point X (default 0)
	float    Ey; //!< View point Y (default 0)
	float    Ez; //!< View point Z (default 0)

	float    AtX; //!< look At point X (default 0)
	float    AtY; //!< look At point Y (default 0)
	float    AtZ; //!< look At point Z (default -1)

	float    UpX; //!< Up vector X (default 0)
	float    UpY; //!< Up vector Y (default 1)
	float    UpZ; //!< Up vector Z (default 0)

	/** @name Projection
	 */

	float    fovy; //!< field of view radians in U direction
	float    aspectRatio; //!< window system dependent

	/**
	   defined as they are used by OpenGL

	   always => positive ; Far > Near (distance from plane to camera origin)
	*/
	float    near;
	/**
	   defined as they are used by OpenGL

	   always => positive ; Far > Near (distance from plane to camera origin)
	*/
	float    far;

	/** @name Dependent values
	 */

	/** @name Frustum planes parameters (together with cam_Near and cam_Far)
		Camera has 6 frustum planes, stored in the fPlanes list. The planes
		point INSIDE the frustum. Order is:

		left, right, bottom, top, near, far

	*/

	//@{
	float left, right, bottom, top; //!< Together with far and near, values from the frustum points

	plane *fPlanes[MAX_CLIP_PLANES]; // Frustum planes. A 6 elements of type plane. Order: (l,r,b,t,n,f)
	// Note: normals point inside the frustum.
	//@}

	//@{
	float  Rx, Ry, Rz; //!< camera right vector (in world coordinates)
	float  Ux, Uy, Uz; //!< camera left vector (in world coordinates)
	float  Dx, Dy, Dz; //!< camera back vector (in world coordinates)
	//@}
	trfm3D *viewTrfm; // the View transformation
	trfm3D *projTrfm; // projection transformation

} camera;

void    DestroyCamera ( camera **theCamera );

/**
 * Create a camera with these parameters:
 *
 * - located at (0, 0, 0)
 * - look at (0, 0, -1)
 * - up vector (0, 1, 0)
 * - perspective projection
 * - aperture (fovy) = 60 degrees
 * - aspect ratio = 1
 *
 *
 * @return the new Camera
 */
camera *CreateOriginCamera();

/**
 * Set perspective parameters of a camera
 *
 * @param thisCamera the Camera to be changed
 * @param fovy field of view in the y direction
 * @param aspect aspect ratio (widht / heigth)
 * @param near distance of the near plane
 * @param far distance of the far plane
 */
void InitConicCamera( camera *thisCamera, float fovy, float aspect, float near, float far); //!< Set perspective camera

/**
   Set the viewing matrix derived from a camera at position "view", looking
   at point "at" and whose up vector is "up" (similar as the gluLookAt function)
   \note the function internally normalizes the vector "up"

*/

void SetCamera( camera *thisCamera,
				float viewX, float viewY, float viewZ,
				float atX,   float atY,   float atZ,
				float upX,   float upY,   float upZ );

/**
 * Set Camera Name
 *
 * @param thisCamera the camera
 * @param cameraName the name
 */
void SetCameraName(camera *thisCamera, char *cameraName);

void  CopyDataCamera( camera *thisCamera, camera *source );

/**
 * Change camera's aspect ratio. Call this function whenever the viewport
 * changes (in GLUT glutReshapeFunc function)
 *
 * @param thisCamera pointer to the camera
 * @param w the new width
 * @param h the new height
 */
void SetCameraAspectRatio( camera *thisCamera, int w, int h );

/**
 * Change camera's aperture (fovy)
 *
 * @param thisCamera pointer to the camera
 * @param angle new aperture
 */
void SetCameraFovy(camera *thisCamera, float angle);

float SetCameraFar(camera *thisCamera, float far);    //!< Set far plane distance (positive). Return old value;
float GetCameraFar(camera *thisCamera);               //!< Get far plane distance (positive)

float SetCameraNear(camera *thisCamera, float near);  //!< Set near plane distance (positive). Return old value.
float GetCameraNear(camera *thisCamera);			  //!< Get near plane distance (positive)

/**
 * Get the position of the camera. Leave the output in (posX, posY, posZ)
 *
 * @param thisCamera the camera
 * @param posX pointer to the X coord
 * @param posY pointer to the Y coord
 * @param posZ pointer to the X coodr
 */

void GetCameraPosition(camera *thisCamera,
					   float *posX, float *posY, float *posZ);

/** @name OpenGL related functions
 */
//@{

/**
 * Return the View transformation of the Camera
 *
 * @param thisCamera a camera
 * @return a trfm3D with the View transformation
 */
trfm3D *ViewTrfmCamera( camera *thisCamera );

/**
 * Get the trfm with the camera projection
 *
 * @param thisCamera a camera
 * @return a trfm3D with the projection transformation
 */
trfm3D *CameraProjectionTrfm(camera *thisCamera);

/**
 * Get the projection OpenGL matrix for the camera
 *
 * @param thisCamera a camera
 * @param gmatrix a 16 valued OpenGL matrix with for projection.
 */

void CameraOpenGLProjection(camera *thisCamera, float *gmatrix);

/**
 * Get the view OpenGL matrix for the camera
 *
 * @param thisCamera a camera
 * @param gmatrix a 16 valued OpenGL matrix with for projection.
 */

void CameraOpenGLView(camera *thisCamera, float *gmatrix);

//@}

void  PrintCamera( camera *thisCamera );

//@{@name Functions: Movement functions

/**
 * Move the camera "step" units. Fly mode.
 *
 * @param thisCamera the camera
 * @param step number of units to fly
 * @note step admits negative values
 */

void FlyCamera(camera *thisCamera, float step);

/**
 * Move the camera "step" units. Walk mode.
 *
 * @param thisCamera the camera
 * @param step number of units to walk
 * @note step admits negative values
 */
void WalkCamera(camera *thisCamera, float step);

/*!
  Free camera advances "step" following vector -D

  \note step admits negative values
*/

void  AvanceCameraLocal( camera *thisCamera, float step );

/*!
  Pan camera left/right

  \note step admits negative values
*/

void  PanXCamera( camera *thisCamera, float step );

/*!
  Pan camera up/down

  \note step admits negative values
*/

void  PanYCamera( camera *thisCamera, float step );
//@}

/** @name Functions: Rotation functions
 */
//@{
//
void  PitchCamera( camera *thisCamera, float angle );           //!< local axis R camera
void  YawCamera( camera *thisCamera, float angle );             //!< local axis U camera
void  RollCamera( camera *thisCamera, float angle );            //!< local axis D camera

void  ArvOverUnderCamera( camera *thisCamera, float angle );
void  ArcLeftRightCamera( camera *thisCamera, float angle );

/*!
  Rotate camera around world Y axis. Use this instead of "YawCamera" when walking.
*/
void  ViewYWorldCamera( camera *thisCamera, float angle );      //!< parallel world Y  passes ViewPoint

/**
 *  Check wether frustum intersects a bounding box.
 *
 * @param thisCamera the Camera.
 * @param theBBox the Bounding Box (in world coordinates).
 * @param planesBitM points to a bitmask where bit i is set if BBOX fully inside the i-th frustum plane.
 *
 * @return
 *     -1 BBOX fully inside
 *     0  BBOX intersects frustum
 *     +1 BBOX fully outside frustum
 */

int FrustumCull(camera *thisCamera,
				BBox *theBBox, unsigned int *planesBitM);


// Hemendik aurrera ez !

void  AtYWorldCamera( camera *thisCamera, float angle );        //!< parallel world Y  passes LookAtPoint
void  AtXZWorldCamera( camera *thisCamera, float angle );       //!< parallel world XZ passes LookAtPoint
//@}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
void  DrawGLAtFeedbackCamera( camera *thisCamera, float factor );

/*!
  Given  one point in camera coordinates
  \return the point in world coordinates */
void  CameraToWorld (camera *thisCamera, float    camX, float    camY, float    camZ,
					 float *worldX, float *worldY, float *worldZ );

/*!
  Control < 0 View moves away from At

  control fraction 0.1    use flagFraction = TRUE

  control step            use flagFraction = FALSE */

void  AvanceViewToAtCamera( camera *thisCamera, float control, int flagFraction );

/*!
  ViewPoint and LookAtPoint fixed,

  set a new Up using View Point, the LookAt Point and the given vector
*/

void  SetUpVectorCamera( camera *thisCamera, float upX, float upY, float upZ );

//! See comments in source file camera.c
void  SetStandarPositionCamera( camera *thisCamera, float xMin, float xMax,
								float yMin, float yMax, float zMin, float zMax, int option );

//! Displace (translate) camera moving the ViewPoint to the given point
void  DisplaceCamera( camera *thisCamera, float transX, float transY, float transZ );

//! Translate camera using the given translation vector
void  TranslateCamera( camera *thisCamera, float vecX, float vecY, float vecZ );


//! ViewPoint fixed, set a new Look At point
void  SetLookAtPointCamera( camera *thisCamera, float atX, float atY, float atZ );

//! ViewPoint fixed, orient camera following a given vector
void  OrientCamera( camera *thisCamera, float vecX, float vecY, float vecZ );
void  SetStandardOrientationCamera( camera *thisCamera, int option );
void  SetHorizontalCamera( camera *thisCamera );

void  SetVALength( camera *thisCamera, float length );
float GetVALength( camera *thisCamera );

//! Draw a graphic repr. of the camera axis
void  DrawGLAtFeedbackCamera( camera *thisCamera, float factor );

void  aztertuAxisX( camera *thisCamera, float angle );
void  aztertuAxisY( camera *thisCamera, float angle );

#endif /* CAMERA_H */
