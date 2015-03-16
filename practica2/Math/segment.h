#ifndef SEGMENT_H
#define SEGMENT_H

/*!
  \file segment.h

  Segments

  The segment internally is stored by means of a point O = (ox,oy,oz) and a
  direction vector D = (dx,dy,dz)

  The eq. of the segment is:

  p(u) = O + uD

*/

typedef struct _Segment {

	//! A point o
	float ox, oy, oz;
	//! A direction vector d
	float dx, dy, dz;
	float a,b;
} segment;


//! Create a segment given a point and a direction vector
/*!
  Given a direction vector D = (dx,dy,dz) and a point O = (ox,oy,oz) ,
  create a segment with eq.

  p(u) = O + uD , a <= u <= b

*/

segment *CreateSegment(float ox, float oy, float oz, float dx, float dy, float dz, float a, float b);

//! Given two points, A and B, create a segment which goes from A to B

segment *CreateSegmentPP(float Ax, float Ay, float Az, float Bx, float By, float Bz);

void DestroySegment(segment **theSegment);

//! Distance between the segment and a point

float DistanceSegmentP(segment * theSegment, float Px, float Py, float Pz);

//! Given a point P, return the value of parameter u in [a,b] nearest the point


float ParamDistanceSegment(segment * theSegment, float Px, float Py, float Pz);

#endif // SEGMENT_H
