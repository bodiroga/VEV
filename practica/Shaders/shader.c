#include <string.h>
#include <math.h>
#include "hash.h"
#include "shader.h"
#include "shaderUtils.h"
#include "scene.h"

static void pShader_uniforms(ShaderProgram *theProgram) {

	static char buffer[1024];
	int i;

	// Fill uniforms for all possible lights
	for (i = 0; i < 8; ++i) { // AGA_MAX_LIGHTS
		sprintf(buffer, "theLights[%d].position", i);
		theProgram->light_unif[i].position = GetProgramUniform(theProgram->name, theProgram->program, buffer);
		sprintf(buffer, "theLights[%d].ambient", i);
		theProgram->light_unif[i].ambient = GetProgramUniform(theProgram->name, theProgram->program, buffer);
		sprintf(buffer, "theLights[%d].diffuse", i);
		theProgram->light_unif[i].diffuse = GetProgramUniform(theProgram->name, theProgram->program, buffer);
		sprintf(buffer, "theLights[%d].specular", i);
		theProgram->light_unif[i].specular = GetProgramUniform(theProgram->name, theProgram->program, buffer);
		sprintf(buffer, "theLights[%d].attenuation", i);
		theProgram->light_unif[i].attenuation = GetProgramUniform(theProgram->name, theProgram->program, buffer);
		sprintf(buffer, "theLights[%d].spotDir", i);
		theProgram->light_unif[i].spotDir = GetProgramUniform(theProgram->name, theProgram->program, buffer);
		sprintf(buffer, "theLights[%d].cosCutOff", i);
		theProgram->light_unif[i].cosCutOff = GetProgramUniform(theProgram->name, theProgram->program, buffer);
		sprintf(buffer, "theLights[%d].exponent", i);
		theProgram->light_unif[i].exponent = GetProgramUniform(theProgram->name, theProgram->program, buffer);
	}
	theProgram->activeLights_unif = GetProgramUniform(theProgram->name, theProgram->program, "active_lights_n");

	theProgram->mat_unif.ambient = GetProgramUniform(theProgram->name, theProgram->program, "theMaterial.ambient");
	theProgram->mat_unif.diffuse = GetProgramUniform(theProgram->name, theProgram->program, "theMaterial.diffuse");
	theProgram->mat_unif.specular = GetProgramUniform(theProgram->name, theProgram->program, "theMaterial.specular");
	theProgram->mat_unif.shininess = GetProgramUniform(theProgram->name, theProgram->program, "theMaterial.shininess");
	theProgram->mat_unif.alpha = GetProgramUniform(theProgram->name, theProgram->program, "theMaterial.alpha");

	theProgram->texSampler_unif = GetProgramUniform(theProgram->name, theProgram->program, "texture0");

	theProgram->modeltoCamera_unif = GetProgramUniform(theProgram->name, theProgram->program, "modelToCameraMatrix");
	theProgram->CameraToClip_unif = GetProgramUniform(theProgram->name, theProgram->program, "cameraToClipMatrix");
	theProgram->modeltoClip_unif = GetProgramUniform(theProgram->name, theProgram->program, "modelToClipMatrix");
	theProgram->normalToCamera_unif = GetProgramUniform(theProgram->name, theProgram->program, "normalToCameraMatrix");

}

ShaderProgram *CreateShader(char *shaderName, char *vShaderFile, char *fShaderFile) {

	ShaderProgram *newShader;

	newShader = malloc(sizeof(*newShader));
	newShader->name = strdup(shaderName);
	printf("[I] Shader %s\n", shaderName);
	printf("[I] compiling %s ...", vShaderFile);
	newShader->vShader = LoadShader(GL_VERTEX_SHADER, vShaderFile);
	printf("OK\n");
	printf("[I] compiling %s ...", fShaderFile);
	newShader->fShader = LoadShader(GL_FRAGMENT_SHADER, fShaderFile);
	printf("OK\n");
	printf("[I] linking ...");
	newShader->program = CreateProgramFromObjects(shaderName, newShader->vShader, newShader->fShader);
	printf("OK\n");
	pShader_uniforms(newShader);

	return newShader;
}

void DestroyShader(ShaderProgram **theProgram) {
	ShaderProgram *oneProgram;

	oneProgram = *theProgram;
	free(oneProgram->name);
	free(oneProgram);
	*theProgram = NULL;
}

void ActivateShader(ShaderProgram *theProgram) {

	GLenum errorCode;

	if (theProgram == NULL) return;

	glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &(theProgram->old_program));
	glUseProgram(theProgram->program);
	errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		fprintf (stderr, "[E] ActivateShader: program %d: %s\n", theProgram->program, gluErrorString(errorCode));
		exit(1);
	}
}

void DeactivateShader(ShaderProgram *theProgram) {
	GLenum errorCode;

	if (theProgram == NULL) return;

	glUseProgram(theProgram->old_program);
	errorCode = glGetError();
	if (errorCode != GL_NO_ERROR) {
		fprintf (stderr, "[E] DeactivateShader: program %d: %s\n", theProgram->program, gluErrorString(errorCode));
		exit(1);
	}
}

void BeforeDrawShader(ShaderProgram *theProgram) {

	RenderState *rs;
	Light *oneLight;
	Material *mat;
	Texture *tex;
	int i;

	if (theProgram == NULL) return;

	rs = RenderStateScene();

	shader_set_uniform_matrix4(theProgram->modeltoCamera_unif, OpenGLRS(rs, AGA_MODELVIEW));
	shader_set_uniform_matrix4(theProgram->CameraToClip_unif, OpenGLRS(rs, AGA_PROJECTION));
	shader_set_uniform_matrix4(theProgram->modeltoClip_unif, OpenGLRS(rs, AGA_MODELVIEW_PROJECTION));
	shader_set_uniform_matrix4(theProgram->normalToCamera_unif, OpenGLRS(rs, AGA_MODELVIEW_PROJECTION));

	i = 0;
	oneLight = StartLoop(rs->lightList);
	while (oneLight) {
		if (IsOnLight(oneLight)) {
			if (i == 8) { // AGA_MAX_LIGHTS
				fprintf(stderr, "[W] too many active lights. Discarding the rest\n");
				break;
			}
			shader_set_uniform_4fv(theProgram->light_unif[i].position, GetPositionLight(oneLight));
			shader_set_uniform_3fv(theProgram->light_unif[i].diffuse, GetDiffuseLight(oneLight));
			shader_set_uniform_3fv(theProgram->light_unif[i].ambient, GetAmbientLight(oneLight));
			shader_set_uniform_3fv(theProgram->light_unif[i].specular, GetSpecularLight(oneLight));
			shader_set_uniform_3fv(theProgram->light_unif[i].attenuation, GetAttenuationVectorLight(oneLight));
			if (IsSpotLight(oneLight)) {
				shader_set_uniform_3fv(theProgram->light_unif[i].spotDir, GetSpotDirectionLight(oneLight));
				shader_set_uniform_1f(theProgram->light_unif[i].exponent, GetSpotExponentLight(oneLight));
				shader_set_uniform_1f(theProgram->light_unif[i].cosCutOff, cosf(GetSpotCutoffLight(oneLight)));
			} else {
				shader_set_uniform_1f(theProgram->light_unif[i].cosCutOff, 0.0f); // if (cos) cutoff is zero -> no spotLight
			}
			++i;
		}
		oneLight = GetNext(rs->lightList);
	}
	shader_set_uniform_1i(theProgram->activeLights_unif, i);

	mat = rs->frontMaterial;
	if (mat != NULL) {
		shader_set_uniform_3fv(theProgram->mat_unif.ambient, GetAmbientMaterial(mat));
		shader_set_uniform_3fv(theProgram->mat_unif.diffuse, GetDiffuseMaterial(mat));
		shader_set_uniform_3fv(theProgram->mat_unif.specular, GetSpecularMaterial(mat));
		shader_set_uniform_1f(theProgram->mat_unif.shininess, GetShininessMaterial(mat));
		shader_set_uniform_1f(theProgram->mat_unif.alpha, GetAlphaMaterial(mat));

		tex = GetTextureMaterial(mat);
		if (tex != NULL) {
			// Set texture to unit 0
			BindGLTextureTunit(tex, 0);
			shader_set_uniform_1i(theProgram->texSampler_unif, 0); // Texture unit 0
		}
	}
}
