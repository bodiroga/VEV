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
static Node *create_floor_city();

////////////////////////////////////////////////////////////////
//
// Dummy
//
////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////
//
// Per-vertex shading
//
////////////////////////////////////////////////////////////////

// Scene with per-vertex shader: with/without texture
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

// Cube with per-vertex shader: without texture
Node *create_cube_notex() {
  	GObject *gobj;
  	trfm3D TT;
  	Node *myNode, *auxNode;

	gobj = SceneRegisterGObject("./obj/cubes/", "cubo.obj");

  	SetTransTrfm3D(&TT, 0, -10, -100);
  	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
  	SetShaderNode(myNode, FindShaderScene("pervertex_notex"));
  	NodeAttachScene(myNode);

	// Cubo sin textura
  	SetTransTrfm3D(&TT, 0, 0, 0);
	AddRot_Y_Trfm3D(&TT, -0.35);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj);
  	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

// Cube with per-vertex shader: with texture
Node *create_cube() {
  	GObject *gobj;
  	trfm3D TT;
  	Node *myNode, *auxNode;

	gobj = SceneRegisterGObject("./obj/cubes/", "cubotex.obj");

  	SetTransTrfm3D(&TT, 0, -10, -100);
  	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
  	SetShaderNode(myNode, FindShaderScene("pervertex"));
  	NodeAttachScene(myNode);

	// Cubo con textura
  	SetTransTrfm3D(&TT, 0, 0, 0);
	AddRot_Y_Trfm3D(&TT, -0.35);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj);
  	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

// Cubes with per-vertex shader: with & without texture
Node *create_cubes() {
  	GObject *gobj1, *gobj2, *gobj3;
  	trfm3D TT;
  	Node *myNode, *auxNode;

	gobj1 = SceneRegisterGObject("./obj/cubes/", "cubotex.obj");
	gobj2 = SceneRegisterGObject("./obj/cubes/", "cubo.obj");
	gobj3 = SceneRegisterGObject("./obj/floor/", "floor.obj");

  	SetTransTrfm3D(&TT, 0, -10, -100);
	AddRot_Y_Trfm3D(&TT, 0.5);
	AddRot_X_Trfm3D(&TT, 0.8);
  	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
  	SetShaderNode(myNode, FindShaderScene("pervertex"));
  	NodeAttachScene(myNode);

	// Cubo con textura
  	SetTransTrfm3D(&TT, 0, 0, 20);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj1);
  	NodeAttach(myNode, auxNode); // takes ownership

	// Cubo sin textura
  	SetTransTrfm3D(&TT, 0, 0, -20);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj2);
  	SetShaderNode(auxNode, FindShaderScene("pervertex_notex"));
  	NodeAttach(myNode, auxNode); // takes ownership

	// Suelo
  	SetTransTrfm3D(&TT, 0, -10, 0);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj3);
  	SetShaderNode(auxNode, FindShaderScene("pervertex_notex"));
  	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}


////////////////////////////////////////////////////////////////
//
// Per-fragment shading
//
////////////////////////////////////////////////////////////////

// Scene with per-fragment shader: with/without texture
Node *create_scene_fragment() {
	  GObject *gobj1;
	  GObject *gobj2;
	  GObject *gobj3;
	  GObject *gobj4;
	  GObject *gobj5;
	  Node    *myNode, *auxNode;

	  static trfm3D TT;

	  // sceneRegisterGObject takes ownership of geometric objects
	  gobj1 =  SceneRegisterGObject( "./obj/spheres/", "smooth.obj");
	  gobj2 = SceneRegisterGObject( "./obj/chapel/", "chapel.obj");
	  gobj3 = SceneRegisterGObject( "./obj/cubes/", "cubo.obj");
	  gobj4 = SceneRegisterGObject( "./obj/cubes/", "cubotex.obj");
	  gobj5 = SceneRegisterGObject( "./obj/floor/", "floor.obj");

	  SetTransTrfm3D(&TT, 0, -10, -100);
	  myNode = CreateNode();
	  SetTrfmNode(myNode, &TT);
	  NodeAttachScene(myNode); // takes ownership
	  SetShaderNode(myNode, FindShaderScene("perfragment"));
	  myNode->drawBBox = 0;

	  // Esfera
	  SetTransTrfm3D(&TT, -20, 0, 0);
	  auxNode = CreateNode();
	  SetTrfmNode(auxNode, &TT);
	  SetGobjNode(auxNode, gobj1);
	  SetShaderNode(auxNode, FindShaderScene("perfragment_notex"));
	  auxNode->drawBBox = 0;
	  NodeAttach(myNode, auxNode); // takes ownership

	  // Iglesia
	  SetTransTrfm3D(&TT, 20, 0, 0);

	  auxNode = CreateNode();
	  SetTrfmNode(auxNode, &TT);
	  SetGobjNode(auxNode, gobj2);
	  auxNode->drawBBox = 0;
	  NodeAttach(myNode, auxNode); // takes ownership

	  // Cubo sin textura
	  SetTransTrfm3D(&TT, 0, 0, -20);
	  auxNode = CreateNode();
	  SetTrfmNode(auxNode, &TT);
	  SetGobjNode(auxNode, gobj3);
	  SetShaderNode(auxNode, FindShaderScene("perfragment_notex"));
	  auxNode->drawBBox = 0;
	  NodeAttach(myNode, auxNode); // takes ownership

	  // Cubo con textura
	  SetTransTrfm3D(&TT, 0, 0, 20);
	  auxNode = CreateNode();
	  SetTrfmNode(auxNode, &TT);
	  SetGobjNode(auxNode, gobj4);
	  auxNode->drawBBox = 0;
	  NodeAttach(myNode, auxNode); // takes ownership

	  // Suelo
	  SetTransTrfm3D(&TT, 0, -10, 00);
	  auxNode = CreateNode();
	  SetTrfmNode(auxNode, &TT);
	  SetGobjNode(auxNode, gobj5);
	  auxNode->drawBBox = 0;
	  SetShaderNode(auxNode, FindShaderScene("perfragment_notex"));
	  NodeAttach(myNode, auxNode); // takes ownership

	  return myNode;
}


// Cube with per-fragment shader: without texture
Node *create_cube_fragment_notex() {
  	GObject *gobj;
  	trfm3D TT;
  	Node *myNode, *auxNode;

	gobj = SceneRegisterGObject("./obj/cubes/", "cubo.obj");

  	SetTransTrfm3D(&TT, 0, -10, -100);
  	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
  	SetShaderNode(myNode, FindShaderScene("perfragment"));
  	NodeAttachScene(myNode);

	// Cubo sin textura
  	SetTransTrfm3D(&TT, 0, 0, 0);
	AddRot_Y_Trfm3D(&TT, -0.35);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj);
  	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

// Cube with per-fragment shader: with texture
Node *create_cube_fragment() {
  	GObject *gobj;
  	trfm3D TT;
  	Node *myNode, *auxNode;

	gobj = SceneRegisterGObject("./obj/cubes/", "cubotex.obj");

  	SetTransTrfm3D(&TT, 0, -10, -100);
  	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
  	SetShaderNode(myNode, FindShaderScene("perfragment"));
  	NodeAttachScene(myNode);

	// Cubo con textura
  	SetTransTrfm3D(&TT, 0, 0, 0);
	AddRot_Y_Trfm3D(&TT, -0.35);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj);
  	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

// Cubes with per-fragment shader: with & without texture
Node *create_cubes_fragment() {
  	GObject *gobj1, *gobj2, *gobj3;
  	trfm3D TT;
  	Node *myNode, *auxNode;

	gobj1 = SceneRegisterGObject("./obj/cubes/", "cubotex.obj");
	gobj2 = SceneRegisterGObject("./obj/cubes/", "cubo.obj");
	gobj3 = SceneRegisterGObject("./obj/floor/", "floor.obj");

  	SetTransTrfm3D(&TT, 0, -10, -100);
	AddRot_Y_Trfm3D(&TT, 0.5);
	AddRot_X_Trfm3D(&TT, 0.8);
  	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
  	SetShaderNode(myNode, FindShaderScene("perfragment"));
  	NodeAttachScene(myNode);

	// Cubo con textura
  	SetTransTrfm3D(&TT, 0, 0, 20);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj1);
  	NodeAttach(myNode, auxNode); // takes ownership

	// Cubo sin textura
  	SetTransTrfm3D(&TT, 0, 0, -20);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj2);
  	SetShaderNode(auxNode, FindShaderScene("perfragment_notex"));
  	NodeAttach(myNode, auxNode); // takes ownership

	// Suelo
  	SetTransTrfm3D(&TT, 0, -10, 0);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj3);
  	SetShaderNode(auxNode, FindShaderScene("perfragment_notex"));
  	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}


////////////////////////////////////////////////////////////////
//
// Bump mapping (floor)
//
////////////////////////////////////////////////////////////////

// Scene with bump mapping: floor
Node *create_scene_bmap() {

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
	gobj3 = SceneRegisterGObject( "./obj/cubes/", "cuboBMtex.obj");
	gobj4 = SceneRegisterGObject( "./obj/cubes/", "cubotex.obj");
	gobj5 = SceneRegisterGObject( "./obj/floor/", "waterfloor.obj");

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
	SetShaderNode(auxNode, FindShaderScene("bump"));
	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

// Cube with bump mapping
Node *create_cube_bmap() {
  	GObject *gobj;
  	trfm3D TT;
  	Node *myNode, *auxNode;
	
	gobj = SceneRegisterGObject("./obj/cubes/", "cuboBMtex.obj");

  	SetTransTrfm3D(&TT, 0, -10, -100);
  	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
	SetShaderNode(myNode, FindShaderScene("bump"));
  	NodeAttachScene(myNode);

	// Cubo con textura
  	SetTransTrfm3D(&TT, 0, 0, 0);
	AddRot_Y_Trfm3D(&TT, -0.35);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj);
  	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

////////////////////////////////////////////////////////////////
//
// Projective texture
//
////////////////////////////////////////////////////////////////

// Scene with projective texture
Node *create_scene_projective() {

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
	gobj5 = SceneRegisterGObject( "./obj/floor/", "simplefloor.obj");

	SetTransTrfm3D(&TT, 0, -10, -100);
	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
	NodeAttachScene(myNode); // takes ownership
	SetShaderNode(myNode, FindShaderScene("projective"));
	myNode->drawBBox = 0;

	SetTransTrfm3D(&TT, -20, 0, 0);
	auxNode = CreateNode();
	SetTrfmNode(auxNode, &TT);
	SetGobjNode(auxNode, gobj);
	//SetShaderNode(auxNode, FindShaderScene("pervertex_notex"));
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
	//SetShaderNode(auxNode, FindShaderScene("pervertex_notex"));
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
	//SetShaderNode(auxNode, FindShaderScene("pervertex_notex"));
	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}

// Cube with projective texture
Node *create_cube_projective() {
  	GObject *gobj1, *gobj2;
  	trfm3D TT;
  	Node *myNode, *auxNode;

	gobj1 = SceneRegisterGObject("./obj/cubes/", "cubotex.obj");
	gobj2 = SceneRegisterGObject("./obj/floor/", "simplefloor.obj");

  	SetTransTrfm3D(&TT, 0, -10, -100);
  	myNode = CreateNode();
	SetTrfmNode(myNode, &TT);
  	SetShaderNode(myNode, FindShaderScene("projective"));
  	NodeAttachScene(myNode);

	// Cubo con textura
  	SetTransTrfm3D(&TT, 0, 0, 20);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj1);
  	NodeAttach(myNode, auxNode); // takes ownership

	// Suelo
  	SetTransTrfm3D(&TT, 0, -10, 0);
  	auxNode = CreateNode();
  	SetTrfmNode(auxNode, &TT);
  	SetGobjNode(auxNode, gobj2);
	NodeAttach(myNode, auxNode); // takes ownership

	return myNode;
}


////////////////////////////////////////////////////////////////
//
// City
//
////////////////////////////////////////////////////////////////

// City (per vertex with texture)
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

	aux = create_floor_city( "./obj/floor/", "cityfloor.obj");
	SetShaderNode(aux, FindShaderScene("pervertex_notex"));
	NodeAttach(myNode, aux); // takes ownership
	return aux;
}

// City (per vertex with texture / bump mapping floor)
Node *create_scene_city_bmap() {

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

	aux = create_floor_city( "./obj/floor/", "cityfloor_grass.obj");
	SetShaderNode(aux, FindShaderScene("bump"));
	NodeAttach(myNode, aux); // takes ownership
	return aux;
}

////////////////////////////////////////////////////////////////
//
// Triangle
//
////////////////////////////////////////////////////////////////

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

static Node *create_floor_city(char *dir, char *fname) {

	Node *myNode, *aux;
	static trfm3D TT;
	GObject *gobj;
	static float floorsize = 108.0;
	int i, j;
	int N = 12; // 5x5 patches
	float left, up;
	float x, z;

	gobj = SceneRegisterGObject(dir, fname);

	myNode = CreateNode();
	SetTransTrfm3D(&TT, 0.0, -1.5, 00);
	SetTrfmNode(myNode, &TT);

	left = -1.0f * floorsize * (float) N / 2.0f;

	for (i = 0; i < N; i++) {
		x = left + floorsize * i;
		for(j = 0; j < N; j++) {
			z = left + floorsize * j;
			SetTransTrfm3D(&TT, x, 0.0, z);
			aux = CreateNode();
			SetTrfmNode(aux, &TT);
			SetGobjNode(aux, gobj);
			aux->drawBBox = 0;
			NodeAttach(myNode, aux); // takes ownership
		}
	}

	return myNode;
}

