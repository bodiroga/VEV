#ifndef _SCENES_H_
#define _SCENES_H_

#include "node.h"

// Dummy
Node *create_scene_noT();

// Per-vertex
Node *create_scene();
Node *create_cube_notex();
Node *create_cube();
Node *create_cubes();

// Per-fragment
Node *create_scene_fragment();
Node *create_cube_fragment_notex();
Node *create_cube_fragment();
Node *create_cubes_fragment();

// Bump mapping
Node *create_scene_bmap();
Node *create_cube_bmap();

// Projective texture
Node *create_scene_projective();
Node *create_cube_projective();

// City
Node *create_scene_city();
Node *create_scene_city_bmap();

// Triangle
Node *create_scene_triang();


#endif /* _CREATE_CITY_H_ */
