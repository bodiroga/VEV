#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "triangleMesh.h"
#include "glm.h"
#include "hash.h"
#include "tools.h"
#include "materialManager.h"
#include "textureManager.h"

// If triangle span
// Vertices: v (>2)
// Triangles: v - 2
// Edges: 2v + 1

// Calculate texture TBN (tangent, bitangent, normal) of a Tmesh
static void tangentTMesh(TriangleMesh *thisMesh);

// normal of a triangle
static void normalTriangle(const float v0[3], const float v1[3], const float v2[3], float n[3]);

// Tangent and bitangent of a triangle
static void tangentTriangle(const float v0[3], const float v1[3], const float v2[3],
							const float uv0[2], const float uv1[2], const float uv2[2],
							float t[3], float b[3], float n[3]);

static void renormalizeN(TriangleMesh *theMesh);

void DestroyTriangleMesh( TriangleMesh **theMesh ) {

	TriangleMesh *thisMesh;

	thisMesh = *theMesh;
	if (!thisMesh) return;

	if (thisMesh->vCoords) {
		DestroyVectorF(&(thisMesh->vCoords));
		DestroyVectorI(&(thisMesh->vIndices));
	}

	if (thisMesh->nCoords) {
		DestroyVectorF(&(thisMesh->nCoords));
		DestroyVectorI(&(thisMesh->nIndices));
	}

	if (thisMesh->texCoords) {
		DestroyVectorF(&(thisMesh->texCoords));
		DestroyVectorI(&(thisMesh->texIndices));
	}

	if (thisMesh->tgtCoords) {
		DestroyVectorF(&(thisMesh->tgtCoords));
		DestroyVectorI(&(thisMesh->tgtIndices));
	}

	if (thisMesh->btgtCoords) {
		DestroyVectorF(&(thisMesh->btgtCoords));
		DestroyVectorI(&(thisMesh->btgtIndices));
	}

	if (thisMesh->n_Coords) {
		DestroyVectorF(&(thisMesh->n_Coords));
		DestroyVectorI(&(thisMesh->n_Indices));
	}

	// reclaim openGL buffers
	if (thisMesh->vbo_id)
		glDeleteBuffers(1, &thisMesh->vbo_id);
	if (thisMesh->vao_id)
		glDeleteVertexArrays(1, &thisMesh->vao_id);

	free(thisMesh);
	*theMesh = NULL;
}

TriangleMesh *CreateVoidTriangleMesh() {
	TriangleMesh *newMesh;

	newMesh = malloc( sizeof(*newMesh) * 1);

	newMesh->type  = AGA_TRM;

	newMesh->materialFront = SceneDefaultMaterial();
	newMesh->materialBack = SceneDefaultMaterial();

	// vertices
	newMesh->vCoords = NULL;
	newMesh->vIndices = NULL;
	// normales
	newMesh->nCoords = NULL;
	newMesh->nIndices = NULL;
	// textura
	newMesh->texCoords = NULL;
	newMesh->texIndices = NULL;

	// texture tangentes
	newMesh->tgtCoords = NULL;
	newMesh->tgtIndices = NULL;
	// texture bitangentes
	newMesh->btgtCoords = NULL;
	newMesh->btgtIndices = NULL;
	// texture normales
	newMesh->n_Coords = NULL;
	newMesh->n_Indices = NULL;

	newMesh->mat_is_transp = 0;
	newMesh->vbo_uptodate = 1;

	return newMesh;
}

///////

size_t NumTrianglesTMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->vIndices) return 0;
	return sizeVectorI(thisMesh->vIndices) / 3;
}

size_t NumVerticesTMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->vCoords) return 0;
	return sizeVectorF(thisMesh->vCoords) / 3;
}

size_t NumNormalsTMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->nCoords) return 0;
	return sizeVectorF(thisMesh->nCoords) / 3;
}

size_t NumTexCoordsTMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->texCoords) return 0;
	return sizeVectorF(thisMesh->texCoords) / 2;
}

size_t NumT_TMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->tgtCoords) return 0;
	return sizeVectorF(thisMesh->tgtCoords) / 3;
}

size_t NumB_TMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->btgtCoords) return 0;
	return sizeVectorF(thisMesh->btgtCoords) / 3;
}

size_t NumN_TMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->n_Coords) return 0;
	return sizeVectorF(thisMesh->n_Coords) / 3;
}

///////

int *TriangVidxTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->vIndices) return NULL;
	return atPtrVectorI(thisMesh->vIndices, 3 * idx);
}

int *TriangNidxTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->nIndices) return NULL;
	return atPtrVectorI(thisMesh->nIndices, 3 * idx);
}

int *TriangTidxTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->texIndices) return NULL;
	return atPtrVectorI(thisMesh->texIndices, 3 * idx);
}

int *TriangTgt_idxTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->tgtIndices) return NULL;
	return atPtrVectorI(thisMesh->tgtIndices, 3 * idx);
}

int *TriangBtgt_idxTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->btgtIndices) return NULL;
	return atPtrVectorI(thisMesh->btgtIndices, 3 * idx);
}

int *TriangN_idxTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->n_Indices) return NULL;
	return atPtrVectorI(thisMesh->n_Indices, 3 * idx);
}

///////

float *GetVertexTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->vCoords) return NULL;
	return atPtrVectorF(thisMesh->vCoords, 3 * idx);
}

float *GetNormalTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->nCoords) return NULL;
	return atPtrVectorF(thisMesh->nCoords, 3 * idx);
}

float *GetTexCoord_TMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->texCoords) return NULL;
	return atPtrVectorF(thisMesh->texCoords, 2 * idx);
}

float *GetT_TMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->tgtCoords) return NULL;
	return atPtrVectorF(thisMesh->tgtCoords, 3 * idx);
}

float *GetB_TMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->btgtCoords) return NULL;
	return atPtrVectorF(thisMesh->btgtCoords, 3 * idx);
}

float *GetN_TMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->n_Coords) return NULL;
	return atPtrVectorF(thisMesh->n_Coords, 3 * idx);
}

///////

int AddPointTMesh(TriangleMesh *thisMesh, float px, float py, float pz) {
	if(!thisMesh->vCoords) thisMesh->vCoords = CreateVectorF();
	pushVectorF(thisMesh->vCoords, px);
	pushVectorF(thisMesh->vCoords, py);
	pushVectorF(thisMesh->vCoords, pz);
	return sizeVectorF(thisMesh->vCoords) / 3 - 1;
}

int AddNormalTMesh(TriangleMesh *thisMesh, float nx, float ny, float nz) {
	if(!thisMesh->nCoords) thisMesh->nCoords = CreateVectorF();
	pushVectorF(thisMesh->nCoords, nx);
	pushVectorF(thisMesh->nCoords, ny);
	pushVectorF(thisMesh->nCoords, nz);
	return sizeVectorF(thisMesh->nCoords) / 3 - 1;
}

int AddTexCoord_TMesh(TriangleMesh *thisMesh, float s, float t) {
	if(!thisMesh->texCoords) thisMesh->texCoords = CreateVectorF();
	pushVectorF(thisMesh->texCoords, s);
	pushVectorF(thisMesh->texCoords, t);
	thisMesh->type |= AGA_TRM_TEX;
	return sizeVectorF(thisMesh->texCoords) / 2 - 1;
};

static void addTriangleVertices(TriangleMesh *thisMesh,
								int P0, int P1, int P2) {
	if (!thisMesh->vIndices) thisMesh->vIndices = CreateVectorI();
	pushVectorI(thisMesh->vIndices, P0);
	pushVectorI(thisMesh->vIndices, P1);
	pushVectorI(thisMesh->vIndices, P2);
	thisMesh->vbo_uptodate = 0;
}

static void addTriangleNormals(TriangleMesh *thisMesh,
							   int N0, int N1, int N2) {
	if (!thisMesh->nIndices) thisMesh->nIndices = CreateVectorI();
	pushVectorI(thisMesh->nIndices, N0);
	pushVectorI(thisMesh->nIndices, N1);
	pushVectorI(thisMesh->nIndices, N2);
	thisMesh->vbo_uptodate = 0;
}

static void addTriangleTexCoords(TriangleMesh *thisMesh, int T0, int T1, int T2) {
	if (!thisMesh->texIndices) thisMesh->texIndices = CreateVectorI();
	pushVectorI(thisMesh->texIndices, T0);
	pushVectorI(thisMesh->texIndices, T1);
	pushVectorI(thisMesh->texIndices, T2);
	// Tiene textura: Calculamos TBN
	tangentTMesh(thisMesh);
	thisMesh->vbo_uptodate = 0;
}

int AddTriangleTMesh(TriangleMesh *thisMesh, int P0, int P1, int P2) {
	float N[3];
	int Nidx;
	addTriangleVertices(thisMesh, P0, P1, P2);
	// Calculate face normal
	normalTriangle(GetVertexTMesh(thisMesh, P0),
				   GetVertexTMesh(thisMesh, P1),
				   GetVertexTMesh(thisMesh, P2),
				   N);
	Nidx = AddNormalTMesh(thisMesh, N[0], N[1], N[2]);
	addTriangleNormals(thisMesh, Nidx, Nidx, Nidx);
	return sizeVectorI(thisMesh->vIndices) / 3 - 1;
}

int AddTriangle_N_TMesh(TriangleMesh *thisMesh,	int P0, int P1, int P2,
						int N0, int N1, int N2) {
	addTriangleVertices(thisMesh, P0, P1, P2);
	addTriangleNormals(thisMesh, N0, N1, N2);
	return sizeVectorI(thisMesh->vIndices) / 3 - 1;
}

int AddTriangle_NT_TMesh(TriangleMesh *thisMesh, int P0, int P1, int P2,
						 int N0, int N1, int N2,
						 int T0, int T1, int T2) {
	addTriangleVertices(thisMesh, P0, P1, P2);
	addTriangleNormals(thisMesh, N0, N1, N2);
	addTriangleTexCoords(thisMesh, T0, T1, T2);
	// Tiene textura: Calculamos TBN
	tangentTMesh(thisMesh);
	return sizeVectorI(thisMesh->vIndices) / 3 - 1;
}

int AddTriangle_T_TMesh(TriangleMesh *thisMesh, int P0, int P1, int P2,
						int T0, int T1, int T2) {
	AddTriangleTMesh(thisMesh, P0, P1, P2);
	addTriangleTexCoords(thisMesh, T0, T1, T2);
	// Tiene textura: Calculamos TBN
	tangentTMesh(thisMesh);
	return sizeVectorI(thisMesh->vIndices) / 3 - 1;
}

/*!

  Creates a new triangle mesh given a model previously readed by glm.

  \param n: number of triangles
  \param triangles: array of n triangles
  \param model: glm model (where the actual vertices, normals, textures coordinates are)
  \param front: fronfacing materiak
  \param back: backfacing materiak
  \param type: type of mesh (AGA_TRM, AGA_TRM_NORMAL, AGA_TRM_FACET_NORMAL, AGA_TRM_TEX)

  \return a pointer to the new created triangle mesh

*/

// Create a triangle mesh given a glm model
TriangleMesh *CreateTriangleMeshGlm(int numtriangles, GLuint *triangles, GLMmodel *model,
									Material *front, Material *back, int type, int hasNormal) {
	TriangleMesh *newMesh;
	GLMtriangle *T;

	int  i, m;
	// for calculating normals
	int Nidx;
	float N[3];
	int *P;

	float *P0, *P1, *P2;
	float *Tex0, *Tex1, *Tex2;
	float vT[3];
	float vB[3];
	float vN[3];
	int hasTexture;

	newMesh = malloc( sizeof(*newMesh) * 1);
	newMesh->type  = type;
	newMesh->materialFront = front;
	newMesh->materialBack = back;

	newMesh->mat_is_transp = IsTranspMaterial(front);

	newMesh->vCoords  = NULL;
	newMesh->vIndices = NULL;
	newMesh->nCoords  = NULL;
	newMesh->nIndices = NULL;
	newMesh->texCoords = NULL;
	newMesh->texIndices = NULL;

	// TBN
	newMesh->tgtCoords  = NULL;
	newMesh->tgtIndices = NULL;
	newMesh->btgtCoords  = NULL;
	newMesh->btgtIndices = NULL;
	newMesh->n_Coords  = NULL;
	newMesh->n_Indices = NULL;

	newMesh->vCoords  = CreateVectorFv(&(model->vertices[3]), model->numvertices * 3);
	newMesh->vIndices = CreateVectorIN(3 * numtriangles);

	if (hasNormal) {
		newMesh->nCoords  = CreateVectorFv(&(model->normals[3]), model->numnormals * 3);
		newMesh->nIndices = CreateVectorIN(3 * numtriangles);
	}

	if (newMesh->type & AGA_TRM_TEX) {
		newMesh->texCoords  = CreateVectorFv(&(model->texcoords[2]), model->numtexcoords * 2);
		newMesh->texIndices = CreateVectorIN(3 * numtriangles);
	}

	for (i = 0; i < numtriangles; ++i) {
		T = &(model->triangles[triangles[i]]);
		pushVectorI(newMesh->vIndices, T->vindices[0] - 1);
		pushVectorI(newMesh->vIndices, T->vindices[1] - 1);
		pushVectorI(newMesh->vIndices, T->vindices[2] - 1);
		if (hasNormal) {
			pushVectorI(newMesh->nIndices, T->nindices[0] - 1);
			pushVectorI(newMesh->nIndices, T->nindices[1] - 1);
			pushVectorI(newMesh->nIndices, T->nindices[2] - 1);
		}
		if (newMesh->type & AGA_TRM_TEX) {
			pushVectorI(newMesh->texIndices, T->tindices[0] - 1);
			pushVectorI(newMesh->texIndices, T->tindices[1] - 1);
			pushVectorI(newMesh->texIndices, T->tindices[2] - 1);
		}
	}

	if(!hasNormal) {
		// TODO generate face normals.
		for (i = 0; i < numtriangles; ++i) {
			P = TriangVidxTMesh(newMesh, i);
			// Calculate face normal
			normalTriangle(GetVertexTMesh(newMesh, *P),
						   GetVertexTMesh(newMesh, *(P + 1)),
						   GetVertexTMesh(newMesh, *(P + 2)),
						   N);
			Nidx = AddNormalTMesh(newMesh, N[0], N[1], N[2]);
			addTriangleNormals(newMesh, Nidx, Nidx, Nidx);
		}
	}

	// Calculate TBN
	if (newMesh->type & AGA_TRM_TEX) {
		newMesh->tgtCoords    = CreateVectorF();
		newMesh->tgtIndices   = CreateVectorI();
		newMesh->btgtCoords   = CreateVectorF();
		newMesh->btgtIndices  = CreateVectorI();
		newMesh->n_Coords     = CreateVectorF();
		newMesh->n_Indices    = CreateVectorI();
		tangentTMesh(newMesh);
	}

	// OpenGL VBO init
	newMesh->vbo_id = 0;
	newMesh->vao_id = 0;

	newMesh->vbo_uptodate = 0;

	return newMesh;
}

static Material *create_mat(GLMmodel * m, GLMmaterial * mat, char *DirName, char *library) {
	Material *newMaterial;
	char *image;
	Texture *newTexture;

	newMaterial = SceneRegisterMaterial(library, mat->name);

	SetAmbientMaterial(newMaterial, &(mat->ambient[0]));
	SetAlphaMaterial(newMaterial, mat->diffuse[3]);
	SetDiffuseMaterial(newMaterial, &(mat->diffuse[0]));
	if (mat->specular[0] != 0.0f ||
		mat->specular[1] != 0.0f ||
		mat->specular[2] != 0.0f)
		SetSpecularMaterial(newMaterial, &(mat->specular[0]), mat->shininess);

	if (mat->emmissive[0] != 0.0f ||
		mat->emmissive[1] != 0.0f ||
		mat->emmissive[2] != 0.0f)
		SetEmissionMaterial(newMaterial, &(mat->emmissive[0]));

	if (mat->texturemap) {
		image = MergeStrings( DirName, mat->texturemap );
		newTexture = SceneRegisterTexture(image);
		SetTextureMaterial (newMaterial, newTexture );
		DestroyString(&image);
	}

	if (mat->bumpmap) {
		image = MergeStrings( DirName, mat->bumpmap );
		newTexture = SceneRegisterBumpTexture(image);
		SetBumpMapTextureMaterial(newMaterial, newTexture );
		DestroyString(&image);
	}
	return newMaterial;
}

list *CreateTMeshObj(char *DirName, char *FileName ) {
	GLMmodel *m;
	GLMgroup *g;

	Material *mat;
	TriangleMesh *surface;
	list *surfaces;

	int type;
	char *library_name;

	library_name = MergeStrings( DirName, FileName );
	m = glmReadOBJ(library_name);

	surfaces = CreateVoidList();

	g = &(m->groups[0]);
	while(g) {
		if (g->numtriangles) {
			type = AGA_TRM;
			if (g->material) {
				mat = create_mat(m, &(m->materials[g->material]), DirName, library_name);
				if (m->texcoords && HasTextureMaterial(mat)) type |= AGA_TRM_TEX;
			} else {
				mat = SceneDefaultMaterial();
			}
			// Create and store the surface (triangleMesh)
			surface = CreateTriangleMeshGlm(g->numtriangles,
											g->triangles, m, mat, mat, type, m->numnormals);
			AddLast(surfaces, surface);
		}
		g = g->next;
	}

	glmDelete(m);
	DestroyString(&library_name);
	return surfaces;
}


void AddVertsNormals(TriangleMesh *thisMesh, float *xx, float *yy, float *zz);
//void AddVertsTexture( polygon *thisPolygon, float *u, float *v );


static void unormalTriangle(const float v0[3], const float v1[3], const float v2[3], float n[3]) {
	float   u[3];
	float   v[3];

	// u = v1 - v0
	u[0] = v1[0] - v0[0];
	u[1] = v1[1] - v0[1];
	u[2] = v1[2] - v0[2];

	// v = v2 - v0
	v[0] = v2[0] - v0[0];
	v[1] = v2[1] - v0[1];
	v[2] = v2[2] - v0[2];

	// n = u x v
	crossVV(&n[0], &n[1], &n[2], u[0], u[1], u[2], v[0], v[1], v[2]);
}

static void normalTriangle(const float v0[3], const float v1[3], const float v2[3], float n[3]) {
	unormalTriangle(v0, v1, v2, n);
	VectorNormalize(&n[0], &n[1], &n[2]);
}

// renormalize all mesh normals
static void renormalizeN(TriangleMesh *thisMesh) {
	int i, m;
	float *N;
	for(i = 0, m = NumNormalsTMesh(thisMesh); i < m; i++) {
		N = GetNormalTMesh(thisMesh, i);
		VectorNormalize(&N[0], &N[1], &N[2]);
	}
	thisMesh->vbo_uptodate = 0;
}

static void tangentTMesh(TriangleMesh *thisMesh) {
	int i, n_idx, triang_n;
	int *T;
	float *P0, *P1, *P2, *Tex0, *Tex1, *Tex2;
	float vT[3];
	float vB[3];
	float vN[3];

	triang_n = NumTrianglesTMesh(thisMesh);

	// Destroy previous tangents and bitangents
	DestroyVectorI(&(thisMesh->tgtIndices));
	DestroyVectorF(&(thisMesh->tgtCoords));
	thisMesh->tgtIndices = CreateVectorIN(triang_n * 3);
	// only one tgt vector per triangle
	thisMesh->tgtCoords  = CreateVectorFN(triang_n * 3);

	DestroyVectorI(&(thisMesh->btgtIndices));
	DestroyVectorF(&(thisMesh->btgtCoords));
	thisMesh->btgtIndices = CreateVectorIN(triang_n * 3);
	// only one btgt vector per triangle
	thisMesh->btgtCoords  = CreateVectorFN(triang_n * 3);

	DestroyVectorI(&(thisMesh->n_Indices));
	DestroyVectorF(&(thisMesh->n_Coords));
	thisMesh->n_Indices = CreateVectorIN(triang_n * 3);
	thisMesh->n_Coords  = CreateVectorFN(triang_n * 3);

	n_idx = 0; // tangent index
	for(i = 0; i < triang_n; ++i) {
		// Vertices
		T = TriangVidxTMesh(thisMesh, i);
		P0 = GetVertexTMesh(thisMesh, T[0]);
		P1 = GetVertexTMesh(thisMesh, T[1]);
		P2 = GetVertexTMesh(thisMesh, T[2]);
		// Textures
		T = TriangTidxTMesh(thisMesh, i);
		Tex0 = GetTexCoord_TMesh(thisMesh, T[0]);
		Tex1 = GetTexCoord_TMesh(thisMesh, T[1]);
		Tex2 = GetTexCoord_TMesh(thisMesh, T[2]);

		tangentTriangle(P0, P1, P2, Tex0, Tex1, Tex2, &vT[0], &vB[0], &vN[0]);

		pushVectorF(thisMesh->tgtCoords, vT[0]);
		pushVectorF(thisMesh->tgtCoords, vT[1]);
		pushVectorF(thisMesh->tgtCoords, vT[2]);

		pushVectorI(thisMesh->tgtIndices, n_idx);
		pushVectorI(thisMesh->tgtIndices, n_idx);
		pushVectorI(thisMesh->tgtIndices, n_idx);

		pushVectorF(thisMesh->btgtCoords, vB[0]);
		pushVectorF(thisMesh->btgtCoords, vB[1]);
		pushVectorF(thisMesh->btgtCoords, vB[2]);

		pushVectorI(thisMesh->btgtIndices, n_idx);
		pushVectorI(thisMesh->btgtIndices, n_idx);
		pushVectorI(thisMesh->btgtIndices, n_idx);

		pushVectorF(thisMesh->n_Coords, vN[0]);
		pushVectorF(thisMesh->n_Coords, vN[1]);
		pushVectorF(thisMesh->n_Coords, vN[2]);

		pushVectorI(thisMesh->n_Indices, n_idx);
		pushVectorI(thisMesh->n_Indices, n_idx);
		pushVectorI(thisMesh->n_Indices, n_idx);

		n_idx++;
	}
	thisMesh->vbo_uptodate = 0;
}

void FacetedTMesh(TriangleMesh *thisMesh) {
	int i, n_idx, triang_n;
	int *T;
	float *P0, *P1, *P2;
	float N[3];

	triang_n = NumTrianglesTMesh(thisMesh);
	// Destroy previous normals
	if (sizeVectorI(thisMesh->nIndices)) {
		DestroyVectorI(&(thisMesh->nIndices));
		DestroyVectorF(&(thisMesh->nCoords));
		thisMesh->nIndices = CreateVectorIN(triang_n * 3);
		thisMesh->nCoords  = CreateVectorFN(triang_n * 3);
	}

	n_idx = 0; // normal index
	for(i = 0; i < triang_n; ++i) {
		T = TriangVidxTMesh(thisMesh, i);
		P0 = GetVertexTMesh(thisMesh, T[0]);
		P1 = GetVertexTMesh(thisMesh, T[1]);
		P2 = GetVertexTMesh(thisMesh, T[2]);
		normalTriangle(P0, P1, P2, &N[0]);
		pushVectorF(thisMesh->nCoords, N[0]);
		pushVectorF(thisMesh->nCoords, N[1]);
		pushVectorF(thisMesh->nCoords, N[2]);

		pushVectorI(thisMesh->nIndices, n_idx);
		pushVectorI(thisMesh->nIndices, n_idx);
		pushVectorI(thisMesh->nIndices, n_idx);
		n_idx++;
	}
	thisMesh->vbo_uptodate = 0;
}

static void tangentTriangle(const float v0[3], const float v1[3], const float v2[3],
							const float uv0[2], const float uv1[2], const float uv2[2],
							float t[3], float b[3], float n[3]) {
	float Q1[3];
	float Q2[3];
	float s1, t1;
	float s2, t2;
	float r;

	// Q1 = v1 - v0
	Q1[0] = v1[0] - v0[0];
	Q1[1] = v1[1] - v0[1];
	Q1[2] = v1[2] - v0[2];
	// Q2 = v2 - v0
	Q2[0] = v2[0] - v0[0];
	Q2[1] = v2[1] - v0[1];
	Q2[2] = v2[2] - v0[2];
	// (s1,t1) = uv1 - uv0
	s1  = uv1[0] - uv0[0];
	t1  = uv1[1] - uv0[1];
	// (s2,t2) = uv2 - uv0
	s2  = uv2[0] - uv0[0];
	t2  = uv2[1] - uv0[1];

	r = 1.0f / (s1 * t2 - s2 * t1);
	// tangent
	t[0] = (Q1[0] * t2 - Q2[0] * t1)*r;
	t[1] = (Q1[1] * t2 - Q2[1] * t1)*r;
	t[2] = (Q1[2] * t2 - Q2[2] * t1)*r;
	VectorNormalize(&t[0], &t[1], &t[2]);

	// bitangent
	b[0] = (Q2[0] * s1 - Q1[0] * s2)*r;
	b[1] = (Q2[1] * s1 - Q1[1] * s2)*r;
	b[2] = (Q2[2] * s1 - Q1[2] * s2)*r;
	VectorNormalize(&b[0], &b[1], &b[2]);

	// normal = tangent x bitangent
	crossVV(&n[0], &n[1], &n[2], t[0], t[1], t[2], b[0], b[1], b[2]);
}

// taken from:
// http://www.iquilezles.org/www/articles/normals/normals.htm

void SmoothTMesh(TriangleMesh *thisMesh) {
	int   i, m, n_idx, triang_n;
	int   *T;
	float *P0, *P1, *P2;
	float N[3];
	float *NN;

	// Destroy previous normals
	if (sizeVectorI(thisMesh->nIndices)) {
		DestroyVectorI(&(thisMesh->nIndices));
		DestroyVectorF(&(thisMesh->nCoords));
		// As many different normals as vertices in the mesh
		thisMesh->nIndices = CreateVectorIN(sizeVectorI(thisMesh->vIndices));
		thisMesh->nCoords = CreateVectorFN(sizeVectorF(thisMesh->vCoords));
	}
	// Init all vertex normals
	for(i = 0, m = sizeVectorF(thisMesh->vCoords); i < m; i++) {
		pushVectorF(thisMesh->nCoords, 0.0);
		pushVectorF(thisMesh->nCoords, 0.0);
		pushVectorF(thisMesh->nCoords, 0.0);
	}
	// Copy normal indices from vertex indices
	for(i = 0, m = sizeVectorI(thisMesh->vIndices); i < m; i++) {
		pushVectorI(thisMesh->nIndices, atVectorI(thisMesh->vIndices, i));
	}
	// Accumulate face normals to compute vertex normals
	triang_n = NumTrianglesTMesh(thisMesh);
	for(i = 0; i < triang_n; ++i) {
		T = TriangVidxTMesh(thisMesh, i);
		P0 = GetVertexTMesh(thisMesh, T[0]);
		P1 = GetVertexTMesh(thisMesh, T[1]);
		P2 = GetVertexTMesh(thisMesh, T[2]);
		unormalTriangle(P0, P1, P2,
						&N[0]);
		//T = TriangNidxTMesh(thisMesh, i);
		NN = GetNormalTMesh(thisMesh, T[0]);
		NN[0] += N[0]; NN[1] += N[1]; NN[2] += N[2];
		NN = GetNormalTMesh(thisMesh, T[1]);
		NN[0] += N[0]; NN[1] += N[1]; NN[2] += N[2];
		NN = GetNormalTMesh(thisMesh, T[2]);
		NN[0] += N[0]; NN[1] += N[1]; NN[2] += N[2];
	}
	// Normalize vertex normals
	renormalizeN(thisMesh);
}

void AssignMaterialTMesh(TriangleMesh *thisMesh, Material *front, Material *back) {
	thisMesh->materialFront = front;
	thisMesh->materialBack = back;
}

void IncBoxTMesh( TriangleMesh *thisMesh, float *xmin, float *xmax,
				  float *ymin, float *ymax, float *zmin, float *zmax ) {
	int i;
	int m = sizeVectorF(thisMesh->vCoords) / 3;
	for (i=0; i<m; ++i) {
		*xmin = fmin(atVectorF(thisMesh->vCoords, 3*i), *xmin);
		*xmax = fmax(atVectorF(thisMesh->vCoords, 3*i), *xmax);
		*ymin = fmin(atVectorF(thisMesh->vCoords, 3*i + 1), *ymin);
		*ymax = fmax(atVectorF(thisMesh->vCoords, 3*i + 1), *ymax);
		*zmin = fmin(atVectorF(thisMesh->vCoords, 3*i + 2), *zmin);
		*zmax = fmax(atVectorF(thisMesh->vCoords, 3*i + 2), *zmax);
	}
}

void ApplyTrfmTMesh(TriangleMesh * mesh, trfm3D * trfm) {
	int i;
	float *v;
	int points_n, normals_n;

	points_n = NumVerticesTMesh(mesh);
	for (i = 0; i < points_n; ++i) {
		v = GetVertexTMesh(mesh, i);
		Trfm3DTransformPoint(trfm, &v[0], &v[1], &v[2]);
	}
	normals_n = NumNormalsTMesh(mesh);
	for (i= 0; i < normals_n; ++i) {
		v = GetNormalTMesh(mesh, i);
		TransformNormalTrfm3D(trfm, &v[0], &v[1], &v[2]);
	}
	renormalizeN(mesh);
	mesh->vbo_uptodate = 0;
}

void PrintVertex1 (TriangleMesh * mesh, int idxV, int idxN) {
	float *T;

	T = GetVertexTMesh(mesh, idxV);
	printf("V%d(%.2f, %.2f, %.2f) ",  idxV, T[0], T[1], T[2]);
	T = GetNormalTMesh(mesh, idxN);
	printf("N%d(%.2f, %.2f, %.2f)\n", idxN, T[0], T[1], T[2]);
}

void PrintVertex2 (TriangleMesh * mesh, int idxV, int idxN, int idxT, int idx_T, int idx_B, int idx_N) {
	float *T;

	PrintVertex1(mesh, idxV, idxN);
	if (mesh->type & AGA_TRM_TEX) {
		T = GetTexCoord_TMesh(mesh, idxT);
		printf("Tex%d(%.2f, %.2f)\n", idxT, T[0], T[1]);
		T = GetT_TMesh(mesh, idx_T);
		printf("   T%d(%.2f, %.2f, %.2f) ", idx_T, T[0], T[1], T[2]);
		T = GetB_TMesh(mesh, idx_B);
		printf("B%d(%.2f, %.2f, %.2f) ",    idx_B, T[0], T[1], T[2]);
		T = GetN_TMesh(mesh, idx_N);
		printf("N%d(%.2f, %.2f, %.2f)\n",    idx_T, T[0], T[1], T[2]);
	}
}

void PrintTMesh(TriangleMesh * mesh) {
	int *TrV, *TrN, *TrT, *TrTT, *TrBT, *TrNT;
	int i;
	size_t idx;
	int triang_n;

	triang_n = NumTrianglesTMesh(mesh);
	printf("Triangles:%d type:%d\n", triang_n, mesh->type);
	for(i = 0; i < triang_n; ++i) {
		printf("Triangle: %d\n", i);
		TrV = TriangVidxTMesh(mesh, i);
		TrN = TriangNidxTMesh(mesh, i);
		if (mesh->type & AGA_TRM_TEX) {
			TrT  = TriangTidxTMesh(mesh, i);
			TrTT = TriangTgt_idxTMesh(mesh, i);
			TrBT = TriangBtgt_idxTMesh(mesh, i);
			TrNT = TriangN_idxTMesh(mesh, i);
			PrintVertex2(mesh, TrV[0], TrN[0], TrT[0], TrTT[0], TrBT[0], TrNT[0]);
			PrintVertex2(mesh, TrV[1], TrN[1], TrT[1], TrTT[1], TrBT[1], TrNT[1]);
			PrintVertex2(mesh, TrV[2], TrN[2], TrT[2], TrTT[2], TrBT[2], TrNT[2]);
		} else {
			PrintVertex1(mesh, TrV[0], TrN[0]);
			PrintVertex1(mesh, TrV[1], TrN[1]);
			PrintVertex1(mesh, TrV[2], TrN[2]);
		}
	}
	if (mesh->materialFront) {
		printf("Material:");
		PrintMaterial(mesh->materialFront);
	}
	printf("End Triangles:\n");
}
