#ifndef TOOLS_H
#define TOOLS_H

/*! \file tools.h

  Auxiliary functions.

  Miscellaneous functions dealing with points, vectors, random numbers etc.

  \note Angle units will be used in the following way
  - angle values will be always stored as radians
  - fovy variables (field of view) will be stored as degrees (exception to the rule)
  - angle constants will be written in degrees
  - the user interface will use degrees, but before storing those values will
  be converted into radians
  - Conversions among radians and degrees will be carried out
  using constants stored in the following parameters (see tools.h) :
  -       angle * DEGREE_TO_RAD transforms from degrees into radians
  -       angle * RAD_TO_DEGREE transforms from radians into degrees
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <float.h>

#ifndef TRUE
#define TRUE   1
#define FALSE  0
#endif

// posible selected status
#define  STATUS_SELECTED        10
#define  STATUS_UNSELECTED      11
#define  STATUS_HIT_SELECTED    12
#define  STATUS_HIT_UNSELECTED  13

// input / output
#define MAX_LINE      256
#define MAX_FILE_NAME 128

#define SP_ASCII       32

// Check openGL error

#define ASSERT_OPENGL { GLenum errorCode = glGetError();				\
		if (errorCode != GL_NO_ERROR) {									\
			fprintf (stderr, "[E] %s, line %d. Error: %s\n", __FUNCTION__, __LINE__, gluErrorString(errorCode)); \
			exit(EXIT_FAILURE);}}


// *** Mathematics
#define VECTOR_EPSILON    0.0001f
#define DISTANCE_EPSILON  1e-05f
#define ANGLE_EPSILON     0.00872665f  //!< 0.5 degrees

#define DOT_PRODUCT(Ax, Ay, Az, Bx, By, Bz)(float)((Ax)*(Bx)+(Ay)*(By)+(Az)*(Bz))
#define DIST_IS_ZERO(D) (((D) > - DISTANCE_EPSILON) && ((D) < DISTANCE_EPSILON))
/*#define V_IS_ZERO(A,B,C) (((A)<VECTOR_EPSILON || -1*(A)> -VECTOR_EPSILON ) && \
  ((B)<VECTOR_EPSILON || -1*(B)> -VECTOR_EPSILON ) && \
  ((C)<VECTOR_EPSILON || -1*(C)> -VECTOR_EPSILON ))
*/
#define V_IS_ZERO(A,B,C) (((((A)>0.0f)?(A):-1.0f * (A))<VECTOR_EPSILON)&&((((B)>0.0f)?(B):-1.0f * (B))<VECTOR_EPSILON)&&((((C)>00.f)?(C):-1.0f*(C))<VECTOR_EPSILON))

#define PI_VALUE           3.14159265359f
#define DEGREE_TO_RAD      0.0174533f //!< 2.0 * 3.1415927 / 360.0
#define RAD_TO_DEGREE      57.2958f    //!< 360 / ( 2.0 * 3.1415927 ) */

#define AGA_MOD(A,B,C) (float) sqrt( (A)*(A) + (B)*(B) + (C)*(C) )

#define AGA_MOD_INV(A,B,C) (float) (1.0f / sqrt( (A)*(A) + (B)*(B) + (C)*(C) ))

//! Normalize the vector
/*!
  \param vX, vY, vZ point to the X,Y,Z coords. of the vector
  \return the module of the unormalized vector (zero if error)
  \result
  (*vX, *vY, *vZ) = normalized vector

*/

float VectorNormalize( float *vX, float *vY, float *vZ );

/**
 * Make the vector sum one
 *
 * @param vX, vY, vZ point to the X,Y,Z coords. of the vector
 @return the sum of the old vector
 \result
 (*vX, *vY, *vZ) = sum to one
*/

float VectorSumOne( float *vX, float *vY, float *vZ );

//! Given two points set a unit vector that goes from A to B
//!  \return the module of the unormalized vector (zero if error)
float UnitVectorPP( float *wX, float *wY, float *wZ,
					float aX,  float aY,  float aZ,
					float bX,  float bY,  float bz );

//! given three points set unit vector normal to its plane
//! its direction follows the ABC ride
//!  \return the module of the unormalized vector (zero if error)

float UnitVectorPPP( float *wX, float *wY, float *wZ,
					 float aX,  float aY,  float aZ,
					 float bX,  float bY,  float bZ,
					 float cX,  float cY,  float cz );

//! Vector cross product : w = u ^ v
void crossVV( float *wx, float *wy, float *wz,
			  float ux,  float uy,  float uz,
			  float vx,  float vy,  float vz );

//! Vector unit cross product : w = u ^ v / |u ^ v|
//!  \return the module of the unormalized vector (zero if error)
float UnitVectorVV( float *wX, float *wY, float *wZ,
					float uX,  float uY,  float uZ,
					float vX,  float vY,  float vz );

//! (unit) projection of v onto plane defined by r & s

/*!
  vector normal to   r & s           : nrs =   r ^ s

  vector normal to   v & nrs : vrs =   v ^ nrs       ( it is parallel to the desired plane )

  vector normal to vrs & nrs : prs = vrs ^ nrs       ( the desired vector )
*/

void Project( float *vX, float *vY, float *vZ,
			  float  rX, float  rY, float  rZ,
			  float  sX, float  sY, float  sZ );

/* ----- Hemendik aurrera ez egin ! --- */

//! Set a vector perpendicular to a given vector and (k or i) vector

void UnitVectorNV( float *wX, float *wY, float *wZ,
				   float uX,  float uY,  float uz );


//! UNTESTED TO_DO
void MakeNormal( float *outX, float *outY, float *outZ,
				 float  inX,  float  inY,  float  inZ,
				 float  refX, float  refY, float  refZ );

//! min angle defined by points AVB , V corner point
float MinAngle( float vX, float vY, float vZ,
				float aX, float aY, float aZ,
				float bX, float bY, float bZ );

//! Spherical coordenates of vector V
/*!
  \note normalizes V

  \param vX, vY, vZ -> coordinates of V
  \param alpha -> points to alpha
  \param beta -> points to beta

  \return alpha, beta, so that

  vX = cos(beta)*sin(alpha)

  vY = sin(beta)

  vZ = cos(beta)*cos(alpha)
*/

void VectorSphereCoordinates3f( float vX, float vY, float vZ, float *alpha, float *beta );

//! Spherical coordenates of vector V
/*!
  \note normalizes V

  \param vv vector of 3 floats [x,y,z]
  \param alpha -> points to alpha
  \param beta -> points to beta

  \return alpha, beta, so that

  vv[0] = cos(*beta)*sin(*alpha)

  vv[1] = sin(*beta)

  vv[2] = cos(*beta)*cos(*alpha)
*/

void VectorSphereCoordinates( float *vv, float *alpha, float *beta );

//! rotates vector around Y axis
//! like moving its end on its parallel
/*! look source file tools.c */
void VectorRotY( float *v, float inc );

//! rotates vector : like moving its end on its meridiam
/*! look source file tools.c */
void VectorRotXZ( float *vIn, float inc, int flagStop );


//! TRUE if A is inside pol

int PointInPolygon2D(int nVert, float *polx, float *poly, float ax, float ay);

//! moves respect their center both values
/*! look source file tools.c */
void shrink2P( float *val1, float *val2, float fraction );


//! print fps
void PrintTime( int numSamples, time_t *timeIni, time_t *timeEnd );

//!    Random number within init and end

/*!
  \note must call srand(seed) at the begining of the program
*/

float RandomWithinDomain( float init, float end );

void  DestroyString( char **thisString );
char *CopyString( char *thisString );
char *MergeStrings( char *stringA, char *stringB );
int   EndString( char *thisString, char *endString );

int   GetLine( char *line, int maxLine, FILE * );
int   GetNextInt ( char *line, int *value );
int   GetNextFloat ( char *line, float *value );
int   GetNextWord( char *line, char **output );
int   FindWord( char *thisString, char *token, int minLength );

void PrintGLMatrix(float M[16]);

#endif /* TOOLS_H */
