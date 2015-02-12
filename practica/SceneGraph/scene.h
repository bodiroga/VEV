#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "node.h"
#include "renderState.h"

/**
 * Initialize the scene
 *
 */

void InitScene();

/**
 * Destroy the scene (destroy all nodes)
 */

void DestroyScene();

/**
 * Return the root node
 *
 * @return the root node
 */

Node *RootNodeScene();

/**
 * Attach a node to the root node.
 * \note takes ownership
 *
 * @param theNode the node
 */

void NodeAttachScene(Node *theNode);

/**
 * Give a name to a Node, so you can later retrieve it given its name
 * \note does _not_ take ownership
 *
 * @param theNode the node
 * @param name the (unique) node name
 * @return 1 if succed; 0 if error or another node with same name exists.
 */

int SetNodenameScene(Node *theNode, char *name);

/**
 * Find a node.
 *
 * @param name the node name
 * @return the node (NULL if not found)
 */

Node *FindNodeScene(char * name);

/**
 * Draw the scene
 *
 */

void DrawScene();

/**
 * Return the current rendering state
 *
 * @return the rendering state
 */

RenderState *RenderStateScene();

// Renderer

/**
 * Set shading type to the scene
 *
 * @param face (GL_FRONT GL_BACK or GL_FRONT_AND_BACK).
 * @param mode (GL_POINT, GL_LINE,GL_FILL). default GL_FILL.
 */

void SetShadingScene(GLenum face, GLenum mode);

#endif //SCENE_H
