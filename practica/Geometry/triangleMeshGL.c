#include "triangleMeshGL.h"
#include "scene.h" // For rendering state
#include "shader.h"

// This module renders a triangleMesh using openGL as a backend.

static void init_opengl_vbo(TriangleMesh * newMesh);

typedef struct _vbo_vertex {
	GLfloat v[3];
	GLfloat n[3];
	GLfloat t[2];
} Vbo_vertex;

#define VBO_BUFFER_OFFSET(i) ((char *)NULL + (i))

static void init_opengl_vbo(TriangleMesh * newMesh) {

	Vbo_vertex *buffer;
	int i, j, m;
	int vindices_n;
	size_t idx;
	int buffer_size;
	float *v;
	int *T;

	// free previous VBO/VAO
	glDeleteBuffers(1, &newMesh->vbo_id);
	glDeleteVertexArrays(1, &newMesh->vao_id);

	vindices_n = sizeVectorI(newMesh->vindices);
	buffer_size = vindices_n * sizeof(*buffer);
	buffer = malloc(buffer_size);

	for(i = 0; i < vindices_n; ++i) {
		idx = atVectorI(newMesh->vindices, i);
		v = GetVertexTMesh(newMesh, idx);
		buffer[i].v[0] = v[0];
		buffer[i].v[1] = v[1];
		buffer[i].v[2] = v[2];
	}

	for(i = 0; i < vindices_n; ++i) {
		idx = atVectorI(newMesh->nindices, i);
		v = GetNormalTMesh(newMesh, idx);
		buffer[i].n[0] = v[0];
		buffer[i].n[1] = v[1];
		buffer[i].n[2] = v[2];
	}

	if(newMesh->type & AGA_TRM_TEX) {
		for(i = 0; i < vindices_n; ++i) {
			idx = atVectorI(newMesh->texindices, i);
			v = GetTCoordTMesh(newMesh, idx);
			buffer[i].t[0] = v[0];
			buffer[i].t[1] = v[1];
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
	glEnableVertexAttribArray(0); // 0 attrib. for vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
						  sizeof(Vbo_vertex),
						  VBO_BUFFER_OFFSET(0));

	glEnableVertexAttribArray(2); // 2 attrib. for vertex normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
						  sizeof(Vbo_vertex),
						  VBO_BUFFER_OFFSET(12));
	if(newMesh->type & AGA_TRM_TEX) {
		glEnableVertexAttribArray(8); // 8 attrib. for texture coord.
		glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE,
							  sizeof(Vbo_vertex),
							  VBO_BUFFER_OFFSET(24));
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
				 sizeVectorI(thisMesh->vindices));
	glBindVertexArray(0);
}
