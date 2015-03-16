#include "tools.h"

float VectorNormalize( float *vx, float *vy, float *vz ) {

	float         A, B, C;
	float         modf;
	float         mod2;
	float         mod;

	A = *vx;
	B = *vy;
	C = *vz;
	mod2 = A*A + B*B + C*C;
	if( mod2 > VECTOR_EPSILON ) {
		mod = sqrtf( mod2 );
		modf = 1.0f / mod;
		*vx = A * modf;
		*vy = B * modf;
		*vz = C * modf;
	}
	else {
		mod = 0.0;
		*vx = 0.0;
		*vy = 0.0;
		*vz = 1.0;
	}
	return mod;
}

float VectorSumOne( float *vX, float *vY, float *vZ ) {

	float m;
	float d;

	m = *vX + *vY + *vZ;

	d = (float) 1.0 / m;

	*vX *= d;
	*vY *= d;
	*vZ *= d;
	return m;
}

void UnitVectorNV( float *wx, float *wy, float *wz,
				   float  ux,  float uy,  float uz ) {

	// SET a vector perpendicular to a given vector and (k or i) vector

	if( ux > 0.1 || ux < -0.1) {
		// *** the k vector
		// w = k ^ u
		*wx = -uy;  *wy = ux;       *wz = 0.0f;
	}
	else {
		// *** the i vector
		// w = i ^ u
		*wx = 0.0f; *wy = -uz;      *wz = uy;
	}

}

float UnitVectorPP( float *wx, float *wy, float *wz,
					float ax,  float ay,  float az,
					float bx,  float by,  float bz ) {

	// given two points SET a unit vector that goes from A to B
	*wx = bx - ax;
	*wy = by - ay;
	*wz = bz - az;
	return VectorNormalize( wx, wy, wz );
}

float UnitVectorPPP( float *wx, float *wy, float *wz,
					 float ax,  float ay,  float az,
					 float bx,  float by,  float bz,
					 float cx,  float cy,  float cz ) {

	// given three points SET unit vector normal to its plane
	// its direction follows the ABC ride

	/*   float  ux, uy, uz; */
	/*   float  vx, vy, vz; */

	/*   UnitVectorPP( ierr, &ux, &uy, &uz, ax, ay, az, bx, by, bz ); */
	/*   UnitVectorPP( ierr, &vx, &vy, &vz, bx, by, bz, cx, cy, cz ); */
	/*   UnitVectorVV( ierr, wx, wy, wz, ux, uy, uz, vx, vy, vz ); */

	// Following ABC ride:
	// NN = (B - A) ^ (C - A)
	// W = NN / |NN|

	float ba_x, ba_y, ba_z;
	float ca_x, ca_y, ca_z;

	ba_x = bx - ax;
	ba_y = by - ay;
	ba_z = bz - az;

	ca_x = cx - ax;
	ca_y = cy - ay;
	ca_z = cz - az;

	crossVV( wx, wy, wz,
			 ba_x, ba_y, ba_z,
			 ca_x, ca_y, ca_z
			 );

	return VectorNormalize( wx, wy, wz );
	//  printf( "N %f %f %f \n", *wx, *wy, *wz );
}

void crossVV( float *wx, float *wy, float *wz,
			  float ux,  float uy,  float uz,
			  float vx,  float vy,  float vz ) {

	// Vector (cross) product : w = u ^ v

	*wx = uy * vz - uz * vy;
	*wy = uz * vx - ux * vz;
	*wz = ux * vy - uy * vx;
}

float UnitVectorVV(  float *wx, float *wy, float *wz,
					 float ux,  float uy,  float uz,
					 float vx,  float vy,  float vz ) {

	// Vector product : w = u ^ v
	*wx = uy * vz - uz * vy;
	*wy = uz * vx - ux * vz;
	*wz = ux * vy - uy * vx;
	return VectorNormalize(  wx, wy, wz );
}

void Project( float *vX, float *vY, float *vZ,
			  float  rX, float  rY, float  rZ,
			  float  sX, float  sY, float  sZ ) {

	// (unit) projection of v onto plane defined by r & s

	// vector normal to   r & s           : nrs =   r ^ s
	// vector normal to   v & nrs : vrs =   v ^ nrs       ( it is parallel to the desired plane )
	// vector normal to vrs & nrs : prs = vrs ^ nrs       ( the desired vector )

	float  nrsX, nrsY, nrsZ;
	float  vrsX, vrsY, vrsZ;
	float  prsX, prsY, prsZ;
	float  prod;


	UnitVectorVV(  &nrsX, &nrsY, &nrsZ,
				   rX,    rY,    rZ,
				   sX,    sY,    sZ );

	// an error occurs if r & s are colinear

	UnitVectorVV(  &vrsX, &vrsY, &vrsZ,
				   *vX,   *vY,   *vZ,
				   nrsX,  nrsY,  nrsZ );
	UnitVectorVV(  &prsX, &prsY, &prsZ,
				   vrsX,  vrsY,  vrsZ,
				   nrsX,  nrsY,  nrsZ );

	prod = *vX * prsX + *vY * prsY  + *vZ * prsZ;
	if( prod < 0.0f ) {
		*vX = -prsX;
		*vY = -prsY;
		*vZ = -prsZ;
	}
	else {
		*vX =  prsX;
		*vY =  prsY;
		*vZ =  prsZ;
	}
}

// UNTESTED TO_DO
void MakeNormal( float *outX, float *outY, float *outZ,
				 float  inX,  float  inY,  float  inZ,
				 float  refX, float  refY, float  refZ ) {

	/* make v in a way that :                          */
	/*   it remains in the plane defined by v and refv */
	/*   it rotates to be perpendicular to refv        */
	/*                         +      +  output : out  */
	/*              input : in  \     |                */
	/*                           \    |                */
	/*                            \   |                */
	/*                             \  |                */
	/*            ref               \ |                */
	/*          +--------------------\                 */
	/*      vv = ref ^ in                              */
	/*     out =  vv ^ ref                             */

	float  vvX,   vvY,   vvZ; // auxiliar vector
	float  mod;


	mod = AGA_MOD( inX, inY, inZ );
	UnitVectorVV(  &vvX,  &vvY,  &vvZ,
				   refX,  refY,  refZ,
				   inX,   inY,   inZ );
	UnitVectorVV(  outX,  outY,  outZ,
				   vvX,   vvY,   vvZ,
				   refX,  refY,  refZ );
	*outX = (*outX) * mod;
	*outY = (*outY) * mod;
	*outZ = (*outZ) * mod;
}

// min angle defined by points AVB , V corner point
float MinAngle( float vX, float vY, float vZ,
				float aX, float aY, float aZ,
				float bX, float bY, float bZ ) {

	float  vaX, vaY, vaZ;
	float  vbX, vbY, vbZ;
	float  vaMod, vbMod;
	float  angle;


	vaX = aX - vX;
	vaY = aY - vY;
	vaZ = aZ - vZ;
	vaMod = AGA_MOD( vaX,  vaY,  vaZ );
	// an error occurs if A & V are too close
	if( vaMod < VECTOR_EPSILON ) { return 0.0f; }

	vbX = bX - vX;
	vbY = bY - vY;
	vbZ = bZ - vZ;
	vbMod = AGA_MOD( vbX,  vbY,  vbZ );
	// an error occurs if B & V are too close
	if( vbMod < VECTOR_EPSILON ) {  return 0.0f; }

	// result must be : pi < angle <= 0
	angle = (float) acos( ( vaX*vbX + vaY*vbY + vaZ*vbZ ) / ( vaMod * vbMod ) );

	return angle;
}

void VectorSphereCoordinates( float *vv, float *alpha, float *beta ) {

	VectorSphereCoordinates3f( vv[0], vv[1], vv[2], alpha, beta );
}


// Given V = (vx, vy, vz), calculate (alpha, beta), so that:
//
// vx = |V| * cos beta * sin alfa
// vy = |V| * sin beta
// vz = |V| * cos beta * cos alfa
//
// also ,    -PI <  alpha  <  PI
//           0  <=  beta  <= PI/2
//

void VectorSphereCoordinates3f( float vx, float vy, float vz, float *alpha, float *beta ) {

	float  modZX;

	VectorNormalize( &vx, &vy, &vz );

	modZX = AGA_MOD(vx, 0, vz);
	if (modZX < 0.00873) {
		// If modZX is near zero beta is (+/-) PI/2 (degenerate solution) and alpha is 0.
		// 0.00873 =~ distance of ZX projection when beta is PI/2 +/- 5 degrees
		*alpha = 0.0f;
		*beta = PI_VALUE / 2.0f;
		if( vy < 0.0f ) *beta = - (*beta);
	} else {
		*beta = atan2f(vy, modZX);
		*alpha = atan2f(vx, vz);
	}
}

void VectorRotY( float *vIn, float inc ) {

	float  alpha;
	float  modZX;
	float  mod;

	//         __________________> X
	//        |*
	//        | *
	//        |  *
	//        |   *
	//        |    *
	//        |     *
	//        |      *
	//        | alpha *
	//        |        *
	//        v
	//       Z

	mod = AGA_MOD( vIn[0], vIn[1], vIn[2] );
	if( mod < VECTOR_EPSILON ) return;
	vIn[0] = vIn[0] / mod;
	vIn[1] = vIn[1] / mod;
	vIn[2] = vIn[2] / mod;

	// if vector is too parallel to the "y" axis do nothing
	if( fabs( vIn[1] ) > sin( PI_VALUE/2.0 - ANGLE_EPSILON ) ) return;
	modZX = (float) sqrt( vIn[0]*vIn[0] + vIn[2]*vIn[2] );
	alpha = (float) acos( vIn[2] / modZX );
	if( vIn[0] < 0.0f ) alpha = 2.0f * PI_VALUE - alpha;

	alpha += inc;
	vIn[0] = (float) sin( alpha ) * modZX;
	vIn[2] = (float) cos( alpha ) * modZX;

	vIn[0] = vIn[0] * mod;
	vIn[1] = vIn[1] * mod;
	vIn[2] = vIn[2] * mod;
}

void VectorRotXZ( float *vIn, float inc, int flagStop ) {

	float  alpha, beta;
	float  mod;
	float  maxAngle = 90.0f * DEGREE_TO_RAD - ANGLE_EPSILON;

	// Plane that contains the vector and the "y" axis
	//
	//      Y
	//        ^
	//        |
	//        |
	//        |        *
	//        |       *
	//        |      *
	//        |     *
	//        |    *
	//        |   *
	//        |  *
	//        | *  beta
	//        |*
	//         ------------------> X-Z
	//

	mod = AGA_MOD( vIn[0], vIn[1], vIn[2] );
	if( mod < VECTOR_EPSILON ) return;
	vIn[0] = vIn[0] / mod;
	vIn[1] = vIn[1] / mod;
	vIn[2] = vIn[2] / mod;

	// if vector is too parallel to the "y" axis do nothing
	if( fabs( vIn[1] ) > sin( maxAngle ) ) return;

	// 1 Compute alpha & beta
	//
	alpha = (float) acos( vIn[2] / sqrt( vIn[0]*vIn[0] + vIn[2]*vIn[2] ) );
	if( vIn[0] < 0.0f ) alpha = 2.0f * PI_VALUE - alpha;

	// hypotenuse must be always 1.0 (because v is a unit vector)
	// first we measure beta from X-Z up to our vector
	// the result will be among -90 and +90
	beta = (float) asin( vIn[1] );

	// 2 ConstantIncrement beta
	// (two possibilities)
	if( flagStop ) {
		// when beta goes further than pi/2 or -pi/2 => stop avoiding a vertical position
		beta += inc;
		if( beta > maxAngle )                beta =   maxAngle;
		else if( beta < - maxAngle ) beta = - maxAngle;
	}
	else {
		// to keep a constant rotation direction
		// inc must be a positive value
		if( alpha > PI_VALUE )      beta += inc;
		else                                        beta -= inc;
	}

	// 3 Compute new vector
	//
	vIn[0] = (float) cos( beta ) * (float) sin( alpha );
	vIn[1] = (float) sin( beta );
	vIn[2] = (float) cos( beta ) * (float) cos( alpha );

	vIn[0] = vIn[0] * mod;
	vIn[1] = vIn[1] * mod;
	vIn[2] = vIn[2] * mod;
}

int PointInPolygon2D(int nVert, float *polx, float *poly, float ax, float ay) {

	// by Randolph Franklin
	int i, j, c = 0;
	for (i = 0, j = nVert-1; i < nVert; j = i++) {
		if (
			( ( (poly[i] <= ay) && (ay < poly[j]) ) ||
			  ( (poly[j] <= ay) && (ay < poly[i]) )    )
			&&
			( ax < (polx[j] - polx[i]) * (ay - poly[i]) / (poly[j] - poly[i]) + polx[i] )
			)
			c = !c;
	}
	return c;
}

void shrink2P( float *val1, float *val2, float fraction ) {

	// moves respect their center both values
	//       ------1--------------------2------  -0.10
	//       -------1------------------2-------  -0.05
	//               1                2
	//       --------*----------------*--------   0 (original position)
	//               1                2
	//       ---------1--------------2---------  +0.05
	//       ----------------12----------------  +0.50
	//       ----------2------------1----------  +0.90
	//       --------1----------------2--------  +1.00
	//       ------1--------------------2------  +1.10

	float  v1, v2;
	float  delta;

	v1 = *val1;
	v2 = *val2;
	delta = (float) fabs( v2 - v1 ) * fraction;

	if( v1 < v2 ) {
		*val1 = v1 + delta;
		*val2 = v2 - delta;
	}
	else {
		*val2 = v2 + delta;
		*val1 = v1 - delta;
	}
}

void PrintTime( int numSamples, time_t *timeIni, time_t *timeEnd ) {

	float  timeSpent;
	float  fps;
	int    i, i1;
	int    tmpI;

	timeSpent = 0;
	for( i=0; i<numSamples ; i++ ) {
		timeSpent += timeEnd[ i ] - timeIni[ i ];
	}
	timeSpent = ( timeSpent / CLOCKS_PER_SEC ) / numSamples;
	fps = 1.0f / timeSpent;
	printf( "fps draw%3.0f ", fps );

	timeSpent = 0;
	for( i=0; i<numSamples ; i++ ) {
		i1 = i + 1;
		if( i1 == numSamples ) i1 = 0;
		tmpI = timeEnd[ i1 ] - timeEnd[ i ];
		if( tmpI > 0 ) timeSpent += tmpI;
	}
	timeSpent = ( timeSpent / CLOCKS_PER_SEC ) / ( numSamples - 1 );
	fps = 1.0f / timeSpent;
	printf( " total%3.0f \n", fps );
}

float RandomWithinDomain( float init, float end ) {

	float  result;
	// The rand function returns a pseudorandom integer in the range 0 to RAND_MAX.
	// Use the srand function to seed the pseudorandom-number generator before calling rand.
	// #include <stdlib.h>
	// void main( void )
	// {
	//    Seed the random-number generator with current time so that
	//      the numbers will be different every time we run.
	//    srand( seed );
	//    (double) rand() / (double) RAND_MAX ;
	// }

	result = (float) ( (double) rand() / (double) RAND_MAX );
	result = init + result * ( end - init );
	return result;
}

void DestroyString( char **theString ) {

	char  *thisString = *theString;

	if( !thisString ) return;

	free( thisString );

	*theString = NULL;
}

char *CopyString( char *thisString ) {

	int            length;
	char  *newString;

	if( ! thisString ) return NULL;

	length = 1 + (int) strlen( thisString );
	// printf("%d\n", length);

	newString = malloc( sizeof(*newString) * length );
	strcpy( newString, thisString );
	return newString;

	/*  we used this code
		length = 0;
		while( thisString[length] != 0 ) {
		length++;
		}
		length++;
	*/
}

char *MergeStrings( char *stringA, char *stringB ) {

	int            length;
	char  *newString;

	if( ! stringA && ! stringB ) return NULL;
	if( ! stringA  ) return CopyString( stringB );
	if( ! stringB  ) return CopyString( stringA );

	length = 1 + (int) strlen( stringA ) + (int) strlen( stringB );
	//printf("%d\n", length);

	newString = malloc( sizeof(*newString) * length );
	strcpy( newString, stringA );
	strcat( newString, stringB );
	return newString;
}

int   EndString( char *thisString, char *endString ) {

	int            thisLength;
	int            endLength;
	int            diff, i;

	if( ! thisString ) return FALSE;

	thisLength = strlen( thisString );
	endLength  = strlen( endString );

	if( thisLength < endLength ) return FALSE;

	//printf("end %d %d \n", thisLength , endLength);
	diff = thisLength - endLength;
	for( i=0; i<endLength; i++ ) {
		if( thisString[diff+i] != endString[i] ) return FALSE;
	}

	return TRUE;
}

int GetLine(char* line, int maxChar, FILE *readFile) {

	if( fgets(line, maxChar-1, readFile ) == NULL ) {
		// printf("0\n");
		return 0;
	}
	else {
		// printf("%d>>%s\n", strlen(line), line );
		return strlen(line);
	}
}

/* old version
   int GetLine(char* line, int maxChar, FILE *readFile )
   {
   char newChar  = 0;
   int   charRead = 0;
   int   charLast;
   int   i;

   charLast = maxChar - 1;

   for (i=0;i<maxChar;i++)
   line[i] = 0;

   while( newChar != 10 ) {

   newChar = getc( readFile );

   if( newChar == EOF ) {
   if( charRead == 0 )
   return 0;
   else
   newChar = 10;
   }

   line[charRead] = newChar;
   charRead++;

   if( charRead == charLast ) {
   //                   el standard devuelve lo leido hasta charLast
   return -1;
   }
   // para truncar lineas :
   //           quitar la segunda condicion del while "&& charRead < charLast"
   //           e imponer esta condicion a las dos lineas anteriores
   //           if( charRead < maxLast ) {
   //                   line[charRead] = newChar;
   //                   charRead++;
   //           }
   //
   }
   return charRead;
   }
*/

int GetNextInt( char *line, int *output ) {

	int            i;
	int            charsRead;
	int            valueRead = 0;
	/*
	  int ctrlRead;
	  ctrlRead = sscanf( line, "%d%n", &valueRead, &charsRead );
	  //printf("%d\n", ctrlRead);
	  if( ctrlRead != 1 ) return -1;
	  //printf("%d\n", charsRead);
	  //printf("%s", line);
	  */
	if( sscanf( line, "%d%n", &valueRead, &charsRead ) != 1 ) {
		*output = 0;
		return -1;
	}

	// delete the integer value from the input line
	for(i=0; i<charsRead; i++ ) {
		line[i] = SP_ASCII;
	}
	*output = valueRead;
	return charsRead;
}

int GetNextFloat( char *line, float *output ) {

	int            i;
	int            charsRead;
	float  valueRead = 0.0f;

	if( sscanf( line, "%f%n", &valueRead, &charsRead ) != 1 ) {
		*output = 0.0f;
		return -1;
	}

	// delete the float value from the input line
	for(i=0; i<charsRead; i++ ) {
		line[i] = SP_ASCII;
	}
	*output = valueRead;
	return charsRead;
}

int GetNextWord( char *line, char **output ) {

	int            i;
	int            position;
	int            charRead;
	int            start, end;
	int            charsInToken;
	char  *tokenRead;

	tokenRead             = NULL;
	position              =  0;
	start                 = -1;
	end                           = -1;
	charsInToken  =  0;
	charRead              = line[0];

	while( charRead != 0 ) {
		//printf(">%c< %d\n", charRead, charRead );
		if( start > -1 ) {
			if(
			   charRead ==  0 ||              // NULL
			   charRead ==  9 ||              // TAB
			   charRead == 10 ||              // LF line feed
			   charRead == 13 ||              // CR carriage return
			   charRead == 32                 // SPACE
			   )   {
				break;
			}
			else {
				charsInToken++;
			}
		}
		else {
			if( charRead !=  0 && charRead !=  9 &&
				charRead != 10 && charRead != 13 &&
				charRead != 32                                                )    {
				start    = position;
				charsInToken = 1;
			}
		}
		position++;
		charRead = line[position];
	}
	if( charsInToken > 0 ) {
		tokenRead = malloc( sizeof(*tokenRead) * (charsInToken + 1) );
		for( i=0; i<charsInToken ; i++ ) {
			tokenRead[i] = line[ i + start ];
		}
		end = start + charsInToken;
		for( i=0; i<end ; i++ ) {
			line[ i ] = 32;
		}
		tokenRead[charsInToken] = 0;
	}
	*output = tokenRead;
	return end;
}

int FindWord( char *thisString, char *token, int minLength ) {

	// search thisString for first string "token" or for its first "minLength" characters
	// set to <SPACE> token characters in thisString

	char  *minStr;
	char  *tokenFound;
	char  *tmp;
	int            position, tmpInt;

	tmpInt = strlen( token );
	if( tmpInt < minLength ) {
		printf(" Warning : bad use of FindWord %s \n", token );
		minLength = tmpInt;
	}
	if( minLength < 1) {
		printf(" Warning : bad use of FindWord %s \n", token );
		return -1;
	}

	minStr = malloc( sizeof(*minStr) * minLength + 1 );
	strncpy( minStr, token, minLength );
	minStr[minLength] = 0;

	tokenFound = strstr( thisString, minStr );

	if( ! tokenFound ) {
		DestroyString( &minStr );
		return -1;
	}

	tmp = thisString;
	position = 0;
	while( tmp != tokenFound ) {
		position++;
		tmp++;
	}

	while( *tokenFound == *token && *tokenFound != 0 ) {
		*tokenFound = 32;
		tokenFound++;
		token++;
	}

	DestroyString( &minStr );
	//printf(" FindWord >>%s<< %d \n", thisString, position );

	return position;
}

/* void pMatrix(const GLfloat *p) { */

/*   int i, j; */

/*   for (i=0; i<4; ++i) { */
/*     for(j=0; j<3; ++j) */
/*       printf("%.3f  ", p[4*j+i]); */
/*     printf("%.3f\n", p[12+i]); */
/*   } */

/* } */

void PrintGLMatrix(float M[16]) {

	int i, j;

	for (i=0; i<4; ++i) {
		for(j=0; j<3; ++j)
			printf("%.3f  ", M[4*j+i]);
		printf("%.3f\n", M[12+i]);
	}
}
