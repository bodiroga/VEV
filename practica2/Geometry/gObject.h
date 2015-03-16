#ifndef GEOMETRY_OBJECT_H
#define GEOMETRY_OBJECT_H

/**
 * @file   gObject.h
 * @author Aitor Soroa
 * @date   Mon Nov 28 12:27:03 2005
 *
 * @brief A geometry object is a list of meshes and a Bounding volume
 *
 * Geometry objects don't understand about local transformations etc. They
 * only store topological info.
 *
 */

#include <stdio.h>
#include "list.h"
#include "bbox.h"
#include "triangleMesh.h"


typedef struct _gObject {

	list     *meshes;
	list     *meshes_transp;
	BBox     *container;
} GObject;

void      DestroyGObject( GObject **theGObject );
GObject  *CreateVoidGObject();

/**
 * Create a geometry object given a wavefront file
 *
 * @param DirName The directory where the obj is
 * @param FileName The file name
 *
 * @return the new created geometry object
 */

GObject  *CreateFromWavefront( char *DirName, char *FileName );

// needs to know which materials are available

void AddPatchGObject( GObject *thisGObject, TriangleMesh *oneMesh );

// Draw the geometry object using current modelview matrix

void DrawGObject( GObject *thisGObject );

// Draw opaque meshes of the geometry object using current modelview matrix

void DrawOpaqueMesh( GObject *thisGObject );

// Draw transparent meshes of the geometry object using current modelview matrix

void DrawTransparentMesh( GObject *thisGObject );

// apply a trfm to all meshes
void ApplyTrfmGObject(GObject *thisGObject, trfm3D * trfm);

// Set gObject to be faceted.
void FacetedGObject(GObject *thisGObject);

// Set gObject to be smooth.
void SmoothGObject(GObject *thisGObject);

int  GetNbrPolygons( GObject *thisGObject );

void SetContainerGObject( GObject *thisGObject );

void PrintGObject( GObject *thisGObject);

#endif /* GEOMETRY_OBJECT_H */
