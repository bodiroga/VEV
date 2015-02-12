#include "list.h"
#include "hash.h"
#include "tools.h"
#include "trfm3D.h"
#include "scene.h"
#include "node.h"
#include "gObjectManager.h"
#include "textureManager.h"
#include "materialManager.h"
#include "shaderManager.h"
#include "lightManager.h"
#include "cameraManager.h"
#include "avatar.h"

static Node *create_city (int N, list *gObj_list); // forward declaration

Node *create_scene() {

	GObject *gobj;
	GObject *gobj2;
	GObject *gobj3;
	GObject *gobj4;
	GObject *gobj5;
	Node    *myNode, *auxNode;

	static trfm3D TT;

	// sceneRegisterGObject takes ownership of geometric objects
	//gobj = SceneRegisterGObject( "./obj/spheres/", "solid.obj");
	gobj =  SceneRegisterGObject( "./obj/spheres/", "smooth.obj");
	gobj2 = SceneRegisterGObject( "./obj/chapel/", "chapel.obj");
	gobj3 = SceneRegisterGObject( "./obj/cubes/", "cubo.obj");
	gobj4 = SceneRegisterGObject( "./obj/cubes/", "cubotex.obj");
	gobj5 = SceneRegisterGObject( "./obj/floor/", "floor.obj");

	SetTransTrfm3D(&TT, 0, -10, -100);
	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
	NodeAttachScene(myNode); // takes ownership
	SetShaderNode(myNode, FindShaderScene("pervertex"));
	myNode->drawBBox = 0;

	SetTransTrfm3D(&TT, -20, 0, 0);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, gobj);
	SetShaderNode(auxNode, FindShaderScene("pervertex_notex"));
	auxNode->drawBBox = 0;
	NodeAttach(myNode, auxNode); // takes ownership

	SetTransTrfm3D(&TT, 20, 0, 0);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, gobj2);
	auxNode->drawBBox = 0;
	NodeAttach(myNode, auxNode); // takes ownership

	SetTransTrfm3D(&TT, 0, 0, -20);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, gobj3);
	SetShaderNode(auxNode, FindShaderScene("pervertex_notex"));
	auxNode->drawBBox = 0;
	NodeAttach(myNode, auxNode); // takes ownership

	SetTransTrfm3D(&TT, 0, 0, 20);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, gobj4);
	auxNode->drawBBox = 0;
	NodeAttach(myNode, auxNode); // takes ownership

	SetTransTrfm3D(&TT, 0, -10, 00);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, gobj5);
	auxNode->drawBBox = 0;
	SetShaderNode(auxNode, FindShaderScene("pervertex_notex"));
	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

Node *create_scene_city() {

	// casita3/house01
	// casa5/wachhaus
	// dom/dom

	Node * aux;
	list *gObj_list;
	Node *myNode;
	static trfm3D TT;

	gObj_list = CreateVoidList();

	AddLast(gObj_list, SceneRegisterGObject( "./obj/casita3/", "house01.obj"));
	AddLast(gObj_list, SceneRegisterGObject( "./obj/casa5/", "wachhaus.obj"));
	AddLast(gObj_list, SceneRegisterGObject( "./obj/dom/", "dom.obj"));

	aux = create_city(500, gObj_list);

	SetTransTrfm3D(&TT, 0, -5, 00);
	//SetTransTrfm3D(&TT, 0, 0, 0);
	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
	SetShaderNode(myNode, FindShaderScene("pervertex"));
	NodeAttachScene(myNode);
	NodeAttach(myNode, aux);

	return myNode;
}

Node *create_scene_noT() {

	GObject *gobj;
	trfm3D TT;
	Node *myNode, *auxNode;

	gobj = SceneRegisterGObject( "./obj/chapel/", "chapel_I.obj");

	SetTransTrfm3D(&TT, 0, 0, 0);
	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
	SetShaderNode(myNode, FindShaderScene("dummy"));
	NodeAttachScene(myNode);

	SetTransTrfm3D(&TT, 1, 0, 0);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, gobj);
	NodeAttach(myNode, auxNode); // takes ownership

	SetTransTrfm3D(&TT, -1, 0, 0);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, gobj);
	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

Node *create_scene_triang() {

	// sceneRegisterGObject takes ownership of geometric objects
	//gobj = SceneRegisterGObject( "./obj/cubes/", "triangle.obj");

	/*   auxNode = CreateNodeGobj(gobj, &T); */
	/*   sceneNodeAttach(auxNode); // takes ownership */

	static trfm3D TT;
	Node *myNode, *auxNode;


	SetTransTrfm3D(&TT, 0, -10, -100);
	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
	SetShaderNode(myNode, FindShaderScene("pervertex_notex"));
	NodeAttachScene(myNode);

	SetTransTrfm3D(&TT, -5, 0, 0);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, SceneRegisterGObject( "./obj/cubes/", "triangle.obj"));
	NodeAttach(myNode, auxNode); // takes ownership

	SetTransTrfm3D(&TT, 5, 0, 0);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, SceneRegisterGObject( "./obj/cubes/", "quad.obj"));
	NodeAttach(myNode, auxNode); // takes ownership

	SetTransTrfm3D(&TT, 10, 0, 0);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, SceneRegisterGObject( "./obj/chapel/", "chapel.obj"));
	SetShaderNode(auxNode, FindShaderScene("pervertex"));
	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

/////////////////////////// Create city functions

struct coord2d {
	int x;
	int y;
};

static hash *gen_coords(size_t maxX, size_t maxY, size_t N) {

	hash *coords;
	coords = CreateVoidHash();

	size_t key_N = sizeof(struct coord2d);
	size_t inserted = 0;
	struct coord2d *aux;

	while(inserted < N) {
		aux = malloc(sizeof *aux);
		do {
			aux->x = rand() % maxX;
			aux->y = rand() % maxY;
		} while(FindHashFreeElement(coords, (char *) aux, key_N) != NULL);
		InsertHashFreeElement(coords, (char *) aux, key_N, aux);
		inserted++;
	}
	return coords;
}

static Node *create_city (int N, list *gObj_list) {

	size_t gObj_n;
	GObject **gObj_arr;
	GObject *gObj_sel;
	hash *coords;
	struct coord2d * coord;
	struct coord2d coord_center;

	size_t maxX = 200;
	size_t maxY = 200;
	float bbsize = 5.0f;

	trfm3D *placement;
	Node *root, *auxNode;

	gObj_n = ElementsInList(gObj_list);
	gObj_arr = malloc(sizeof(*gObj_arr) * gObj_n);
	List2Array(gObj_list, (void **) gObj_arr);

	// elements have occupy ~ (5x5) units

	// generate coords (without repetition)
	coords = gen_coords(maxX, maxY, N);

	// Create Nodes
	coord_center.x = (int)(floor((float) maxX / 2.0f));
	coord_center.y = (int)(floor((float) maxY / 2.0f));
	placement = CreateTrfm3D();
	root = CreateNode();
	SetTrfmNode(root, placement);
	for(coord = StartLoopHash(coords); coord; coord = GetNextHash(coords)) {
		placement = CreateTrfm3D();
		SetTransTrfm3D(placement,
					   (coord->x - coord_center.x) * bbsize,
					   0,
					   (coord_center.y - coord->y) * bbsize);
		// get one gObj at random
		gObj_sel = gObj_arr[ rand() % gObj_n];
		auxNode = CreateNode();
		SetTrfmNode(auxNode, placement);
		SetGobjNode(auxNode, gObj_sel);
		NodeAttach(root, auxNode);
	}

	// Destroy coord hash
	for(coord = StartLoopHash(coords); coord; coord = GetNextHash(coords))
		free(coord);
	DestroyHash(&coords);
	free(gObj_arr);
	return root;
}
