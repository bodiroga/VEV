#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

/**
 * @file   shaderManager.h
 *
 * @brief  a manager for creating/registering/accessing shader programs.
 */


#include "shader.h"

/**
 * Initialize shader manager
 */
void InitShaderManager();

/**
 * Destroy shader manager
 */
void DestroyShaderManager();

/**
 * Register a new shader program
 *
 * @param shaderName the name of the shader program
 * @param vShaderFile filename where vertex shader is
 * @param fShaderFile filename where fragment shader is
 *
 * @return if the name is new, the new shader program; else, the shader program
 * associated with this name.
 */
ShaderProgram *RegisterShaderScene(char *shaderName, char *vShaderFile, char *fShaderFile);

/**
 * Retrieve a shader program given its name
 *
 * @param shaderName the name of the shader program
 *
 * @return the shader program (NULL of not found)
 */
ShaderProgram *FindShaderScene(char *shaderName);


#endif
