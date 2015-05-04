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

	Node *newNode;
	Node *oneNode;

	newNode = CreateNode();
	newNode->gObject = sNode->gObject;
	newNode->theLight = sNode->theLight;
	newNode->theShader = sNode->theShader;
	SetCopyTrfm3D(newNode->placement, sNode->placement);
	newNode->parent = theParent;

	oneNode = StartLoop(sNode->nodeChilds);
	while(oneNode) {
		AddLast(newNode->nodeChilds,
				NodeCloneParent(oneNode, newNode)); // recursive call
		oneNode = GetNext(sNode->nodeChilds);
	}
	return newNode;
}


Node *CloneNode(Node *sNode) {

	if (sNode == NULL) return NULL;
	return NodeCloneParent(sNode, NULL);

}

// auxiliary function
//
// given a node:
// - update the BBox of the node (nodeUpdateBB)
// - Propagate BBox to parent until root is reached
//
// - Preconditions:
//    - the BBox of thisNode's children are up-to-date.
//    - placement_WC of node and parents are up-to-date

static void nodePropagateBBRoot(Node * thisNode) {

	Node *oneNode;
	for (oneNode = thisNode; oneNode; oneNode = oneNode->parent)
		nodeUpdateBB(oneNode);
}

// auxiliary function
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
// These functions can be useful (from bbox.h):
//
// SetVoidBBox(BBox *A)
//
//    Set A to be the void (empty) BBox
//
// TransformBBox(BBox *A, BBox *B, trfm3D *T);
//
//    Transform B by applying transformation T. Leave result in A.
//
// BoxBox(BBox *A, BBox *B)
//
//    Change BBox A so it also includes BBox B
//

static void nodeUpdateBB (Node *thisNode) {

	Node *oneNode;

	if (thisNode == NULL) return;

	// Get initial BBOX and set to container_WC
	//   if gObject, use his local BBox transformed to WC
	//   else get BBox of first child
	if (thisNode->gObject) {
		TransformBBox (thisNode->container_WC,
					   thisNode->gObject->container,
					   thisNode->placement_WC);
	} else {
		SetVoidBBox(thisNode->container_WC);
		// Increase container_WC with child's BBoxes
		oneNode = StartLoop(thisNode->nodeChilds);
		while(oneNode) {
			BoxBox(thisNode->container_WC,
				   oneNode->container_WC);
			oneNode = GetNext(thisNode->nodeChilds);
		}
	}
}

// Update WC (world coordinates matrix) of a node and
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

	Node *oneNode;

	if (thisNode->parent == NULL) {
		SetCopyTrfm3D(thisNode->placement_WC, thisNode->placement); // placement already in world coordinates.
	} else {
		// Compose placement with parent's placement_WC
		SetCopyTrfm3D( thisNode->placement_WC, thisNode->parent->placement_WC);
		CompositionTrfm3D (thisNode->placement_WC, thisNode->placement);
	}

	// update child transformations
	oneNode = StartLoop(thisNode->nodeChilds);
	while(oneNode) {
		UpdateWCnode(oneNode); // recursive call
		oneNode = GetNext(thisNode->nodeChilds);
	}
	// Now update Bounding Box to world coordinates
	nodeUpdateBB(thisNode);
}

// Auxiliary function
//
// given a node:
// - Update WC of sub-tree starting at node (UpdateWCnode)
// - Propagate Bounding Box to root (nodePropagateBBRoot)

static void UpdateGSnode( Node *thisNode) {

	if (thisNode == NULL) return;
	UpdateWCnode(thisNode);
	nodePropagateBBRoot(thisNode->parent);
}

//
// Attach (group) a Node object into another Node
// Print a warning (and do nothing) if the node has an gObject.

void NodeAttach(Node *theNode, Node *theChild) {

	if (theNode == NULL || theChild == NULL) return;
	if (theNode->gObject) {
		fprintf(stderr, "[W] NodeAttach: can not attach childs to a node with a gObject.\n");
		return;
	}
	AddLast(theNode->nodeChilds, theChild);
	theChild->parent = theNode;
	// Update WC of the child
	UpdateGSnode(theChild);
}

//
// Detach a Node object from a Node
//

void NodeDetach(Node *theNode) {

	Node *theParent;

	if (theNode == NULL) return;
	theParent = theNode->parent;
	if (theParent == NULL) return; // already detached (or root node)
	theNode->parent = NULL;
	RemoveFromList(theParent->nodeChilds, theNode);
	// Update bounding box of parent
	nodePropagateBBRoot(theParent);
}

//
// Detach all children object from a Node
//

void AllChildsNodeDetach(Node *theNode) {

	Node *oneNode;

	if (theNode == NULL) return;
	oneNode = StartLoop(theNode->nodeChilds);
	while(oneNode) {
		oneNode->parent = NULL;
		oneNode = GetNext(theNode->nodeChilds);
	}

	EmptyList(theNode->nodeChilds);
	nodePropagateBBRoot(theNode);
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
	ShaderProgram *prev_shader;
	RenderState *rs;
	int i=0;

	if (theNode == NULL) return;
	rs = RenderStateScene();

	if (theNode->isCulled) return;

	// Set shader (save previous)
	if (theNode->theShader != NULL) {
		prev_shader = GetShaderProgramRS(rs);
		SetShaderProgramRS(rs, theNode->theShader);
	}

	// Print BBoxes
	if(rs->drawBBox || theNode->drawBBox)
		DrawGLBBox( theNode->container_WC );

	// Draw geometry object
	if (theNode->gObject) {
		// the world transformation is already precalculated, so just put the
		// transformation in the openGL matrix
		//
		PushRS(rs, AGA_MODELVIEW);
		MultTrfmRS(rs, AGA_MODELVIEW, theNode->placement_WC);
		DrawGObject( theNode->gObject );
		PopRS(rs, AGA_MODELVIEW);
	}

	// draw sub-nodes (recursive)
	oneNode = StartLoop(theNode->nodeChilds);
	while(oneNode) {
		++i;
		DrawNode(oneNode); // Recursive call
		oneNode = GetNext(theNode->nodeChilds);
	}

	if (theNode->theShader != NULL) {
		// restore shader
		SetShaderProgramRS(rs, prev_shader);
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

	int res = 0;
	Node *oneNode, *aux;

	if (thisNode == NULL) return NULL;

	if (thisNode->gObject) {
		if (BSphereBBoxIntersect(bsph, thisNode->container_WC) == IINTERSECT)
			return thisNode;
		else
			return NULL;
	}

	oneNode = StartLoop(thisNode->nodeChilds);

	while(oneNode) {
		aux = CollisionBSphereNode(oneNode, bsph);
		if (aux != NULL) {
			return aux;
		}
		oneNode = GetNext(thisNode->nodeChilds);
	}

	return NULL; /* No collision */
}
