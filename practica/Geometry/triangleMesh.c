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

static void normalTriangle(const float v0[3],
						   const float v1[3],
						   const float v2[3],
						   float n[3]);

static void renormalizeN(TriangleMesh *theMesh);

void DestroyTriangleMesh( TriangleMesh **theMesh ) {

	TriangleMesh *thisMesh;

	thisMesh = *theMesh;
	if (!thisMesh) return;

	if (thisMesh->vcoords) {
		DestroyVectorF(&(thisMesh->vcoords));
		DestroyVectorI(&(thisMesh->vindices));
	}

	if (thisMesh->ncoords) {
		DestroyVectorF(&(thisMesh->ncoords));
		DestroyVectorI(&(thisMesh->nindices));
	}

	if (thisMesh->tex_coords) {
		DestroyVectorF(&(thisMesh->tex_coords));
		DestroyVectorI(&(thisMesh->texindices));
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

	newMesh->vcoords = NULL;
	newMesh->vindices = NULL;
	newMesh->ncoords = NULL;
	newMesh->nindices = NULL;
	newMesh->tex_coords = NULL;
	newMesh->texindices = NULL;

	newMesh->mat_is_transp = 0;
	newMesh->vbo_uptodate = 1;

	return newMesh;
}

size_t NumTrianglesTMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->vindices) return 0;
	return sizeVectorI(thisMesh->vindices) / 3;
};

size_t NumVerticesTMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->vcoords) return 0;
	return sizeVectorF(thisMesh->vcoords) / 3;
}

size_t NumNormalsTMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->ncoords) return 0;
	return sizeVectorF(thisMesh->ncoords) / 3;
}

size_t NumTexCoordsTMesh(TriangleMesh *thisMesh) {
	if (!thisMesh->tex_coords) return 0;
	return sizeVectorF(thisMesh->tex_coords) / 2;
}

int *TriangVidxTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->vindices) return NULL;
	return atPtrVectorI(thisMesh->vindices, 3 * idx);
}

int *TriangNidxTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->nindices) return NULL;
	return atPtrVectorI(thisMesh->nindices, 3 * idx);
}

int *TriangTidxTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->texindices) return NULL;
	return atPtrVectorI(thisMesh->texindices, 3 * idx);
}

float *GetVertexTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->vcoords) return NULL;
	return atPtrVectorF(thisMesh->vcoords, 3 * idx);
}

float *GetNormalTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->ncoords) return NULL;
	return atPtrVectorF(thisMesh->ncoords, 3 * idx);
}

float *GetTCoordTMesh(TriangleMesh *thisMesh, size_t idx) {
	if (!thisMesh->tex_coords) return NULL;
	return atPtrVectorF(thisMesh->tex_coords, 2 * idx);
}

int AddPointTMesh(TriangleMesh *thisMesh, float px, float py, float pz) {
	if(!thisMesh->vcoords) thisMesh->vcoords = CreateVectorF();
	pushVectorF(thisMesh->vcoords, px);
	pushVectorF(thisMesh->vcoords, py);
	pushVectorF(thisMesh->vcoords, pz);
	return sizeVectorF(thisMesh->vcoords) / 3 - 1;
}

int AddNormalTMesh(TriangleMesh *thisMesh, float nx, float ny, float nz) {
	if(!thisMesh->ncoords) thisMesh->ncoords = CreateVectorF();
	pushVectorF(thisMesh->ncoords, nx);
	pushVectorF(thisMesh->ncoords, ny);
	pushVectorF(thisMesh->ncoords, nz);
	return sizeVectorF(thisMesh->ncoords) / 3 - 1;
}

int AddTexCoordTMesh(TriangleMesh *thisMesh, float s, float t) {
	if(!thisMesh->tex_coords) thisMesh->tex_coords = CreateVectorF();
	pushVectorF(thisMesh->tex_coords, s);
	pushVectorF(thisMesh->tex_coords, t);
	thisMesh->type |= AGA_TRM_TEX;
	return sizeVectorF(thisMesh->tex_coords) / 2 - 1;
};

static void addTriangleVertices(TriangleMesh *thisMesh,
								int P0, int P1, int P2) {
	if (!thisMesh->vindices) thisMesh->vindices = CreateVectorI();
	pushVectorI(thisMesh->vindices, P0);
	pushVectorI(thisMesh->vindices, P1);
	pushVectorI(thisMesh->vindices, P2);
	thisMesh->vbo_uptodate = 0;
}

static void addTriangleNormals(TriangleMesh *thisMesh,
							   int N0, int N1, int N2) {
	if (!thisMesh->nindices) thisMesh->nindices = CreateVectorI();
	pushVectorI(thisMesh->nindices, N0);
	pushVectorI(thisMesh->nindices, N1);
	pushVectorI(thisMesh->nindices, N2);
	thisMesh->vbo_uptodate = 0;
}

static void addTriangleTcords(TriangleMesh *thisMesh,
							  int T0, int T1, int T2) {
	if (!thisMesh->texindices) thisMesh->texindices = CreateVectorI();
	pushVectorI(thisMesh->texindices, T0);
	pushVectorI(thisMesh->texindices, T1);
	pushVectorI(thisMesh->texindices, T2);
	thisMesh->vbo_uptodate = 0;
}

int AddTriangleTMesh(TriangleMesh *thisMesh,
					 int P0, int P1, int P2) {
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
	return sizeVectorI(thisMesh->vindices) / 3 - 1;
}

int AddTriangle_N_TMesh(TriangleMesh *thisMesh,
						int P0, int P1, int P2,
						int N0, int N1, int N2) {
	addTriangleVertices(thisMesh, P0, P1, P2);
	addTriangleNormals(thisMesh, N0, N1, N2);
	return sizeVectorI(thisMesh->vindices) / 3 - 1;
}

int AddTriangle_NT_TMesh(TriangleMesh *thisMesh,
						 int P0, int P1, int P2,
						 int N0, int N1, int N2,
						 int T0, int T1, int T2) {
	addTriangleVertices(thisMesh, P0, P1, P2);
	addTriangleNormals(thisMesh, N0, N1, N2);
	addTriangleTcords(thisMesh, T0, T1, T2);
	return sizeVectorI(thisMesh->vindices) / 3 - 1;
}

int AddTriangle_T_TMesh(TriangleMesh *thisMesh,
						int P0, int P1, int P2,
						int T0, int T1, int T2) {
	AddTriangleTMesh(thisMesh, P0, P1, P2);
	addTriangleTcords(thisMesh, T0, T1, T2);
	return sizeVectorI(thisMesh->vindices) / 3 - 1;
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
									Material *front, Material *back, int type, int hasNormals) {

	TriangleMesh     *newMesh;

	GLMtriangle *T;

	int  i, m;

	newMesh = malloc( sizeof(*newMesh) * 1);

	newMesh->type  = type;

	newMesh->materialFront = front;
	newMesh->materialBack = back;

	newMesh->mat_is_transp = IsTranspMaterial(front);

	newMesh->vcoords = NULL;
	newMesh->vindices = NULL;
	newMesh->ncoords = NULL;
	newMesh->nindices = NULL;
	newMesh->tex_coords = NULL;
	newMesh->texindices = NULL;

	newMesh->vcoords = CreateVectorFv(&(model->vertices[3]), model->numvertices * 3);
	newMesh->vindices = CreateVectorIN(3 * numtriangles);

	if (hasNormals) {
		newMesh->ncoords = CreateVectorFv(&(model->normals[3]), model->numnormals * 3);
		newMesh->nindices = CreateVectorIN(3 * numtriangles);
	}

	if (newMesh->type & AGA_TRM_TEX) {
		newMesh->tex_coords = CreateVectorFv(&(model->texcoords[2]), model->numtexcoords * 2);
		newMesh->texindices = CreateVectorIN(3 * numtriangles);
	}

	for (i = 0; i < numtriangles; ++i) {
		T = &(model->triangles[triangles[i]]);
		pushVectorI(newMesh->vindices, T->vindices[0] - 1);
		pushVectorI(newMesh->vindices, T->vindices[1] - 1);
		pushVectorI(newMesh->vindices, T->vindices[2] - 1);
		if (hasNormals) {
			pushVectorI(newMesh->nindices, T->nindices[0] - 1);
			pushVectorI(newMesh->nindices, T->nindices[1] - 1);
			pushVectorI(newMesh->nindices, T->nindices[2] - 1);
		}
		if (newMesh->type & AGA_TRM_TEX) {
			pushVectorI(newMesh->texindices, T->tindices[0] - 1);
			pushVectorI(newMesh->texindices, T->tindices[1] - 1);
			pushVectorI(newMesh->texindices, T->tindices[2] - 1);
		}
	}

	if(!hasNormals) {
		// TODO generate face normals.
	}

	// OpenGL VBO init
	newMesh->vbo_id = 0;
	newMesh->vao_id = 0;

	newMesh->vbo_uptodate = 0;

	//PrintTMesh(newMesh);
	return newMesh;
}

static Material *create_mat(GLMmodel * m,
							GLMmaterial * mat,
							char *DirName,
							char *library) {

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
	return newMaterial;
}

list *CreateTMeshObj( char *DirName, char *FileName ) {


	GLMmodel      *m;
	GLMgroup      *g;

	Material      *mat;
	TriangleMesh  *surface;
	list *surfaces;

	int type;
	char *library_name;

	library_name = MergeStrings( DirName, FileName );
	m=glmReadOBJ(library_name);

	surfaces = CreateVoidList();

	g = &(m->groups[0]);
	while(g) {
		if (g->numtriangles) {
			type = AGA_TRM;
			if (m->texcoords) type |= AGA_TRM_TEX;
			if (g->material) {
				mat = create_mat(m, &(m->materials[g->material]), DirName, library_name);
			} else {
				mat = SceneDefaultMaterial();
			}
			// Create and store the surface (triangleMesh)
			surface = CreateTriangleMeshGlm(g->numtriangles, g->triangles, m, mat, mat, type, m->numnormals);
			AddLast(surfaces, surface);
		}
		g = g->next;
	}
	glmDelete(m);
	DestroyString(&library_name);
	return surfaces;
}


void AddVertsNormals( TriangleMesh *thisMesh, float *xx, float *yy, float *zz );
//void AddVertsTexture( polygon *thisPolygon, float *u, float *v );


static void unormalTriangle(const float v0[3],
							const float v1[3],
							const float v2[3],
							float n[3]) {

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

	crossVV(&n[0], &n[1], &n[2],
			u[0], u[1], u[2],
			v[0], v[1], v[2]);
}

static void normalTriangle(const float v0[3],
						   const float v1[3],
						   const float v2[3],
						   float n[3]) {

	unormalTriangle(v0, v1, v2, n);
	VectorNormalize( &n[0], &n[1], &n[2]);
}

// renormalize all mesh normals
static void renormalizeN(TriangleMesh *thisMesh) {
	int i, m;
	float *N;
	for(i = 0, m = NumNormalsTMesh(thisMesh); i < m; i++) {
		N = GetNormalTMesh(thisMesh, i);
		VectorNormalize( &N[0], &N[1], &N[2]);
	}
	thisMesh->vbo_uptodate = 0;
}


void FacetedTMesh(TriangleMesh *thisMesh) {
	int   i, n_idx, triang_n;
	int   *T;
	float *P0, *P1, *P2;
	float N[3];

	triang_n = NumTrianglesTMesh(thisMesh);
	// Destroy previous normals
	if (sizeVectorI(thisMesh->nindices)) {
		DestroyVectorI(&(thisMesh->nindices));
		DestroyVectorF(&(thisMesh->ncoords));
		thisMesh->nindices = CreateVectorIN(triang_n * 3);
		thisMesh->ncoords = CreateVectorFN(triang_n * 3);
	}

	n_idx = 0; // normal index
	for(i = 0; i < triang_n; ++i) {
		T = TriangVidxTMesh(thisMesh, i);
		P0 = GetVertexTMesh(thisMesh, T[0]);
		P1 = GetVertexTMesh(thisMesh, T[1]);
		P2 = GetVertexTMesh(thisMesh, T[2]);
		normalTriangle(P0, P1, P2,
					   &N[0]);
		pushVectorF(thisMesh->ncoords, N[0]);
		pushVectorF(thisMesh->ncoords, N[1]);
		pushVectorF(thisMesh->ncoords, N[2]);

		pushVectorI(thisMesh->nindices, n_idx);
		pushVectorI(thisMesh->nindices, n_idx);
		pushVectorI(thisMesh->nindices, n_idx);
		n_idx++;
	}
	thisMesh->vbo_uptodate = 0;
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
	if (sizeVectorI(thisMesh->nindices)) {
		DestroyVectorI(&(thisMesh->nindices));
		DestroyVectorF(&(thisMesh->ncoords));
		// As many different normals as vertices in the mesh
		thisMesh->nindices = CreateVectorIN(sizeVectorI(thisMesh->vindices));
		thisMesh->ncoords = CreateVectorFN(sizeVectorF(thisMesh->vcoords));
	}
	// Init all vertex normals
	for(i = 0, m = sizeVectorF(thisMesh->vcoords); i < m; i++) {
		pushVectorF(thisMesh->ncoords, 0.0);
		pushVectorF(thisMesh->ncoords, 0.0);
		pushVectorF(thisMesh->ncoords, 0.0);
	}
	// Copy normal indices from vertex indices
	for(i = 0, m = sizeVectorI(thisMesh->vindices); i < m; i++) {
		pushVectorI(thisMesh->nindices, atVectorI(thisMesh->vindices, i));
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
	int m = sizeVectorF(thisMesh->vcoords) / 3;
	for (i=0; i<m; ++i) {
		*xmin = fmin(atVectorF(thisMesh->vcoords, 3*i), *xmin);
		*xmax = fmax(atVectorF(thisMesh->vcoords, 3*i), *xmax);
		*ymin = fmin(atVectorF(thisMesh->vcoords, 3*i + 1), *ymin);
		*ymax = fmax(atVectorF(thisMesh->vcoords, 3*i + 1), *ymax);
		*zmin = fmin(atVectorF(thisMesh->vcoords, 3*i + 2), *zmin);
		*zmax = fmax(atVectorF(thisMesh->vcoords, 3*i + 2), *zmax);
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

void PrintTMesh(TriangleMesh * mesh) {

	int i;
	float *V;
	float *N;
	float *T;
	int *TrV, *TrN, *TrT;
	size_t idx;
	int triang_n;

	TrT = NULL;

	triang_n = NumTrianglesTMesh(mesh);

	printf("Triangles:%d type:%d\n", triang_n, mesh->type);

	for(i = 0; i < triang_n; ++i) {
		printf("Triangle: %d\n", i);
		TrV = TriangVidxTMesh(mesh, i);
		TrN = TriangNidxTMesh(mesh, i);
		if (mesh->type & AGA_TRM_TEX) TrT = TriangTidxTMesh(mesh, i);

		idx = 0;
		V = GetVertexTMesh(mesh, TrV[idx]);
		N = GetNormalTMesh(mesh, TrN[idx]);
		if (mesh->type & AGA_TRM_TEX) T = GetTCoordTMesh(mesh, TrT[idx]);

		printf("V%d(%.2f, %.2f, %.2f) ", TrV[idx], V[0], V[1], V[2]);
		printf("N%d(%.2f, %.2f, %.2f) ", TrN[idx], N[0], N[1], N[2]);
		if (mesh->type & AGA_TRM_TEX) printf("T%d(%.2f, %.2f) ", TrT[idx], T[0], T[1]);
		printf("\n");

		idx = 1;
		V = GetVertexTMesh(mesh, TrV[idx]);
		N = GetNormalTMesh(mesh, TrN[idx]);
		if (mesh->type & AGA_TRM_TEX) T = GetTCoordTMesh(mesh, TrT[idx]);

		printf("V%d(%.2f, %.2f, %.2f) ", TrV[idx], V[0], V[1], V[2]);
		printf("N%d(%.2f, %.2f, %.2f) ", TrN[idx], N[0], N[1], N[2]);
		if (mesh->type & AGA_TRM_TEX) printf("T%d(%.2f, %.2f) ", TrT[idx], T[0], T[1]);
		printf("\n");

		idx = 2;
		V = GetVertexTMesh(mesh, TrV[idx]);
		N = GetNormalTMesh(mesh, TrN[idx]);
		if (mesh->type & AGA_TRM_TEX) T = GetTCoordTMesh(mesh, TrT[idx]);

		printf("V%d(%.2f, %.2f, %.2f) ", TrV[idx], V[0], V[1], V[2]);
		printf("N%d(%.2f, %.2f, %.2f) ", TrN[idx], N[0], N[1], N[2]);
		if (mesh->type & AGA_TRM_TEX) printf("T%d(%.2f, %.2f) ", TrT[idx], T[0], T[1]);
		printf("\n");
	}
}
