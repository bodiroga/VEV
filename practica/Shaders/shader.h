#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/glut.h>

typedef struct _LightUniform {

	GLint position;
	GLint ambient;
	GLint diffuse;
	GLint specular;
	GLint attenuation;
	GLint spotDir;     // only for spotLights
	GLint cosCutOff;   // only for spotLights
	GLint exponent;    // only for spotLights

} LightUniform;

typedef struct _MaterialUniform {

	GLint ambient;
	GLint diffuse;
	GLint specular;
	GLint alpha;
	GLint shininess;

} MaterialUniform;


typedef struct _ShaderProgram {

	char *name;

	GLuint vShader, fShader, program;

	GLint modeltoCamera_unif;
	GLint CameraToClip_unif;
	GLint modeltoClip_unif;
	GLint normalToCamera_unif;

	GLint            activeLights_unif;
	LightUniform     light_unif[8];     // AGA_MAX_LIGHTS

	MaterialUniform mat_unif;

	GLint texSampler_unif;

	GLuint old_program; // active program when ActivateShader was called

} ShaderProgram;

/**
 * Create a Shader Program
 *
 * @param shaderName name of the shader program.
 * @param vShaderFile filename of the vertex shader.
 * @param fShaderFile filename of the fragment shader
 *
 * @return the newly created shader program
 */
ShaderProgram *CreateShader(char *shaderName, char *vShaderFile, char *fShaderFile);

/**
 * Destroy a Shader program.
 *
 * @param theProgram a pointer to the shader program.
 */
void DestroyShader(ShaderProgram **theProgram);

/**
 * Activate Shader Program
 *
 * @param theProgram the shader program.
 */
void ActivateShader(ShaderProgram *theProgram);

/**
 * Dectivate Shader Program
 *
 * @param theProgram the shader program.
 */
void DeactivateShader(ShaderProgram *theProgram);

/**
 * Set-up shader uniforms with state information. Call it before drawing
 * anything.
 *
 * @param theProgram the shader program.
 */
void BeforeDrawShader(ShaderProgram *theProgram);

#endif
