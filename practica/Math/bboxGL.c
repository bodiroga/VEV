#include "bboxGL.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include "scene.h" // For rendering state
#include "materialManager.h"

static void init_opengl_vbo(BBox *thisBBox) {

	static float buffer[24];
	static GLubyte idx[] = {
		0, 1, 1, 2, 2, 3, 3, 0, // front ( 4 lines: {P0, P1} {P1, P2}, {P2, P3}, {P3, P0} )
		5, 4, 4, 7, 7, 6, 6, 5, // back
		3, 7, 2, 6, 1, 5, 0, 4, // edges between front and back
	};

	buffer[0] =  thisBBox->xMin; buffer[1]  = thisBBox->yMin; buffer[2]  = thisBBox->zMax; // P0
	buffer[3] =  thisBBox->xMax; buffer[4]  = thisBBox->yMin; buffer[5]  = thisBBox->zMax; // P1
	buffer[6] =  thisBBox->xMax; buffer[7]  = thisBBox->yMax; buffer[8]  = thisBBox->zMax; // P2
	buffer[9] =  thisBBox->xMin; buffer[10] = thisBBox->yMax; buffer[11] = thisBBox->zMax; // P3
	buffer[12] = thisBBox->xMin; buffer[13] = thisBBox->yMin; buffer[14] = thisBBox->zMin; // P4
	buffer[15] = thisBBox->xMax; buffer[16] = thisBBox->yMin; buffer[17] = thisBBox->zMin; // P5
	buffer[18] = thisBBox->xMax; buffer[19] = thisBBox->yMax; buffer[20] = thisBBox->zMin; // P6
	buffer[21] = thisBBox->xMin; buffer[22] = thisBBox->yMax; buffer[23] = thisBBox->zMin; // P7

	/* // free previous VBO/VAO */
	/* glDeleteBuffers(1, &thisBBox->vbo_id); */
	/* glDeleteBuffers(1, &thisBBox->idxvbo_id); */

	if (thisBBox->vbo_id == 0) {
		// create new VBO for vertices
		glGenBuffers(1, &thisBBox->vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, thisBBox->vbo_id);
		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(buffer),
					 NULL,
					 GL_STREAM_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, thisBBox->vbo_id);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buffer), &buffer[0]);
	// upload data to VBO
	// create new VBO for indices
	if (thisBBox->idxvbo_id == 0) {
		glGenBuffers(1, &thisBBox->idxvbo_id);
		// bind VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, thisBBox->idxvbo_id);
		// upload data to VBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					 sizeof(idx),
					 &idx[0],
					 GL_STATIC_DRAW);
	}
	thisBBox->vbo_uptodate = 1;
}


void DrawGLBBox(BBox * thisBBox) {

	// Variables for displaying the BBox

	RenderState *rs;

	if (thisBBox->vbo_uptodate == 0)
		init_opengl_vbo(thisBBox);

	rs = RenderStateScene();

	BeforeDrawShader(GetShaderProgramRS(rs));

	glPushAttrib(GL_POLYGON_BIT);

	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK , GL_LINE);

	// Drawing
	glBindBuffer(GL_ARRAY_BUFFER, thisBBox->vbo_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, thisBBox->idxvbo_id);
	// Attribute specification
	glEnableVertexAttribArray(0); // 0 attrib. for vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, 0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopAttrib();
}
