#include <string.h>
#include "skybox.h"
#include "gObjectManager.h"
#include "textureManager.h"
#include "shaderManager.h"
#include "scene.h"
#include "trfm3D.h"

static char buffer[4096];

static char* cmap_faces[6] = { "xpos", "xneg", "ypos", "yneg", "zpos", "zneg" };

static char *advance_p(char *o, size_t n,
					   char *overflow) {
	char *p;
	p = o;
	while(n--) {
		if (++p > overflow) {
			fprintf(stderr, "[E] generate names: directory name too long\n");
			exit(1);
		}
	}
	return p;
}

static char **generate_names(char *dirname) {

	static char *ext=".jpg";
	size_t i;
	size_t ldn;      // len of dirname
	size_t ln;     // len of name
	size_t le;     // len of extension
	char *p = NULL; // start of next name
	static char *names[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

	char *overflow = buffer + 4096;

	ldn = strlen(dirname);
	if(!ldn) goto end;
	if(dirname[ldn - 1] == '/') ldn--;
	le = strlen(ext);
	p = &buffer[0];
	for(i = 0; i < 6; i++) {
		names[i] = p;
		strncpy(p, dirname, ldn);
		p = advance_p(p, ldn, overflow);
		*p = '/';
		p++;
		ln = strlen(cmap_faces[i]);
		strncpy(p, cmap_faces[i], ln);
		p = advance_p(p, ln, overflow);
		strncpy(p, ext, le + 1);
		p = advance_p(p, le + 1, overflow);
	}

 end:
	return &(names[0]);

}

// create a (huge) triangle Mesh with a cube map
GObject *CreateSkybox(char *sbname,
					  char *dirname) {

	static const int A = 1000.0;
	float A2 = 0.5f * A;
	TriangleMesh *mesh;
	Texture *ctex;
	GObject *gobj;
	int len;
	char **names;

	mesh = CreateVoidTriangleMesh();

	AddPointTMesh(mesh, -A, -A2, A);  // P0
	AddPointTMesh(mesh,  A, -A2, A);  // P1
	AddPointTMesh(mesh,  A, A2, A);  // P2
	AddPointTMesh(mesh, -A, A2, A);  // P3
	AddPointTMesh(mesh, -A, -A2,  -A);  // P4
	AddPointTMesh(mesh,  A, -A2,  -A);  // P5
	AddPointTMesh(mesh,  A, A2,  -A);  // P6
	AddPointTMesh(mesh, -A, A2,  -A);  // P7

	// front
	AddTriangleTMesh(mesh, 0, 1, 2); // P0 - P1 - P2
	AddTriangleTMesh(mesh, 2, 3, 0); // P2 - P3 - P0
	// back
	AddTriangleTMesh(mesh, 5, 4, 7);
	AddTriangleTMesh(mesh, 7, 6, 5);
	// left
	AddTriangleTMesh(mesh, 4, 0, 3);
	AddTriangleTMesh(mesh, 3, 7, 4);
	// right
	AddTriangleTMesh(mesh, 1, 5, 6);
	AddTriangleTMesh(mesh, 6, 2, 1);
	// top
	AddTriangleTMesh(mesh, 3, 2, 6);
	AddTriangleTMesh(mesh, 6, 7, 3);
	// bottom
	AddTriangleTMesh(mesh, 4, 5, 1);
	AddTriangleTMesh(mesh, 1, 0, 4);

	// Create cube map texture and assign.

	names = generate_names(dirname);
	ctex = SceneRegisterCubeMapTexture(sbname,
									   names[0], names[1],
									   names[2], names[3],
									   names[4], names[5]);
	SetTextureMaterial(mesh->materialFront, ctex);
	gobj = SceneRegisterVoidGObject("SKYBOX/", sbname);
	AddPatchGObject(gobj, mesh);
	return gobj;
}

void DisplaySky(GObject *sky, camera *cam) {

	ShaderProgram *prev_shader;
	RenderState *rs;
	GLboolean prev_cull;
	float x,y,z; // camera position
	static trfm3D localT;

	rs = RenderStateScene();

	glGetBooleanv(GL_CULL_FACE, &prev_cull);

	prev_shader = GetShaderProgramRS(rs);
	SetShaderProgramRS(rs, FindShaderScene("sky"));
	glDisable(GL_CULL_FACE);

	// move skybox to camera origin
	GetCameraPosition(cam, &x, &y, &z);
	SetTransTrfm3D(&localT, x, y, z);
	PushRS(rs, AGA_MODELVIEW);
	MultTrfmRS(rs, AGA_MODELVIEW, &localT);
	DrawGObject(sky);
	PopRS(rs, AGA_MODELVIEW);
	// restore shader
	SetShaderProgramRS(rs, prev_shader);
	if (prev_cull == GL_TRUE)
		glEnable(GL_CULL_FACE);

}
