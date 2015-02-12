#include <stdlib.h>
#include <stdio.h>
#include "line.h"
#include "plane.h"
#include "bbox.h"
#include "bsphere.h"
#include "intersect.h"

////// WARNING !!! IREJECT == 1 // IINTERSECT == 0

void pause() {
	printf("Press ENTER to continue...\n");
	getchar();
}

//***************************************************
//***
//*** Distance from Point to Line
//***
//***************************************************

void caseLine1(float ax, float ay, float az, float bx, float by, float bz, float px, float py, float pz) {

	line *line;
	float d, u;

	line = CreateLinePP(ax, ay, az, bx, by, bz);
	u = ParamDistance(line, px, py, pz);
	d = DistanceP(line, px, py, pz);

	printf("\n");
	printf(" Result for line:\n");
	printf("      A(%.3f,%.3f,%.3f)\n", ax, ay, az);
	printf("      B(%.3f,%.3f,%.3f)\n", bx, by, bz);
	printf("      Point(%.3f,%.3f,%.3f)\n", px, py, pz);
	printf("      u = %.3f       distance = %.3f\n", u, d);

	DestroyLine(&line);

	pause();

}

void caseLine2() {
	float ox, oy, oz, dx, dy, dz, px, py, pz;

	line *line;
	float d, u;

	printf("Ox, Oy, Oz:");
	scanf("%f, %f, %f", &ox, &oy, &oz);
	printf("\n");

	printf("Dx, Dy, Dz:");
	scanf("%f, %f, %f", &dx, &dy, &dz);
	printf("\n");

	printf("Px, Py, Pz:");
	scanf("%f, %f, %f", &px, &py, &pz);
	printf("\n");

	getchar();

	line = CreateLine(ox, oy, oz, dx, dy, dz);
	u = ParamDistance(line, px, py, pz);
	d = DistanceP(line, px, py, pz);

	printf("\n");
	printf(" Results for line:\n");
	printf("      O(%.3f,%.3f,%.3f)\n", ox, oy, oz);
	printf("      D(%.3f,%.3f,%.3f)\n", dx, dy, dz);
	printf("      Point(%.3f,%.3f,%.3f)\n", px, py, pz);
	printf("      u = %.3f       distance = %.3f\n", u, d);

	DestroyLine(&line);

	pause();
}

void menuLine() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test Distance from point to line\n");
		printf ("**************************************\n\n");

		printf(" 1. Line from A(0,0,0) to B(1,1,0) and Point(0,1,0)\n");
		printf(" 2. Line from A(0,0,0) to B(1,1,0) and Point(0.5,0.5,0.5)\n");
		printf(" 3. Line from A(5,3,1) to B(1,4,-2) and Point(1.7692, 3.8077, -1.4231)\n");
		printf(" 4. Line from A(5,3,1) to B(1,4,-2) and Point(1,4,-3)\n");
		printf(" 5. Enter data\n");
		printf(" 6. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%d", &option); getchar();

		switch( option ) {
		case 1:
			caseLine1(0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0);
			break;
		case 2:
			caseLine1(0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.5, 0.5, 0.5);
			break;
		case 3:
			caseLine1(5, 3, 1, 1, 4, -2, 1.7692, 3.8077, -1.4231);
			break;
		case 4:
			caseLine1(5, 3, 1, 1, 4, -2, 1, 4, -3);
			break;
		case 5:
			caseLine2();
			break;
		case 6:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

//***************************************************
//***
//*** CASE INTERSECT AABB and AABB
//***
//***************************************************

void caseAABBs1(float x1min, float y1min, float z1min, float x1max, float y1max, float z1max,
				float x2min, float y2min, float z2min, float x2max, float y2max, float z2max) {
	BBox *bb1, *bb2;
	int result;

	bb1 = CreateBBox(x1min, y1min, z1min, x1max, y1max, z1max);
	bb2 = CreateBBox(x2min, y2min, z2min, x2max, y2max, z2max);
	result = BBoxBBoxIntersect(bb1, bb2);

	printf("\n");
	printf(" Result for BBox-BBox:\n");
	printf("      BBox1{(%.3f, %.3f, %.3f), (%.3f, %.3f, %.3f)}\n", x1min, y1min, z1min, x1max, y1max, z1max);
	printf("      BBox2{(%.3f, %.3f, %.3f), (%.3f, %.3f, %.3f)}\n", x2min, y2min, z2min, x2max, y2max, z2max);
	printf("      Intersect = %s\n", intersect_string(result));

	DestroyBBox(&bb1);
	DestroyBBox(&bb2);

	pause();

}

void caseAABBs2() {
	float x1min, y1min, z1min, x1max, y1max, z1max;
	float x2min, y2min, z2min, x2max, y2max, z2max;
	BBox *bb1, *bb2;
	int result;

	// First AABB
	printf("\n"); printf("AABB 1\n");
	printf("  Min xmin, ymin, zmin:");
	scanf("%f, %f, %f", &x1min, &y1min, &z1min);
	printf("\n");
	printf("  Max xmax, ymax, zmax:");
	scanf("%f, %f, %f", &x1max, &y1max, &z1max);

	// Second AABB
	printf("\n"); printf("AABB 2\n");
	printf("  Min xmin, ymin, zmin:");
	scanf("%f, %f, %f", &x2min, &y2min, &z2min);
	printf("\n");
	printf("  Max xmax, ymax, zmax:");
	scanf("%f, %f, %f", &x2max, &y2max, &z2max);
	printf("\n");

	getchar();

	bb1 = CreateBBox(x1min, y1min, z1min, x1max, y1max, z1max);
	bb2 = CreateBBox(x2min, y2min, z2min, x2max, y2max, z2max);
	result = BBoxBBoxIntersect(bb1, bb2);

	printf("\n");
	printf(" Result for BBox-BBox:\n");
	printf("      BBox1{(%.3f, %.3f, %.3f), (%.3f, %.3f, %.3f)}\n", x1min, y1min, z1min, x1max, y1max, z1max);
	printf("      BBox2{(%.3f, %.3f, %.3f), (%.3f, %.3f, %.3f)}\n", x2min, y2min, z2min, x2max, y2max, z2max);
	printf("      Intersect = %s\n", intersect_string(result));

	DestroyBBox(&bb1);
	DestroyBBox(&bb2);

	pause();

}

void menuAABBs() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect AABB-AABB\n");
		printf ("**************************************\n\n");

		printf(" 1. AABB{(0,0,0),(1,1,1)} and AABB{(0,0,0),(2,2,2)}\n");
		printf(" 2. AABB{(0,0,0),(1,1,1)} and AABB{(1.5,1.5,1.5),(2,2,2)}\n");
		printf(" 3. AABB{(-1,-1,-1),(1,1,1)} and AABB{(0,0,0),(2,2,2)}\n");
		printf(" 4. AABB{(0,0,0),(1,1,1)} and AABB{(1,1,1),(2,2,2)}\n");
		printf(" 5. Enter data\n");
		printf(" 6. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseAABBs1(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0);
			break;
		case 2:
			caseAABBs1(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.5, 1.5, 1.5, 2.0, 2.0, 2.0);
			break;
		case 3:
			caseAABBs1(-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0);
			break;
		case 4:
			caseAABBs1(0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0);
			break;
		case 5:
			caseAABBs2();
			break;
		case 6:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

//***************************************************
//***
//*** CASE INTERSECT AABB and Plane
//***
//***************************************************

void caseAABBplane1(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax, float nx, float ny, float nz, float d) {
	BBox *bb;
	plane *p;
	int result;

	bb = CreateBBox(xmin, ymin, zmin, xmax, ymax, zmax);
	p = CreatePlane(nx, ny, nz, d);
	result = BBoxPlaneIntersect(bb, p);

	printf("\n");
	printf(" Result for AABB-plane:\n");
	printf("      BBox{(%.3f, %.3f, %.3f), (%.3f, %.3f, %.3f)}\n", xmin, ymin, zmin, xmax, ymax, zmax);
	printf("      Plane{(%.3f, %.3f, %.3f),%.3f}\n", nx, ny, nz, d);
	printf("      Intersect = %s\n", intersect_string(result));


	DestroyBBox(&bb);
	DestroyPlane(&p);

	pause();

}

void caseAABBplane2() {
	float xmin, ymin, zmin, xmax, ymax, zmax;
	float nx, ny, nz, d;
	BBox *bb;
	plane *p;
	int result;

	// AABB
	printf("\n"); printf("AABB\n");
	printf("  Min. xmin, ymin, zmin:");
	scanf("%f, %f, %f", &xmin, &ymin, &zmin);
	printf("\n");
	printf("  Max. xmax, ymax, zmax:");
	scanf("%f, %f, %f", &xmax, &ymax, &zmax);

	// Plane
	printf("\n"); printf("Plane"); printf("  Normal nx, ny, nz:");
	scanf("%f, %f, %f", &nx, &ny, &nz);
	printf("\n"); printf("  d:");
	scanf("%f", &d);
	printf("\n");

	getchar();

	bb = CreateBBox(xmin, ymin, zmin, xmax, ymax, zmax);
	p = CreatePlane(nx, ny, nz, d);
	result = BBoxPlaneIntersect(bb, p);

	printf("\n");
	printf(" Result for AABB-plane:\n");
	printf("      BBox{(%.3f, %.3f, %.3f), (%.3f, %.3f, %.3f)}\n", xmin, ymin, zmin, xmax, ymax, zmax);
	printf("      Plane{(%.3f, %.3f, %.3f),%.3f}\n", nx, ny, nz, d);
	printf("      Intersect = %s\n", intersect_string(result));


	DestroyBBox(&bb);
	DestroyPlane(&p);

	pause();

}

void menuAABBplane() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect AABB-Plane\n");
		printf ("**************************************\n\n");

		printf(" 1. AABB{(0,0,0),(1,1,1)} and Plane{(4,-4,0),0}\n");
		printf(" 2. AABB{(-2,1,1),(-1,1,1)} and Plane{(4,-4,0),0}\n");
		printf(" 3. AABB{(-1.1,1,1),(-0.1,1,1)} and Plane{(4,-4,0),0}\n");
		printf(" 4. AABB{(2,1,1),(3,1,1)} and Plane{(4,-4,0),0}\n");
		printf(" 5. Enter data\n");
		printf(" 6. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseAABBplane1(0, 0, 0, 1, 1, 1, 4, -4, 0, 0);
			break;
		case 2:
			caseAABBplane1(-2, 1, 1, -1, 1, 1, 4, -4, 0, 0);
			break;
		case 3:
			caseAABBplane1(-1.1, 1, 1, -0.1, 1, 1, 4, -4, 0, 0);
			break;
		case 4:
			caseAABBplane1(2, 1, 1, 3, 1, 1, 4, -4, 0, 0);
			break;
		case 5:
			caseAABBplane2();
			break;
		case 6:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

//***************************************************
//***
//*** CASE INTERSECT between SPHERES
//***
//***************************************************

void caseSpheres1(float c1x, float c1y, float c1z, float r1, float c2x, float c2y, float c2z, float r2){
	BSphere *bs1, *bs2;
	int result;

	bs1 = CreateBSphere(c1x, c1y, c1z, r1);
	bs2 = CreateBSphere(c2x, c2y, c2z, r2);

	result = BSphereIntersectBSphere(bs1, bs2);

	printf("\n");
	printf(" Result for Sphere-Sphere:\n");
	printf("      Sphere1{(%.3f, %.3f, %.3f} , %.3f}\n", c1x, c1y, c1z, r1);
	printf("      Sphere2{(%.3f, %.3f, %.3f} , %.3f}\n", c2x, c2y, c2z, r2);
	printf("      Intersect = %s\n", intersect_string(result));

	DestroyBSphere(&bs1);
	DestroyBSphere(&bs2);

	pause();

}

void caseSpheres2() {
	float c1x, c1y, c1z, r1, c2x, c2y, c2z, r2;
	BSphere *bs1, *bs2;
	int result;

	printf("\n"); printf("Sphere 1\n"); printf("  Center cx, cy, cz:");
	scanf("%f, %f, %f", &c1x, &c1y, &c1z);
	printf("\n"); printf("  Radius r:");
	scanf("%f", &r1);
	printf("\n");

	printf("\n"); printf("Sphere 2\n"); printf("  Center cx, cy, cz:");
	scanf("%f, %f, %f", &c2x, &c2y, &c2z);
	printf("\n"); printf("  Radius r:");
	scanf("%f", &r2);
	printf("\n");

	getchar();

	bs1 = CreateBSphere(c1x, c1y, c1z, r1);
	bs2 = CreateBSphere(c2x, c2y, c2z, r2);

	result = BSphereIntersectBSphere(bs1, bs2);

	printf("\n");
	printf(" Result for Sphere-Sphere:\n");
	printf("      Sphere1{(%.3f, %.3f, %.3f} , %.3f}\n", c1x, c1y, c1z, r1);
	printf("      Sphere2{(%.3f, %.3f, %.3f} , %.3f}\n", c2x, c2y, c2z, r2);
	printf("      Intersect = %s\n", intersect_string(result));

	DestroyBSphere(&bs1);
	DestroyBSphere(&bs2);

	pause();
}

void menuSpheres() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect Sphere-Sphere\n");
		printf ("**************************************\n\n");

		printf(" 1. Sphere1{(0,0,0), 1.0} and Sphere2{(0.5,1.0,1.0),1.0}\n");
		printf(" 2. Sphere1{(0,0,0), 1.0} and Sphere2{(1.78,0.91,0.0),1.0}\n");
		printf(" 3. Sphere1{(0,0,0), 1.0} and Sphere2{(0.191,0.962,0.195),1.0}\n");
		printf(" 4. Sphere1{(0,0,0), 1.0} and Sphere2{(2.001,0.0,0.0),1.0}\n");
		printf(" 5. Enter data\n");
		printf(" 6. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseSpheres1(0.0, 0.0, 0.0, 1.0, 0.5, 1.0, 1.0, 1.0);
			break;
		case 2:
			caseSpheres1(0.0, 0.0, 0.0, 1.0, 1.78, 0.91, 0.0, 1.0);
			break;
		case 3:
			caseSpheres1(0.0, 0.0, 0.0, 1.0, 0.191, 0.962, 0.195, 1.0);
			break;
		case 4:
			caseSpheres1(0.0, 0.0, 0.0, 1.0, 2.001, 0.0, 0.0, 1.0);
			break;
		case 5:
			caseSpheres2();
			break;
		case 6:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

//***************************************************
//***
//*** CASE SPHERE-PLANE
//***
//***************************************************

void caseSpherePlane1 (float cx, float cy, float cz, float r, float nx, float ny, float nz, float d) {

	BSphere *bs;
	plane *p;
	int result;

	bs = CreateBSphere(cx, cy, cz, r);
	p = CreatePlane(nx, ny, nz, d);
	result = BSpherePlaneIntersect(bs, p);

	printf("\n");
	printf(" Result for Sphere-Plane:\n");
	printf("      Sphere{(%.3f, %.3f, %.3f} , %.3f}\n", cx, cy, cz, r);
	printf("      Plane{(%.3f, %.3f, %.3f} , %.3f}\n", nx, ny, nz, d);
	printf("      Intersect = %s\n", intersect_string(result));

	DestroyBSphere(&bs);
	DestroyPlane(&p);

	pause();

}

void caseSpherePlane2 (){

	float cx, cy, cz, r;
	float nx, ny, nz, d;

	BSphere *bs;
	plane *p;
	int result;

	// Sphere
	printf("\n"); printf("Sphere\n"); printf("  Center cx, cy, cz:");
	scanf("%f, %f, %f", &cx, &cy, &cz);
	printf("\n"); printf("  Radius r:");
	scanf("%f", &r);
	printf("\n");

	// Plane
	printf("\n"); printf("Plane"); printf("  Normal nx, ny, nz:");
	scanf("%f, %f, %f", &nx, &ny, &nz);
	printf("\n"); printf("  d:");
	scanf("%f", &d);
	printf("\n");

	getchar();

	bs = CreateBSphere(cx, cy, cz, r);
	p = CreatePlane(nx, ny, nz, d);
	result = BSpherePlaneIntersect(bs, p);

	printf("\n");
	printf(" Result for Sphere-Plane:\n");
	printf("      Sphere{(%.3f, %.3f, %.3f} , %.3f}\n", cx, cy, cz, r);
	printf("      Plane{(%.3f, %.3f, %.3f} , %.3f}\n", nx, ny, nz, d);
	printf("      Intersect = %s\n", intersect_string(result));

	DestroyBSphere(&bs);
	DestroyPlane(&p);

	pause();

}

void menuSpherePlane() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect Sphere-Plane\n");
		printf ("**************************************\n\n");

		printf(" 1. Sphere{(0,0,0), 1.0} and plane{(4,-4,0), 0}\n");
		printf(" 2. Sphere{(1.78,0.3,-0.5), 1.0} and plane{(4,-4,0), 0}\n");
		printf(" 3. Sphere{(2,0.3,-0.5), 1.0} and plane{(4,-4,0), 0}\n");
		printf(" 4. Sphere{(-1.5,0.3,0), 1.0} and plane{(4,-4,0), 0}\n");
		printf(" 5. Enter data\n");
		printf(" 6. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseSpherePlane1(0.0, 0.0, 0.0, 1.0, 4, -4, 0, 0);
			break;
		case 2:
			caseSpherePlane1(1.78, 0.3, -0.5, 1.0, 4, -4, 0, 0);
			break;
		case 3:
			caseSpherePlane1(2, 0.3, -0.5, 1.0, 4, -4, 0, 0);
			break;
		case 4:
			caseSpherePlane1(-1.5, 0.3, 0.0, 1.0, 4, -4, 0, 0);
			break;
		case 5:
			caseSpherePlane2();
			break;
		case 6:
			end = 1;
			break;

		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}


//***************************************************
//***
//*** CASE INTERSECT Sphere and AABB
//***
//***************************************************

void caseSphereAABB1(float cx, float cy, float cz, float r, float xmin, float ymin, float zmin, float xmax, float ymax, float zmax) {
	BSphere *bs;
	BBox *bb;
	int result;

	bs = CreateBSphere(cx, cy, cz, r);
	bb = CreateBBox(xmin, ymin, zmin, xmax, ymax, zmax);
	result = BSphereBBoxIntersect(bs, bb);

	printf("\n");
	printf(" Result for Sphere-AABB:\n");
	printf("      Sphere{(%.3f, %.3f, %.3f} , %.3f}\n", cx, cy, cz, r);
	printf("      BBox{(%.3f, %.3f, %.3f), (%.3f, %.3f, %.3f)}\n", xmin, ymin, zmin, xmax, ymax, zmax);
	printf("      Intersect = %s\n", intersect_string(result));

	DestroyBSphere(&bs);
	DestroyBBox(&bb);

	pause();

}

void caseSphereAABB2() {
	float xmin, ymin, zmin, xmax, ymax, zmax;
	float cx, cy, cz, r;
	BBox *bb;
	BSphere *bs;
	int result;

	// Sphere
	printf("\n"); printf("Sphere\n"); printf("  Center cx, cy, cz:");
	scanf("%f, %f, %f", &cx, &cy, &cz);
	printf("\n"); printf("  Radius r:");
	scanf("%f", &r);
	printf("\n");

	// AABB
	printf("\n"); printf("AABB\n");
	printf("  Min. xmin, ymin, zmin:");
	scanf("%f, %f, %f", &xmin, &ymin, &zmin);
	printf("\n");
	printf("  Max. xmax, ymax, zmax:");
	scanf("%f, %f, %f", &xmax, &ymax, &zmax);

	getchar();

	bs = CreateBSphere(cx, cy, cz, r);
	bb = CreateBBox(xmin, ymin, zmin, xmax, ymax, zmax);
	result = BSphereBBoxIntersect(bs, bb);

	printf("\n");
	printf(" Result for Sphere-AABB:\n");
	printf("      Sphere{(%.3f, %.3f, %.3f} , %.3f}\n", cx, cy, cz, r);
	printf("      BBox{(%.3f, %.3f, %.3f), (%.3f, %.3f, %.3f)}\n", xmin, ymin, zmin, xmax, ymax, zmax);
	printf("      Intersect = %s\n", intersect_string(result));

	DestroyBSphere(&bs);
	DestroyBBox(&bb);

	pause();
}

void menuSphereAABB() {
	int end = 0;
	int option;
	while (end == 0){
		system("clear");
		printf ("**************************************\n");
		printf ("*** Test intersect Sphere-AABB\n");
		printf ("**************************************\n\n");

		printf(" 1. Sphere{(0,0,0),1} and AABB{(0,0,0),(1,1,1)}\n");
		printf(" 2. Sphere{(0,0,0),1} and AABB{(-2,0,0),(-1,1,1)}\n");
		printf(" 3. Sphere{(0,0,0),1} and AABB{(-3,0,0),(-2,1,1)}\n");
		printf(" 4. Sphere{(0,0,0),1} and AABB{(0,0,0),(0.5,0.5,0.5)}\n");
		printf(" 5. Enter data\n");
		printf(" 6. Main menu\n\n");
		printf(" Enter your choice: ");
		scanf("%i", &option); getchar();

		switch(option) {
		case 1:
			caseSphereAABB1(0, 0, 0, 1, 0, 0, 0, 1, 1, 1);
			break;
		case 2:
			caseSphereAABB1(0, 0, 0, 1, -2, 0, 0, -1, 1, 1);
			break;
		case 3:
			caseSphereAABB1(0, 0, 0, 1, -3, 0, 0, -2, 1, 1);
			break;
		case 4:
			caseSphereAABB1(0, 0, 0, 1, 0, 0, 0, 0.5, 0.5, 0.5);
			break;
		case 5:
			caseSphereAABB2();
			break;
		case 6:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 6\n");
			break;
		}
	}
}

//***************************************************
//***
//*** MENU PRINCIPAL
//***
//***************************************************

void mainMenu() {
	int end = 0;
	int option;

	while (end == 0){
		system("clear");
		printf(" 1. Distance from point to  line\n");
		printf(" 2. Test intersect AABB-AABB\n");
		printf(" 3. Test intersect AABB-plane\n");
		printf(" 4. Test intersect Spheres\n");
		printf(" 5. Test intersect Sphere-plane\n");
		printf(" 6. Test intersect Sphere-AABB\n");
		printf(" 7. Exit\n\n");
		printf(" Enter your choice: ");
		scanf("%d", &option); getchar();

		switch(option) {
		case 1:
			menuLine();
			break;
		case 2:
			menuAABBs();
			break;
		case 3:
			menuAABBplane();
			break;
		case 4:
			menuSpheres();
			break;
		case 5:
			menuSpherePlane();
			break;
		case 6:
			menuSphereAABB();
			break;
		case 7:
			end = 1;
			break;
		default:
			printf("Please enter a number between 1 and 7\n");
			break;
		}
	}
}


int main() {
	mainMenu();
	return 0;
}
