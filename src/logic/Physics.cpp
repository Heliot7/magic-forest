#include <iostream>
#include <cmath>

#include "headers/kinect/Kinect.h"
#include "headers/logic/Physics.h"
#include "headers/render/Scene.h"


// Returns if the bullet impacts the given element
bool isCollisionBullet(Point t1, Point t2)
{
	Vector distance = t1-t2;
	distance.y = 0;
	if (distance.length() < 0.5 + 0.5)
		return true;
	return false;
}

// Returns true if both BB collide
bool isCollision(Box b1, Point t1, Box b2, Point t2)
{
	Vector distance = t1-t2;
	if (fabs(distance.x) < b1.distX()/2 + b2.distX()/2 && fabs(distance.y) < b1.distY()/2 + b2.distY()/2 && fabs(distance.z) < b1.distZ()/2 + b2.distZ()/2)
		return true;
	return false;
}

bool isCollision(vector<Element*> legoParts, bool *colParts, Box b2, Point t2)
{
	int i = 0;
	bool result = false;
	for (vector<Element*>::iterator it = legoParts.begin(); it!=legoParts.end(); ++it) 
	{
		Box b1 = (*it)->getModel()->boundingBox();
		Point t1 = (*it)->getPos();
		Vector distance = t1-t2;
		if (fabs(distance.x) < b1.distX()/2 + b2.distX()/2 && fabs(distance.y) < b1.distY()/2 + b2.distY()/2 && fabs(distance.z) < b1.distZ()/2 + b2.distZ()/2)
		{
			colParts[i] = true;
			result = true;
		}
		i++;
	}

	return result;
}

bool isCollision(vector<Element*> legoParts, bool *colParts, btDynamicsWorld *world, Element* object)
{
	int i = 0;
	bool result = false;
	for (vector<Element*>::iterator it = legoParts.begin(); it!=legoParts.end(); ++it) 
	{
		Element* bodyPart = *it;

		CollisionCallback collisionCallBack;
		collisionCallBack.collision = false;

		btTransform trans = bodyPart->getBody()->getCenterOfMassTransform();			
		btVector3 pos = bodyPart->getBody()->getCenterOfMassPosition();
		Point pPos;
		pPos.x = trans.getOrigin().getX();
		pPos.y = trans.getOrigin().getY();
		pPos.z = trans.getOrigin().getZ();
		pPos.x = pos.getX(); //trans.getOrigin().getX();
		pPos.y = pos.getY(); //trans.getOrigin().getY();
		pPos.z = pos.getZ(); //trans.getOrigin().getZ();

		world->contactPairTest(bodyPart->getBody(), object->getBody(), collisionCallBack);
		//if(object->getType() == ENE_TREE && !collisionCallBack.collision)
		//	world->contactPairTest(bodyPart->getBody(), ((NPC*)bodyPart)->left, collisionCallBack);		
		//if(object->getType() == ENE_TREE && !collisionCallBack.collision)
		//	world->contactPairTest(bodyPart->getBody(), ((NPC*)bodyPart)->right, collisionCallBack);		
		if(collisionCallBack.collision)
		{
			//cout << "Collision!!!!!!!!!!!!!!!!! " << i << endl;
			colParts[i] = true;
			result = true;
		}
		i++;
	}
	return result;
}

btQuaternion angle(btVector3& v1, btVector3& v2)
{	
	float d = v1.dot(v2);
	btVector3 axis = v1;
	axis.cross(v2);
	float qw = (float)sqrt(v1.length2() * v2.length2()) + d;
	
	// Vectors are 180 degrees apart
	if (qw < 0.0001)
	{ 
		btQuaternion quad = btQuaternion(v1.x(), v1.y(), -v1.z(), 0);
		return quad.normalized();
	}

	btQuaternion q = btQuaternion(axis.x(), axis.y(), axis.z(), qw); 
	return q.normalized();
}
