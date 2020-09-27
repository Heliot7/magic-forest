#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <iostream>
#include <string>
#include <vector>

#include "headers/logic/Element.h"

class Obstacle : public Element
{
public:

	Obstacle(Point aPos = Point(), Vector aDir = Vector(), bool wall = true, float damage = 0.0f);
	~Obstacle();

	// Getters
	int isWall();
	float getDamage();
	btConvexHullShape* getShape() { return shape; }
	// Setters
	void setWall(int wall);
	void setDamage(float aDamage);
	void setShape(btConvexHullShape* hull) { shape = hull; }

private:

	int wall;
	float damage;
	btConvexHullShape *shape;

};

#endif
