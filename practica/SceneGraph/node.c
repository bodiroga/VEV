#include <stdlib.h>
#include "node.h"
#include "intersect.h"
#include "bboxGL.h"
#include "scene.h"

// forward declarations of auxiliary functions

static Node* NodeCloneParent(Node *sNode, Node *theParent);
static void nodeUpdateBB (Node *thisNode);
static void nodePropagateBBRoot(Node * thisNode);
static void UpdateGSnode( Node *thisNode);

Node *CreateNode() {

	Node * thisNode;

	thisNode = malloc(sizeof(*thisNode) * 1);
	thisNode->gObject = NULL;
	thisNode->theLight = NULL;
	thisNode->theShader = NULL;
	thisNode->nodeChilds = CreateVoidList();
	thisNode->placement = CreateTrfm3D();
	thisNode->placement_WC = CreateTrfm3D();
	thisNode->container_WC = CreateVoidBBox();
	thisNode->parent = NULL;
	thisNode->isCulled = 0;
	thisNode->drawBBox = 0;

	return thisNode;
}

void DestroyNode(Node **theNode) {

	Node * oneNode;
	Node  *thisNode = *theNode;

	if( ! thisNode ) return;

	// destroy Node list
	oneNode = StartLoop(thisNode->nodeChilds);
	while(oneNode) {
		DestroyNode(&oneNode); // Recursive call
		oneNode = GetNext(thisNode->nodeChilds);
	}
	DestroyList(&thisNode->nodeChilds);

	DestroyBBox(&thisNode->container_WC);
	DestroyTrfm3D(&thisNode->placement);
	DestroyTrfm3D(&thisNode->placement_WC);
	thisNode->gObject = NULL;
	thisNode->theLight = NULL;
	thisNode->theShader = NULL;
	thisNode->parent = NULL;

	free( thisNode );

	*theNode = NULL;
}

Node * CycleChildNode(Node *theNode, size_t idx) {

	size_t   i;
	size_t   m;
	Node     *cur = NULL;

	if( theNode == NULL ) return NULL;

	m = idx % ElementsInList(theNode->nodeChilds);
	for( i = 0, cur = StartLoop(theNode->nodeChilds);
		 cur && i < m;
		 ++i, cur = GetNext(theNode->nodeChilds));
	return cur;
}

Node *NextSiblingNode(Node *theNode) {
	Node *theParent;
	Node *theSibling = NULL;

	if (!theNode) return NULL;
	theParent = theNode->parent;
	if (!theParent) return theNode;
	theSibling = StartLoop(theParent->nodeChilds);
	while(theSibling && theSibling != theNode) {
		theSibling = GetNext(theParent->nodeChilds);
	}
	theSibling = GetNext(theParent->nodeChilds);
	if (!theSibling) return StartLoop(theParent->nodeChilds);
	return theSibling;
}


Node *FirstChildNode(Node *theNode) {

	Node *theChild = NULL;

	if (!theNode) return NULL;
	theChild = StartLoop(theNode->nodeChilds);
	if (!theChild) return theNode;
	return theChild;
}

Node *ParentNode(Node *theNode) {
	if (!theNode) return NULL;
	if (!theNode->parent) return theNode;
	return theNode->parent;
}

static Node* NodeCloneParent(Node *sNode, Node *theParent) {

	Node *clonedNode = CreateNode();
	Node *siguienteChild;

	clonedNode->nodeChilds = sNode->nodeChilds;
	clonedNode->gObject = sNode->gObject;
	clonedNode->theLight = sNode->theLight;
	clonedNode->theShader = sNode->theShader;
	clonedNode->container_WC = sNode->container_WC;
	clonedNode->placement = sNode->placement;
	clonedNode->placement_WC = sNode->placement_WC;
	clonedNode->isCulled = sNode->isCulled;
	clonedNode->drawBBox = sNode->drawBBox;
	clonedNode->parent = theParent;

	siguienteChild = StartLoop(clonedNode->nodeChilds);
	while(siguienteChild) {
		NodeCloneParent(siguienteChild, clonedNode);
		siguienteChild = GetNext(clonedNode->nodeChilds);
	}

	return clonedNode;

}


Node *CloneNode(Node *sNode) {

	Node *clonedNode = CreateNode();
	Node *siguienteChild;

	clonedNode->nodeChilds = sNode->nodeChilds;
	clonedNode->gObject = sNode->gObject;
	clonedNode->theLight = sNode->theLight;
	clonedNode->theShader = sNode->theShader;
	clonedNode->container_WC = sNode->container_WC;
	clonedNode->placement = sNode->placement;
	clonedNode->placement_WC = sNode->placement_WC;
	clonedNode->isCulled = sNode->isCulled;
	clonedNode->drawBBox = sNode->drawBBox;
	clonedNode->parent = sNode->parent;

	siguienteChild = StartLoop(clonedNode->nodeChilds);
	while(siguienteChild) {
		NodeCloneParent(siguienteChild, clonedNode);
		siguienteChild = GetNext(clonedNode->nodeChilds);
	}

	return clonedNode;

}

// @@ TODO: auxiliary function
//
// given a node:
// - update the BBox of the node (nodeUpdateBB)
// - Propagate BBox to parent until root is reached
//
// - Preconditions:
//    - the BBox of thisNode's children are up-to-date.
//    - placement_WC of node and parents are up-to-date

static void nodePropagateBBRoot(Node * thisNode) {

	if (thisNode->parent == NULL) {
		NULL;
	} else {
		nodeUpdateBB(thisNode->parent);
		nodePropagateBBRoot(thisNode->parent);
	}

}

// @@ TODO: auxiliary function
//
// given a node, update its BBox to World Coordinates so that it includes:
//  - the BBox of the geometricObject it contains (if any)
//  - the BBox-es of the children
//
// Note: Bounding box is always in world coordinates
//
// Precontitions:
//
//     * thisNode->placement_WC is up-to-date
//     * container_WC of thisNode's children are up-to-date
//
// These functions can be useful:
//
// TransformBBox(BBox *A, BBox *B, trfm3D *T);
//
//    Transform A By applying transformation T. Leave result in A.
//
// BoxBox(BBox *A, BBox *B)
//
//    Change BBox A so it also includes BBox B
//

static void nodeUpdateBB (Node *thisNode) {

	if (thisNode->parent == NULL) {
		NULL;
	} else {
		if (thisNode->gObject != NULL) {
			TransformBBox(thisNode->container_WC, thisNode->gObject->container, thisNode->placement_WC);
		} else {
			Node *siguienteChild;
			siguienteChild = StartLoop(thisNode->nodeChilds);
			while(siguienteChild) {
				BoxBox(thisNode->container_WC, siguienteChild->container_WC);
				siguienteChild = GetNext(thisNode->nodeChilds);
			}
		}
	}

}

// @@ TODO: Update WC (world coordinates matrix) of a node and
// its bounding box recursively updating all children.
//
// given a node:
//  - update the world transformation (placement_WC) using the parent's WC transformation
//  - recursive call to update WC of the children
//  - update Bounding Box to world coordinates
//
// Precondition:
//
//  - placement_WC of thisNode->parent is up-to-date (or thisNode->parent == NULL)
//

static void UpdateWCnode( Node *thisNode) {

	Node * siguienteChild;

	if (thisNode->parent == NULL) {
		thisNode->placement_WC = thisNode->placement;
	} else {
		thisNode->placement_WC = thisNode->parent->placement_WC;
		CompositionTrfm3D(thisNode->placement_WC, thisNode->placement);
	}

	siguienteChild = StartLoop(thisNode->nodeChilds);
	while(siguienteChild) {
		UpdateWCnode(siguienteChild);
		siguienteChild = GetNext(thisNode->nodeChilds);
	}

	nodeUpdateBB(thisNode);

}

// @@ TODO: Auxiliary function
//
// given a node:
// - Update WC of sub-tree starting at node (UpdateWCnode)
// - Propagate Bounding Box to root (nodePropagateBBRoot)

static void UpdateGSnode( Node *thisNode) {

}

// @@ TODO:
//
// Attach (group) a Node object into another Node
// Print a warning (and do nothing) if the node has an gObject.

void NodeAttach(Node *theNode, Node *theChild) {

	// Si el nodo es una hoja, imprimimos la advertencia
	if (theNode->gObject != NULL) {
		fprintf(stdout, "Cuidado, el nodo es una hoja\n");
		return;
	} else {
		// Le decimos al padre que tiene un nuevo hijo
		AddLast(theNode->nodeChilds, theChild);
		// Le decimos al hijo quien es su padre
		theChild->parent = theNode;
	}

}

// @@ TODO:
//
// Detach a Node object from a Node
//

void NodeDetach(Node *theNode) {

	Node * siguienteChild;

	// Primero vamos a decirle a los hijos del nodo que su nuevo padre es el abuelo
	// y vamos a decirle al abuelo que sus nietos pasan a ser sus hijos
	siguienteChild = StartLoop(theNode->nodeChilds);
	while(siguienteChild) {
		siguienteChild->parent = theNode->parent;
		AddLast(theNode->parent->nodeChilds, siguienteChild);
		siguienteChild = GetNext(theNode->nodeChilds);
	}

	// Finalmente quitamos el nodo de la lista de hijos del padre
	RemoveFromList(theNode->parent->nodeChilds, theNode);


}

// @@ TODO:
//
// Detach all children object from a Node
//

void AllChildsNodeDetach(Node *theNode) {

	Node * siguienteChild;

	// Para cada hijo del nodo vamos a llamar a NodeDetach
	siguienteChild = StartLoop(theNode->nodeChilds);
	while(siguienteChild) {
		NodeDetach(siguienteChild);
		siguienteChild = GetNext(theNode->nodeChilds);
	}

}

void SetGobjNode( Node *thisNode, GObject *gobj ) {
	if (thisNode == NULL) return;
	if (ElementsInList(thisNode->nodeChilds)) {
		fprintf(stderr, "[W] SetGobjNode: can not attach a gObject to a node with childs.\n");
		return;
	}
	thisNode->gObject = gobj;
	nodePropagateBBRoot(thisNode);
}

void SetTrfmNode( Node *thisNode, const trfm3D * M) {
	if (thisNode == NULL) return;
	SetCopyTrfm3D(thisNode->placement, M);
	// Update Geometric state
	UpdateGSnode(thisNode);
}

void ApplyTrfmNode( Node *thisNode, const trfm3D * M) {
	if (thisNode == NULL) return;
	CompositionTrfm3D(thisNode->placement, M);
	// Update Geometric state
	UpdateGSnode(thisNode);
}

void TransNode( Node *thisNode, float x, float y, float z ) {
	static trfm3D localT;

	if (thisNode == NULL) return;
	SetTransTrfm3D(&localT, x, y, z);
	ApplyTrfmNode(thisNode, &localT);
};

void RotateXNode( Node *thisNode, float angle ) {
	static trfm3D localT;

	if (thisNode == NULL) return;
	SetRot_X_Trfm3D(&localT, angle);
	ApplyTrfmNode(thisNode, &localT);
};

void RotateYNode( Node *thisNode, float angle ) {
	static trfm3D localT;

	if (thisNode == NULL) return;
	SetRot_Y_Trfm3D(&localT, angle);
	ApplyTrfmNode(thisNode, &localT);
};

void RotateZNode( Node *thisNode, float angle ) {
	static trfm3D localT;

	if (thisNode == NULL) return;
	SetRot_Z_Trfm3D(&localT, angle);
	ApplyTrfmNode(thisNode, &localT);
};

void ScaleNode( Node *thisNode, float factor ) {
	static trfm3D localT;

	if (thisNode == NULL) return;
	SetScaleTrfm3D(&localT, factor);
	ApplyTrfmNode(thisNode, &localT);
};

void AttachLightNode(Node *theNode, Light *theLight) {
	if (theNode == NULL) return;
	theNode->theLight = theLight;
}

void DetachLightNode(Node *theNode) {
	if (theNode == NULL) return;
	theNode->theLight = NULL;
}

void SetShaderNode(Node *theNode, ShaderProgram *theShader) {
	if (theNode == NULL) return;
	theNode->theShader = theShader;
}

void UnsetShaderNode(Node *theNode) {
	if (theNode == NULL) return;
	theNode->theShader = NULL;
}

// @@ TODO:
// Draw a (sub)tree.
//     - First spatial Objects
//     - Then children (depth-first)
//
// These functions can be useful:
//
// rs = RenderStateScene();
//
// MultTrfmRS(rs, AGA_MODELVIEW, T); // Apply T transformation
// PushRS(rs, AGA_MODELVIEW); // push current matrix into stack
// PushRS(rs, AGA_MODELVIEW); // pop matrix from stack to current
//
// DrawGObject(gobj); // draw geometry object
//

void DrawNode(Node *theNode) {

	Node * oneNode;
	RenderState *rs;

}

// Set culled state of a node's children

static void set_subtree_isCulled(Node *theNode,
								 int culled) {
	Node * oneNode;
	oneNode = StartLoop(theNode->nodeChilds);
	while(oneNode) {
		oneNode->isCulled = culled;
		set_subtree_isCulled(oneNode, culled);
		oneNode = GetNext(theNode->nodeChilds);
	}
}

// @@ TODO: Frustum culling. See if a subtree is culled by the camera, and
//          update isCulled accordingly.

void UpdateCullNode(Node *theNode,
					camera * cam) {
}

// @@ TODO: Check whether a BSphere (in world coordinates) intersects with a
// (sub)tree.
//
// Return a pointer to the Node which collides with the BSphere. NULL
// if not collision.

Node *CollisionBSphereNode(Node *thisNode, BSphere *bsph) {

	return NULL; /* No collision */
}
