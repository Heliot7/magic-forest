#include "headers/logic/Attack.h"

using namespace std;

// Constructor
Attack::Attack(Point aPos, Vector aDir): Element(aPos, aDir)
{
}

/* Getters */

float Attack::getDamage()
{
	return damage;
}

/* Setters */
void Attack::setDamage(float aDam)
{
	damage = aDam;
}
