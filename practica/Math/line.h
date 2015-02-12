#ifndef LINE_H
#define LINE_H

/*!
  \file line.h

  Lines

  The line internally is stored by means of a point O = (ox,oy,oz) and a
  direction vector D = (dx,dy,dz)

  The eq. of the line is:

  p(u) = O + uD

*/

typedef struct _Line {

	//! A point o
	float ox, oy, oz;
	//! A direction vector d
	float dx, dy, dz;
} line;


//! Create a line given a point and a direction vector
/*!
  Given a direction vector D = (dx,dy,dz) and a point O = (ox,oy,oz) ,
  create a line with eq.

  p(u) = O + uD

*/

line *CreateLine(float ox, float oy, float oz, float dx, float dy, float dz);

//! Given two points, A and B, create a line which goes from A to B

line *CreateLinePP(float Ax, float Ay, float Az, float Bx, float By, float Bz);

void DestroyLine(line **theLine);

//! Give the point corresponding to parameter t

void PointInLine(line *theLine, float t, float *Px, float *Py, float *Pz);

//! Minimum distance between line and a point

float DistanceP(line * theLine, float Px, float Py, float Pz);

//! Given a point P, return the value of parameter u nearest the point

float ParamDistance(line * theLine, float Px, float Py, float Pz);

//! Print line

void PrintLine(line *theLine);

#endif // LINE_H
