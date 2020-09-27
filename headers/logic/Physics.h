#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include "headers/logic/Element.h"
#include "headers/render/Geometry.h"

#include <LinearMath/btQuaternion.h>

bool isCollisionBullet(Point t1, Point t2);
bool isCollision(Box b1, Point t1, Box b2, Point t2);
bool isCollision(vector<Element*> legoParts, bool *colParts, Box b2, Point t2);
bool isCollision(vector<Element*> legoParts, bool *colParts, btDynamicsWorld *world, Element* object);
btQuaternion angle(btVector3& v1, btVector3& v2);

#endif
