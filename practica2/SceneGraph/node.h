#ifndef NODE_H
#define NODE_H

#include <stddef.h>

/**
 * @file   Node.h
 * @author Aitor Soroa
 *
 * @brief Scene graphs are composed by nodes. By now, there are two types of
 * nodes
 * - SpatialObject's, that is, actual objects
 * - GroupNode's, used to group objects (or other nodes)
 *
 */

#include <stdio.h>
#include "trfm3D.h"
#include "list.h"
#include "bbox.h"
#include "bsphere.h"
#include "gObject.h"
#include "camera.h"
#include "light.h"
#include "shader.h"


typedef struct _Node {

	// nodes
	list               *nodeChilds;
	GObject            *gObject; //!< not owned!
	Light              *theLight;
	ShaderProgram      *theShader;
	BBox               *container_WC;
	trfm3D             *placement;
	trfm3D             *placement_WC;
	int                isCulled;
	int                drawBBox;
	struct _Node       *parent;
} Node;


/**
 *
 * Create a node with unit placement and bbox
 *
 * @return the created node
 */

Node *CreateNode();

/**
 * Make a copy of the subtree starting at sNode. The created node will _not_
 * have any parents.
 *
 * @param sNode the subtree root
 *
 * @return a new subtree not linked to any node
 *
 * \note the WC transformations and BBox-es of the subtree are not
 * calculated (as we don't know which parent is the subtree linked to). It
 * will be calculated, as usual, when attaching the subtree to a node, via
 * attachNode function.
 */

Node *CloneNode(Node *sNode);

void DestroyNode(Node **theNode);

/**
 * Set a trfm (orientation + placement)
 *
 * @param thisNode
 * @param M
 */

void SetTrfmNode( Node *thisNode, const trfm3D * M);

/**
 * Attach a geometry Node
 *
 * @param thisNode
 * @param M the geometry Object
 */

void SetGobjNode( Node *thisNode, GObject * M);

/**
 * Apply a trfm
 *
 * @param thisNode
 * @param M
 */

void ApplyTrfmNode( Node *thisNode, const trfm3D * M);

/**
 * Apply a translation
 *
 * @param thisNode
 * @param x
 * @param y
 * @param z
 */

void TransNode( Node *thisNode, float x, float y, float z );

/**
 * Rotate X
 *
 * @param thisNode
 * @param angle
 */

void RotateXNode( Node *thisNode, float angle );

/**
 * Rotate Y
 *
 * @param thisNode
 * @param angle
 */

void RotateYNode( Node *thisNode, float angle );

/**
 * Rotate Z
 *
 * @param thisNode
 * @param angle
 */

void RotateZNode( Node *thisNode, float angle );

/**
 * Uniform scale
 *
 * @param thisNode
 * @param factor
 */

void ScaleNode( Node *thisNode, float factor );


/**
 * attachNode: attach (group) a Node object into another Node
 *
 * @param theNode : the Node where the object will be attached
 * @param theChild : the object to be attached
 *
 *  \note the node takes ownership
 */
void NodeAttach(Node *theNode, Node *theChild);

/**
 * Detach all childs of a Node.
 *
 * @param theNode the Node
 *
 * \note The childs objects are _not_ destroyed, only detached from the Node
 */

void AllChildsNodeDetach(Node *theNode);

/**
 * Detach a Node object from its parent
 *
 * @param theNode  the Node
 *
 * \note The object is _not_ destroyed, only detached from the Node
 */

void NodeDetach(Node *theNode);

/**
 * Attach a light to a Node
 *
 * @param theNode  the Node
 * @param theLight the light object to be attached
 *
 */

void AttachLightNode(Node *theNode, Light *theLight);

/**
 * Detach a light from a Node
 *
 * @param theNode  the Node
 *
 */

void DetachLightNode(Node *theNode);

/**
 * DrawNode: Draw a (sub)tree. Objects first, sub-objects last. Depth first.
 *
 * @param theNode A pointer to the (sub)graph
 *
 */

/**
 * Attach a shader to a Node
 *
 * @param theNode  the Node
 * @param theShader the shader object to be attached. If NULL, detach shader.
 * \note The node and all descendants will be rendered with this shader.
 *
 */

void SetShaderNode(Node *theNode, ShaderProgram *theShader);

/**
 * Detach a shader from a Node
 *
 * @param theNode  the Node
 *
 */

void UnsetShaderNode(Node *theNode);

/**
 * DrawNode: Draw a (sub)tree. Objects first, sub-objects last. Depth first.
 *
 * @param theNode A pointer to the (sub)graph
 *
 */

void DrawNode(Node *theNode);


/**
 * ParentNode: Get parent of node. Return itself if root node.
 *
 * @param theNode A pointer to the node
 */

Node *ParentNode(Node *theNode);

/**
 * NextSiblingNode: Get next sibling of node. Return itself if last child.
 *
 * @param theNode A pointer to the node
 */

Node *NextSiblingNode(Node *theNode);

/**
 * FirstChildNode: Get first child of node. Return itself if leaf node.
 *
 * @param theNode A pointer to the node
 */

Node *FirstChildNode(Node *theNode);

/**
 * NodeCicleChild: Cycle through children of a Node. Return children[ i % lenght(children) ]
 *
 * @param theNode A pointer to the node
 * @param i index
 */

Node *CycleChildNode(Node *theNode, size_t i);

/*
 * Check whether a tree is culled according to a camera.
 *
 * @param theNode A pointer to the (sub)graph
 * @param cam A camera (frustum)
 */

void UpdateCullNode(Node *theNode, camera *cam);

/**
 * NodeCollision: Check wether a BSphere (in woorld coord.) collides with a
 * (sub)-tree
 *
 * @param theNode A pointer to the node
 * @param bsp  Bounding Sphere in world coordinates
 * @return the Node which collided with the BSphere. NULL if not collision.
 */

Node *CollisionBSphereNode(Node *theNode, BSphere *bsp);

#endif
