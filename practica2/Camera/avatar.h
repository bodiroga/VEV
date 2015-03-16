#ifndef AVATAR_H
#define AVATAR_H

#include "camera.h"
#include "bsphere.h"
#include "trfm3D.h"

typedef struct _Avatar {

	camera   *cam;
	BSphere  *bsph;
	int      walk; // 1 walk (default); 0 fly
} Avatar;

void    DestroyAvatar ( Avatar **theAvatar );

/**
 * Create an avatar, linked to a camera and with a bounding box (sphere) or radius radius
 *
 * @param theCamera camera attached to the avatar
 * @param radius radius of bounding sphere
 *
 * @return the new avatar
 */

Avatar *CreateAvatar( camera *theCamera, float radius);

/**
 * Set avatar on walk or fly mode
 *
 * @param theAvatar the avatar
 * @param walkOrFly 1 -> walk, 0 -> fly
 *
 * @return previous state
 */

int WalkOrFlyAvatar(Avatar *theAvatar, int walkOrFly);

/**
 * Advance the avatar if possible (no collisions). Walk or fly depending on status.
 *
 * @param theAvatar the avatar
 * @param step unit to advance
 *
 * @return true if avatar moves. false (0) if not
 */

int AdvanceAvatar(Avatar *theAvatar, float step);

/**
 * Avatar look left/right (depending on sign of angle)
 *
 * @param theAvatar the Avatar to rotate
 * @param angle positive means rotate to the right. Negative rotate to the left.
 */

void LeftRightAvatar(Avatar *theAvatar, float angle);

/**
 * Avatar look up/down (depending on sign of angle)
 *
 * @param theAvatar the Avatar to rotate
 * @param angle positive means rotate looking up. Negative rotate looking down.
 */
void UpDownAvatar(Avatar *theAvatar, float angle);

#endif /* AVATAR_H */
