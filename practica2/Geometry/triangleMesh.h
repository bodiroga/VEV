#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include <GL/glew.h>
#include <GL/glut.h>
#include "list.h"
#include "vector.h"
#include "trfm3D.h"
#include "material.h"
#include "bbox.h"
#include "glm.h"

#define AGA_TRM                   0
#define AGA_TRM_TEX               1

#define AGA_EDGE_NOBOUND         -1

typedef struct _hedge {

	int v; // start vertex
	int face; // Triangle left
	int pair; // Sibiling edge
	int next; // Next half-edge
} TMhedge;



typedef struct _TriangleMesh {

	int     type; // AGA_TRM, AGA_TRM_TEX. Default AGA_TRM

	//! one material (not owned)
	Material *materialFront;
	Material *materialBack;

	int     mat_is_transp;  // Wether the material is transparent

	//

	/*!
	  \name Coordinate of vertices
	*/
	//@{
	VectorF *vCoords; // Vertex coordinates. Number of distinct vertices: sizeVectorF(vcoords)/3 */
	// Must be >=3 (at least one triangle) */
	//@}

	/*!
	  \name Normals at vertives
	  \note NULL if there are no Surface Normals at vertices
	*/
	//@{
	VectorF *nCoords;   // Normal coordinates. NULL if no normals.
	//@}

	/*!
	  \name Texture coordinates at vertices
	  \note NULL if there are no Texture Coordinates at vertices
	*/

	VectorF  *texCoords;  // Vertex texture coord. NULL if no textures.

	/*!
	  \name Texture tangents at vertices
	  \note NULL if there are no Texture tangents at vertices
	*/
	//@{
	VectorF *tgtCoords;   // Tangent coordinates. NULL if no texture tangents.
	//@}

	/*!
	  \name Texture bitangents at vertives
	  \note NULL if there are no Texture bitangents at vertices
	*/
	//@{
	VectorF *btgtCoords;   // Bitangent coordinates. NULL if no texture bitangents.
	//@}

	VectorF *n_Coords;
	VectorI *n_Indices;

	VectorI *vIndices;   // Triangle vertex indices.
	                     // Number of triangles: size(vindices)/3

	VectorI *nIndices;   // Triangle normal indices. NULL if no normals.
	VectorI *texIndices; // Triangle texture indices. NULL if no textures.

	VectorI *tgtIndices;  // Triangle texture indices. NULL if no textures.
	VectorI *btgtIndices; // Triangle texture indices. NULL if no textures.

	int     vbo_uptodate; // whether the VAO/VBO are up-to-date
	// VBO
	GLuint  vbo_id; // Vertex Buffer Object id
	// VAO
	GLuint  vao_id; // Vertex Array Object

} TriangleMesh;


void DestroyTriangleMesh( TriangleMesh **theMesh );

/**
 * Creates an empty triangle Mesh
 *
 * @return the newly created Mesh
 */
TriangleMesh *CreateVoidTriangleMesh();

// number of trianglse
size_t NumTrianglesTMesh(TriangleMesh *thisMesh);
// number of vertices
size_t NumVerticesTMesh(TriangleMesh *thisMesh);
// number of normals
size_t NumNormalsTMesh(TriangleMesh *thisMesh);
// number of tex coords.
size_t NumTexCoordsTMesh(TriangleMesh *thisMesh);
// number of texture tangents
size_t NumT_TMesh(TriangleMesh *thisMesh);
// number of texture bitangents
size_t NumB_TMesh(TriangleMesh *thisMesh);
// number of texture normals
size_t NumN_TMesh(TriangleMesh *thisMesh);


// get vertex indices for triangle i
int *TriangVidxTMesh(TriangleMesh *thisMesh, size_t i);
// get normal indices for triangle i
int *TriangNidxTMesh(TriangleMesh *thisMesh, size_t i);
// get tex coords indices for triangle i
int *TriangTidxTMesh(TriangleMesh *thisMesh, size_t i);
// get texture tangent indices for triangle i
int *TriangTgt_idxTMesh(TriangleMesh *thisMesh, size_t i);
// get texture bitangent indices for triangle i
int *TriangBtgt_idxTMesh(TriangleMesh *thisMesh, size_t i);
// get texture normals indices for triangle i
int *TriangN_idxTMesh(TriangleMesh *thisMesh, size_t i);

// get coordinates of vertex i
float *GetVertexTMesh(TriangleMesh *thisMesh, size_t idx);
// get coordinates of normal i
float *GetNormalTMesh(TriangleMesh *thisMesh, size_t idx);
// get coordinates of texcoord i
float *GetTexCoord_TMesh(TriangleMesh *thisMesh, size_t idx);
// get coordinates of texture tangent i
float *GetT_TMesh(TriangleMesh *thisMesh, size_t idx);
// get coordinates of texture bitangent i
float *GetB_TMesh(TriangleMesh *thisMesh, size_t idx);
// get coordinates of texture normal i
float *GetN_TMesh(TriangleMesh *thisMesh, size_t idx);

// Add a vertex to the mesh. Return the index of the added vertex.
int AddPointTMesh(TriangleMesh *thisMesh, float px, float py, float pz);
// Add a normal to the mesh. Return the index of the added normal.
int AddNormalTMesh(TriangleMesh *thisMesh, float nx, float ny, float nz);
// Add a tex coord. to the mesh. Return the index of the added tex coord.
int AddTexCoord_TMesh(TriangleMesh *thisMesh, float s, float t);

// Add a triangle giving vertices. Normal are calculated.
// return the index of the newly created triangle.
int AddTriangleTMesh(TriangleMesh *thisMesh,
					 int P0, int P1, int P2);

// Add a triangle giving vertices and normals.
// return the index of the newly created triangle.
int AddTriangle_N_TMesh(TriangleMesh *thisMesh,
						int P0, int P1, int P2,
						int N0, int N1, int N2);

// Add a triangle giving vertices, normals and texture coordinates.
// return the index of the newly created triangle.
int AddTriangle_NT_TMesh(TriangleMesh *thisMesh,
						 int P0, int P1, int P2,
						 int N0, int N1, int N2,
						 int T0, int T1, int T2);

// Add a triangle giving vertices and texture coordinates. Normals are calculated.
// return the index of the newly created triangle.
int AddTriangle_T_TMesh(TriangleMesh *thisMesh,
						int P0, int P1, int P2,
						int T0, int T1, int T2);

/**
 * Create a list of triangle meshes from a wavefront (.obj) file
 *
 * @param DirName The directory of the file
 * @param FileName The wavefront file
 *
 * @return a list of triangle meshes
 */

list *CreateTMeshObj( char *DirName, char *FileName );

void AssignMaterialTMesh(TriangleMesh *thisMesh, Material *front, Material *back);

/**
 * Recalculate all normals to create a faceted object
 *
 * @param thisMesh the mesh
 */
void FacetedTMesh(TriangleMesh *thisMesh);

/**
 * Recalculate all normals to create a smooth object. Normals are calculated by
 * averaging facet normals.
 *
 * @param thisMesh the mesh
 */
void SmoothTMesh(TriangleMesh *thisMesh);

void IncBoxTMesh( TriangleMesh *thisMesh, float *xmin, float *xmax,
				  float *ymin, float *ymax, float *zmin, float *zmax );

void ApplyTrfmTMesh(TriangleMesh * mesh, trfm3D * trfm);

void PrintTMesh(TriangleMesh * mesh);

#endif
