#ifndef TRFM3D_H
#define TRFM3D_H

#include <stdio.h>

/*! \file trfm3D.h

  Geometric transformations

  Let P be a (4 x 1) column vector representing a point or vector in
  homogeneous coordinates. Then, any transformation T will be applied as:

  P' = T * P

  Therefore, the order of the transformation is from right-to-left.

  Example. Suppose we have a point P = (2,3,5) and want to a) move it by the
  translation vector (1,1,1) and b) rotate PI/4 radians about the world Y
  axis. The following code performs the desired transformations:

  <code>
  float P[3] = {2.0, 3.0, 5.0}; // Original point
  trfm3D *T;
  T = CreateTrfm3D();
  SetRot_Y_Trfm3D( T, PI_VALUE / 4.0); // b) rotation pi/4 about Y axis. T = Rot
  AddTransTrfm3D( T, 1.0, 1.0, 1.0); // a) translation of (1,1,1). T = Rot*Trans
  Trfm3DTransformPoint(T, &P[0], &P[1], &P[2]); // Apply the transformation P' = T * P = Rot*Trans*P
  </code>
*/


//! The matrix will be stored split into its components

/*     | r1x*s  r2x*s  r3x*s tx | */
/* M = | r1y*s  r2y*s  r3y*s ty | */
/*     | r1z*s  r2z*s  r3z*s tz | */
/*     |   dx     dy     dz  dw | */

typedef struct _Trfm3D {

	float  r1X, r1Y, r1Z;
	float  r2X, r2Y, r2Z;
	float  r3X, r3Y, r3Z;
	float  scl;
	float  trX, trY, trZ;
	float  dX, dY, dZ, dW;
} trfm3D;

//! Destroy a trfm transformation

/*!
  This function receives a pointer to a pointer to a trmf3D and does two things:
  - frees the space used by the trmf3D
  - sets the pointer to the trmf3D to NULL

  Example:
  <code>
  trmf3D *t = CreateTrfm3D(); // creates a new transf. t points to it \n
  ... \n
  DestroyTrfm3D(&t); // now, t == NULL
  </code>
*/

void    DestroyTrfm3D( trfm3D ** );

//! Create a new trfm transformation

/*!
  This function allocates the space to store a trfm3D, and initializes it
  with the unit transformation
  \return the pointer to the new allocated trfm3D transformation
*/

trfm3D *CreateTrfm3D();

//! Create a copy of a trfm transformation

/*!
  This function allocates the space to store a trfm3D, and copies the transformation
  to it.
  \return the pointer to the new allocated absolute-value trfm3D transformation
*/

trfm3D *CreateCopyTrfm3D( const trfm3D * );

//! Transform a point P by transformation T
/*! Transform a point P by transformation T, post-multiplying the matrix T to
  the column-point P
  \result P' = T*P
*/

void Trfm3DTransformPoint(const trfm3D * , float *xtr, float *ytr, float *ztr );

//! Transform a vector V by transformation T
/*! Transform a vector V by transformation T, post-multiplying the matrix T to
  the column-vector V
  \result V' = T*V
*/
void TransformVectorTrfm3D(const trfm3D * , float *xtr, float *ytr, float *ztr );


/**
 * Transform a normal
 *
 */

void TransformNormalTrfm3D(const trfm3D *, float *nx, float *ny, float *nz);

/**
 * Transform a (normalized) plane given by N*X = d
 *
 */

void TransformPlaneTrfm3D(const trfm3D *, float *nx, float *ny, float *nz, float *d);

/**
 * Project a point P by proyection transformation
 *
 */

void ProjectPointTrfm3D( const trfm3D *thisTrfm3D, float *xTr, float *yTr, float *zTr );

/*! Gets an OpenGl compatible matrix with a trfm3D.

  glmatrix must have enough space to store the values of the matrix (16)
*/

void GetGLMatrixTrfm3D( const trfm3D * , float *glmatrix );

/*! Multiplies the OpenGl (active) matrix with the trfm3D.
  The function doesn't care about which OpenGL matrix is active (MODELVIEW, TEXTURE etc)
*/

void MultGLMatrixTrfm3D( const trfm3D * );

void SetUnitTrfm3D( trfm3D *thisTrfm3D ); //!< Set the trfm to the unit transformation

// copies values from source onto cloned
/*!

  \note cloned must point to a valid trfm3D transformation (i.e. can't be NULL)

*/

void SetCopyTrfm3D(trfm3D *cloned, const trfm3D *source );

void SetRot_X_Trfm3D(  trfm3D * , float angle ); //!< Sets the trfm to be a rotation of angle radians in the X axis
void SetRot_Y_Trfm3D(  trfm3D * , float angle ); //!< Sets the trfm to be a rotation of angle radians in the Y axis
void SetRot_Z_Trfm3D(  trfm3D * , float angle ); //!< Sets the trfm to be a rotation of angle radians in the Z axis
void SetRotVecTrfm3D(trfm3D * , float rx, float ry, float rz, float theta ); //!< Sets the trfm to be a rotation of theta radians on arbitrary axis r=(rx,ry,rz)
void SetTransTrfm3D( trfm3D * , float tx, float ty, float tz ); //!< Sets the trfm to be a translation T
void SetScaleTrfm3D( trfm3D * , float scale ); //!< Sets the trfm to be a uniform scale

//! Set the trfm to rotate angle radians about an axis defined by vector and
//! located at point
/*!

  \note vector must be an unit vector

*/

void SetRotAxisTrfm3D( trfm3D * , float vectorX, float vectorY, float vectorZ,
					   float pointX,  float pointY,  float pointZ,
					   float angle );


/**
 * Set the trfm so that it performs a perspective projection given by the
 * frustum parameters
 *
 * @param thisTrfm the trfm
 * @param left frustum left
 * @param right frustum right
 * @param top frustum top
 * @param bottom frustum bottom
 * @param near frustum near
 * @param far frustum far
 *
 */
void SetFrustumTrfm3D( trfm3D *thisTrfm,
					   float left, float right,
					   float top, float bottom,
					   float near, float far);

/**
 * Given a point (Px,Py,Pz), a right vector (Rx, Ry, Rz), an up vector (Ux,
 * Uy, Uz) and a vector looking backwards (Dx, Dy, Dz), all in world
 * coordinates, create a Local-to-World transformation.
 *
 * @param thisTrfm3D pointer to a transformation (can't be NULL)
 * @param Px x coord. of point P
 * @param Py y coord. of point P
 * @param Pz z coord. of point P
 * @param Rx x coord. of vector R
 * @param Ry y coord. of vector R
 * @param Rz z coord. of vector R
 * @param Ux x coord. of vector U
 * @param Uy y coord. of vector U
 * @param Uz z coord. of vector U
 * @param Dx x coord. of vector D
 * @param Dy y coord. of vector D
 * @param Dz z coord. of vector D
 */

void SetLocal2WorldTrfm3D(trfm3D *thisTrfm3D,
						  float Px, float Py, float Pz,
						  float Rx, float Ry, float Rz,
						  float Ux, float Uy, float Uz,
						  float Dx, float Dy, float Dz);


/**
 * Given a point (Px,Py,Pz), a right vector (Rx, Ry, Rz), an up vector (Ux,
 * Uy, Uz) and a vector looking backwards (Dx, Dy, Dz), all in world
 * coordinates, create a World-to-Local transformation.
 *
 * @param thisTrfm3D pointer to a transformation (can't be NULL)
 * @param Px x coord. of point P
 * @param Py y coord. of point P
 * @param Pz z coord. of point P
 * @param Rx x coord. of vector R
 * @param Ry y coord. of vector R
 * @param Rz z coord. of vector R
 * @param Ux x coord. of vector U
 * @param Uy y coord. of vector U
 * @param Uz z coord. of vector U
 * @param Dx x coord. of vector D
 * @param Dy y coord. of vector D
 * @param Dz z coord. of vector D
 */

void SetWorld2LocalTrfm3D(trfm3D *thisTrfm3D,
						  float Px, float Py, float Pz,
						  float Rx, float Ry, float Rz,
						  float Ux, float Uy, float Uz,
						  float Dx, float Dy, float Dz);

/**
 * Given a trfm that represents a local frame, create a World-to-Local
 * transformation
 *
 * @param thisTrfm3D pointer to a transformation (can't be NULL)
 * @param frameTrfm  a Trfm representing a frame
 */

void SetWorld2LocalFrameTrfm3D(trfm3D *thisTrfm3D, const trfm3D *frameTrfm);

/**
 * Normalize a trfm so that the three rotation vectors are unit lenght and
 * mutually orthogonal
 */

void NormalizeTrfm3D( trfm3D *);

void SetInverseTrfm3D( trfm3D * );	//!< Sets the matrix to its inverse

void AddRot_X_Trfm3D(  trfm3D * , float angle ); //!< Add the specified trfm a rotation of angle radians in the X axis
void AddRot_Y_Trfm3D(  trfm3D * , float angle ); //!< Add the specified trfm a rotation of angle radians in the Y axis
void AddRot_Z_Trfm3D(  trfm3D * , float angle ); //!< Add the specified trfm a rotation of angle radians in the Z axis
void AddRotVecTrfm3D(  trfm3D *, float rx, float ry, float rz, float theta ); //!< add to the trfm a rotation of theta radians on arbitrary axis r=(rx,ry,rz)
void AddRotAxisTrfm3D( trfm3D *, float rx, float ry, float rz, float pX,  float pY,  float pZ, float theta); //!< add to the trfm a rotation of theta radians on arbitrary axis r=(rx,ry,rz) located at point p=(px,py,pz)

void AddTransTrfm3D( trfm3D * , float tx, float ty, float tz ); //!< Add the specified trfm a translation T
void AddScaleTrfm3D( trfm3D * , float scale ); //!< Add the specified trfm a uniform scale


//! Absolute-value transformation

/*!
  This function transforms all the negative components to positive ones
*/

void AbsTrfm3D( trfm3D * );


/*! Transformation composition : add one trfm onto another one
  [target] = [target] * [add] */

void CompositionTrfm3D(trfm3D *target, const trfm3D *add);

/*! place P1 onto Q1

  place P2 onto line Q1-Q2, being Q1 the origin, place P2 in the same side as Q2

  place P3 onto plane Q123, being Q12 a line, place P3 on the same side as Q3 */

void Set3PointsTrfm3D( trfm3D * ,
					   float p1X, float p1Y, float p1Z,
					   float p2X, float p2Y, float p2Z,
					   float p3X, float p3Y, float p3Z,
					   float q1X, float q1Y, float q1Z,
					   float q2X, float q2Y, float q2Z,
					   float q3X, float q3Y, float q3Z );

/*! place P onto (0, 0, 0)

  place Q onto x axis (positive side)

  place R onto plane xy (z positive) */

void Set3PntStdTrfm3D( trfm3D *,
					   float px, float py, float pz,
					   float qx, float qy, float qz,
					   float rx, float ry, float rz );


// Streaming and debug

/*! Sets the (active) OpenGl matrix compatible with the trfm3D.
  The function doesn't care about which OpenGL matrix is active (MODELVIEW, TEXTURE etc)
*/
//void SetGLMatrixTrfm3D( const trfm3D * );


void PrintTrfm3D( const trfm3D * ); //!< Print to stdout the matrix

// ***** COMMENTS *****
// ***** COMMENTS *****
/*
  As we seek compatibility with OpenGL,
  we will compose transformations in the following way

  | r1x*s  r1y*s  r1z*s 0 |
  {x' y' z' 1} = {x  y  z  1} | r2x*s  r2y*s  r2z*s 0 |
  | r3x*s  r3y*s  r3z*s 0 |
  |  tx     ty     tz   1 |

  So composition will be :

  P1 = P0 * m1                            = P0 * M1
  P2 = P1 * m2 = P0 * m1 * m2             = P0 * M2
  P3 = P2 * m3 = P0 * m1 * m2 * m3        = P0 * M3

  NOTE :
  In C, we understand m[i][j] as the element placed in row [i], column [j]
  As matrices store data row by row, then :
  void list( int *mAsArray )
  {
  int		 i;
  printf("list as array\n");
  for ( i=0 ; i<16; i++ )
  printf("%d	%d\n", i, mAsArray[i] );
  }
  int main()
  {
  int		 mAsmatrix[4][4];
  int		 pocic, i , j;
  pocic=0;
  for ( i=0 ; i<4; i++ )
  for ( j=0 ; j<4; j++ )
  { mAsmatrix[i][j] = pocic; pocic++; }
  printf("list by rows\n");
  for ( i=0 ; i<4; i++ ) {
  printf("row %d : ", i);
  for ( j=0 ; j<4; j++ )
  printf(" %d", mAsmatrix[i][j]);
  printf("\n");
  }
  list( mAsmatrix );
  }
  list by rows
  row 0 :  0 1 2 3
  row 1 :  4 5 6 7
  row 2 :  8 9 10 11
  row 3 :  12 13 14 15
  list as array
  0       0
  1       1
  2       2
  3       3
  4       4
  5       5
  6       6
  7       7
  8       8
  9       9
  10      10
  11      11
  12      12
  13      13
  14      14
  15      15

  Then, if
  mAsMatrix has M rows and N columns,
  then
  mAsArray[i*M+j] corresponds with element in row [i], column [j]

  However OpenGL expects mAsArray stored in column order,
  so there are two ways to create transformation matrices for OpenGL in C :
  * Store the data into one array following the rule
  float m[16]; // store the matrix column by column
  |  0   4   8   12 |
  |  1   5   9   13 |
  |  2   6  10   14 |
  |  3   7  11   15 |
  * Store the data into a 4x4 matrix
  float m[4][4];
  but, keep into account that we must follow an inverse "reading"
  Here m[i][j] must be the element placed in row [** j **], column [** i **]
*/

/*
  Angle units will be used in the following way
  - angle values will be always stored as radians
  - fovy variables (field of view) will be stored as degrees (exception to the rule)
  - angle constants will be written in degrees
  - the user interface will use degrees, but before storing those values will
  be converted into radians
  - Conversions among radians and degrees will be carried out
  using constants stored in the following parameters (see tools.h) :
  angle * DEGREE_TO_RAD transforms from degrees into radians
  angle * RAD_TO_DEGREE transforms from radians into degrees
*/

/*! Transformation composition : add one trfm onto another one using
  post-multiplication.

  DON'T USE THIS FUNCTION !!

  [target] = [add] * [target] */

int cmpTrfm3D(trfm3D *A, trfm3D *B);

#endif /* TRFM3D_H */
