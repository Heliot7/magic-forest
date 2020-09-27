#include "headers/logic/NPC.h"

using namespace std;

// Constructor
NPC::NPC(Point aPos, Vector aDir, float aLife, float aDamage, bool isRanged): Character(aPos, aDir, aLife, aDamage)
{
	direction = aDir;
	range = isRanged;
	expPoints = 1;
	up = false;
	hit = 0;
	rotLeft = (float)(rand()%180);
	rotRight = (float)(rand()%180);
}

// Destructor
NPC::~NPC()
{
	
}

/* Getters */
Vector NPC::getDir()
{
	return direction;
}

bool NPC::isRanged()
{
	return range;
}

/* Setters */
void NPC::setDir(Vector aDir)
{
	direction = aDir;
}

void NPC::setRanged(bool isRanged)
{
	isRanged = range;
}

