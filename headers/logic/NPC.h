#ifndef NPC_H
#define NPC_H

#include <iostream>
#include <string>
#include <vector>

#include "headers/logic/Character.h"

class NPC : public Character
{
public:

	NPC(Point pos = Point(), Vector aDir = Vector(), float aLife = 1.0f, float aDamage = 1.0f, bool isRanged = false);
	~NPC();

	// Getters
	Vector getDir();
	int getHit() { return hit; }
	bool isRanged();
	int getExpPoints() { return expPoints; }
	bool isUp() { return up; }
	float getRotLeft() { return rotLeft; }
	float getRotRight() { return rotRight; }
	// Setters
	void setDir(Vector aDir);
	void upgradeHit() { hit++; }
	void setDynamic(bool isDynamic);
	void setExpPoints(int points) { expPoints = points; }
	void setRanged(bool isRanged);
	void setUp(bool aup) { up = aup; }
	void setRotLeft(float left) { rotLeft = left; }
	void setRotRight(float right) { rotRight = right; }
	void updateRotLeft(float left) { rotLeft += left; }
	void updateRotRight(float right) { rotRight += right; }
	void setBodyLeft(btScalar mass, btDefaultMotionState* myMotionState, btCollisionShape* shape, btVector3& localInertia)
	{
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		left = new btRigidBody(rbInfo);
	}
	void setBodyRight(btScalar mass, btDefaultMotionState* myMotionState, btCollisionShape* shape, btVector3& localInertia)
	{
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		right = new btRigidBody(rbInfo);
	}

	btRigidBody *left;
	btRigidBody *right;

private:

	int hit;
	bool range;
	int expPoints;
	bool up;
	float rotLeft;
	float rotRight;

};

#endif
