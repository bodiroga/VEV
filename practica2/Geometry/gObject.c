#include "tools.h"
#include "gObject.h"
#include "triangleMeshGL.h"
#include <stdio.h>

void    DestroyGObject( GObject **theGObject ) {

	TriangleMesh    *oneMesh;
	GObject  *thisGObject;

	thisGObject = *theGObject;
	if( ! thisGObject ) return;

	oneMesh = StartLoop( thisGObject->meshes );
	while( oneMesh ) {
		DestroyTriangleMesh( &oneMesh );
		oneMesh = GetNext( thisGObject->meshes );
	}
	DestroyList( &thisGObject->meshes );

	oneMesh = StartLoop( thisGObject->meshes_transp );
	while( oneMesh ) {
		DestroyTriangleMesh( &oneMesh );
		oneMesh = GetNext( thisGObject->meshes_transp );
	}
	DestroyList( &thisGObject->meshes_transp );

	free( thisGObject );
	*theGObject = NULL;
}

GObject *CreateVoidGObject() {

	GObject     *newGObject;

	newGObject = malloc( sizeof(*newGObject) * 1 );
	newGObject->meshes = CreateVoidList();
	newGObject->meshes_transp = CreateVoidList();
	newGObject->container = CreateVoidBBox();

	return newGObject;
}

void    AddPatchGObject( GObject *thisGObject, TriangleMesh *oneMesh ) {

	if(oneMesh->mat_is_transp) {
		AddLast( thisGObject->meshes_transp, oneMesh );
	} else {
		AddLast( thisGObject->meshes, oneMesh );
	}
}

GObject  *CreateFromWavefront( char *DirName, char *FileName ) {

	GObject     *newGObject;
	TriangleMesh       *oneMesh;
	list               *auxlist;

	newGObject = CreateVoidGObject();
	auxlist = CreateTMeshObj(DirName, FileName);
	oneMesh = StartLoop( auxlist );
	while( oneMesh ) {
		AddPatchGObject(newGObject, oneMesh);
		oneMesh = GetNext( auxlist );
	}
	SetContainerGObject( newGObject );
	return newGObject;
}

void    DrawGObject( GObject *thisGObject ) {
	TriangleMesh  *oneMesh;

	oneMesh = StartLoop( thisGObject->meshes );
	while( oneMesh ) {
		DrawGLTriangleMesh( oneMesh);
		oneMesh = GetNext( thisGObject->meshes );
	}

	oneMesh = StartLoop( thisGObject->meshes_transp );
	while( oneMesh ) {
		DrawGLTriangleMesh( oneMesh);
		oneMesh = GetNext( thisGObject->meshes_transp );
	}

}

void    DrawOpaqueMesh( GObject *thisGObject ) {
	TriangleMesh  *oneMesh;

	oneMesh = StartLoop( thisGObject->meshes );
	while( oneMesh ) {
		DrawGLTriangleMesh( oneMesh);
		oneMesh = GetNext( thisGObject->meshes );
	}
}

void    DrawTransparentMesh( GObject *thisGObject ) {
	TriangleMesh  *oneMesh;

	oneMesh = StartLoop( thisGObject->meshes_transp );
	while( oneMesh ) {
		DrawGLTriangleMesh( oneMesh);
		oneMesh = GetNext( thisGObject->meshes_transp );
	}
}


// apply a trfm to all meshes
void ApplyTrfmGObject(GObject *thisGObject, trfm3D * trfm) {
	TriangleMesh  *oneMesh;

	oneMesh = StartLoop( thisGObject->meshes );
	while( oneMesh ) {
		ApplyTrfmTMesh( oneMesh, trfm);
		oneMesh = GetNext( thisGObject->meshes );
	}

	oneMesh = StartLoop( thisGObject->meshes_transp );
	while( oneMesh ) {
		ApplyTrfmTMesh( oneMesh, trfm);
		oneMesh = GetNext( thisGObject->meshes_transp );
	}
}

// Set gObject to be faceted.
void FacetedGObject(GObject *thisGObject) {
	TriangleMesh  *oneMesh;

	oneMesh = StartLoop( thisGObject->meshes );
	while( oneMesh ) {
		FacetedTMesh(oneMesh);
		oneMesh = GetNext( thisGObject->meshes );
	}

	oneMesh = StartLoop( thisGObject->meshes_transp );
	while( oneMesh ) {
		FacetedTMesh(oneMesh);
		oneMesh = GetNext( thisGObject->meshes_transp );
	}
}

// Set gObject to be smooth.
void SmoothGObject(GObject *thisGObject) {
	TriangleMesh  *oneMesh;

	oneMesh = StartLoop( thisGObject->meshes );
	while( oneMesh ) {
		SmoothTMesh(oneMesh);
		oneMesh = GetNext( thisGObject->meshes );
	}

	oneMesh = StartLoop( thisGObject->meshes_transp );
	while( oneMesh ) {
		SmoothTMesh(oneMesh);
		oneMesh = GetNext( thisGObject->meshes_transp );
	}
}

int  GetNbrPolygons( GObject *thisGObject ) {

	TriangleMesh  *oneMesh;
	int      numPols;

	numPols = 0;
	oneMesh = StartLoop( thisGObject->meshes );
	while( oneMesh ) {
		numPols += NumTrianglesTMesh(oneMesh);
		oneMesh = GetNext( thisGObject->meshes );
	}

	oneMesh = StartLoop( thisGObject->meshes_transp );
	while( oneMesh ) {
		numPols += NumTrianglesTMesh(oneMesh);
		oneMesh = GetNext( thisGObject->meshes_transp );
	}

	return numPols;
}

void SetContainerGObject( GObject *thisGObject ) {

	TriangleMesh  *oneMesh;
	float    xmin, xmax, ymin, ymax, zmin, zmax;
	if( ! thisGObject ) return;

	xmin = ymin = zmin =   FLT_MAX;
	xmax = ymax = zmax = - FLT_MAX;
	oneMesh = StartLoop( thisGObject->meshes );
	while( oneMesh ) {
		IncBoxTMesh(  oneMesh, &xmin, &xmax, &ymin, &ymax, &zmin, &zmax );
		oneMesh = GetNext( thisGObject->meshes );
	}
	oneMesh = StartLoop( thisGObject->meshes_transp );
	while( oneMesh ) {
		IncBoxTMesh(  oneMesh, &xmin, &xmax, &ymin, &ymax, &zmin, &zmax );
		oneMesh = GetNext( thisGObject->meshes_transp );
	}
	thisGObject->container->xMax = xmax;
	thisGObject->container->xMin = xmin;
	thisGObject->container->yMax = ymax;
	thisGObject->container->yMin = ymin;
	thisGObject->container->zMax = zmax;
	thisGObject->container->zMin = zmin;
}

void PrintGObject( GObject *thisGObject) {

	TriangleMesh *oneMesh;
	printf("*** Opaque meshes:\n");
	oneMesh = StartLoop( thisGObject->meshes );
	while( oneMesh ) {
		PrintTMesh( oneMesh);
		oneMesh = GetNext( thisGObject->meshes );
	}

	printf("*** Transparent meshes:\n");
	oneMesh = StartLoop( thisGObject->meshes_transp );
	while( oneMesh ) {
		PrintTMesh( oneMesh);
		oneMesh = GetNext( thisGObject->meshes_transp );
	}
}
