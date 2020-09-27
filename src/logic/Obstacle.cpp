#include "headers/logic/Obstacle.h"

using namespace std;

// Constructor
Obstacle::Obstacle(Point aPos, Vector aDir, bool aIsWall, float aDamage): Element(aPos, aDir), wall(aIsWall), damage(aDamage)
{
}

// Destructor
Obstacle::~Obstacle()
{
}

/* Getters */
int Obstacle::isWall()
{
	return wall;
}

float Obstacle::getDamage()
{
	return damage;
}

/* Setters */
void Obstacle::setWall(int aIsWall)
{
	wall = aIsWall;
}

void Obstacle::setDamage(float aDamage)
{
	damage = aDamage;
}

