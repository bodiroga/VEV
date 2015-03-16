#include "triangleMeshGL.h"
#include "scene.h" // For rendering state
#include "shader.h"

// This module renders a triangleMesh using openGL as a backend.

static void init_opengl_vbo(TriangleMesh * newMesh);

typedef struct _vbo_vertex {
	GLfloat v[3];
	GLfloat n[3];
	GLfloat t[2];
	GLfloat tbn_t[3];
	GLfloat tbn_b[3];
	GLfloat tbn_n[3];
} Vbo_vertex;

#define VBO_BUFFER_OFFSET(i) ((char *)NULL + (i))

static void init_opengl_vbo(TriangleMesh * newMesh) {

	Vbo_vertex *buffer;
	int i, j, m;
	int vIndices_n;
	size_t idx;
	int buffer_size;
	float *v;
	int *T;

	// free previous VBO/VAO
	glDeleteBuffers(1, &newMesh->vbo_id);
	glDeleteVertexArrays(1, &newMesh->vao_id);

	vIndices_n = sizeVectorI(newMesh->vIndices);
	buffer_size = vIndices_n * sizeof(*buffer);
	buffer = malloc(buffer_size);

	for(i = 0; i < vIndices_n; ++i) {
		idx = atVectorI(newMesh->vIndices, i);
		v = GetVertexTMesh(newMesh, idx);
		buffer[i].v[0] = v[0];
		buffer[i].v[1] = v[1];
		buffer[i].v[2] = v[2];
	}

	for(i = 0; i < vIndices_n; ++i) {
		idx = atVectorI(newMesh->nIndices, i);
		v = GetNormalTMesh(newMesh, idx);
		buffer[i].n[0] = v[0];
		buffer[i].n[1] = v[1];
		buffer[i].n[2] = v[2];
	}
	if(newMesh->type & AGA_TRM_TEX) {
		for(i = 0; i < vIndices_n; ++i) {
			// textures
			idx = atVectorI(newMesh->texIndices, i);
			v = GetTexCoord_TMesh(newMesh, idx);
			buffer[i].t[0] = v[0];
			buffer[i].t[1] = v[1];
			// TBN: tangents
			idx = atVectorI(newMesh->tgtIndices, i);
			v = GetT_TMesh(newMesh, idx);
			buffer[i].tbn_t[0] = v[0];
			buffer[i].tbn_t[1] = v[1];
			buffer[i].tbn_t[2] = v[2];
			// TBN: bitangents
			idx = atVectorI(newMesh->btgtIndices, i);
			v = GetB_TMesh(newMesh, idx);
			buffer[i].tbn_b[0] = v[0];
			buffer[i].tbn_b[1] = v[1];
			buffer[i].tbn_b[2] = v[2];
			// TBN: normals
			idx = atVectorI(newMesh->n_Indices, i);
			v = GetN_TMesh(newMesh, idx);
			buffer[i].tbn_n[0] = v[0];
			buffer[i].tbn_n[1] = v[1];
			buffer[i].tbn_n[2] = v[2];
		}
	}

	glGenVertexArrays(1, &newMesh->vao_id);
	// bind new VAO to the conetxt
	glBindVertexArray(newMesh->vao_id);
	// create new VBO
	glGenBuffers(1, &newMesh->vbo_id);
	// bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, newMesh->vbo_id);
	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER,
				 buffer_size,
				 buffer,
				 GL_STATIC_DRAW);
	// Attribute specification
	// 0-11 (vertex-position) 12-23 (normal) 24-31 (texture coord) 32-43 (TBN tangent)
	// 44-55 (TBN bitangent) 56-67 (TBN normal)
	glEnableVertexAttribArray(0); // 0 attrib. for vertex position (3 x 4 = 12)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1); // 1 attrib. for vertex normal (3 x 4 = 12)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(12));
	if(newMesh->type & AGA_TRM_TEX) {
		glEnableVertexAttribArray(2); // 2 attrib. for texture coord. (2 x 4 = 8)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(24));
		glEnableVertexAttribArray(3); // 3 attrib. for TBN tangent coord.  (3 x 4 = 12)
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(32));
		glEnableVertexAttribArray(4); // 4 attrib. for TBN bitangent coord. (3 x 4 = 12)
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(44));
		glEnableVertexAttribArray(5); // 5 attrib. for TBN bitangent coord. (3 x 4 = 12)
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(56));
	}
	// unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// unbind VAO
	glBindVertexArray(0);
	free(buffer);
	newMesh->vbo_uptodate = 1;
}

#undef VBO_BUFFER_OFFSET

void DrawGLTriangleMesh (TriangleMesh * thisMesh) {

	RenderState *rs;
	ShaderProgram *shaderProgram;

	if (!NumVerticesTMesh(thisMesh)) return;

	if (thisMesh->vbo_uptodate == 0)
		init_opengl_vbo(thisMesh);

	rs = RenderStateScene();
	shaderProgram = GetShaderProgramRS(rs);

	if (thisMesh->materialFront) {
		SetFrontMaterialRS(rs, thisMesh->materialFront);
	}

	if (thisMesh->materialBack) {
		SetBackMaterialRS(rs, thisMesh->materialBack);
	}

	BeforeDrawShader(shaderProgram);

	glBindVertexArray(thisMesh->vao_id);
	glDrawArrays(GL_TRIANGLES,
				 0,
				 sizeVectorI(thisMesh->vIndices));
	glBindVertexArray(0);
}
