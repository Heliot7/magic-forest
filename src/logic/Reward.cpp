#include "headers/logic/Reward.h"

using namespace std;

// Constructor
Reward::Reward(Point aPos, Vector aDir, float aValue): Element(aPos, aDir), value(aValue)
{
}

// Destructor
Reward::~Reward()
{	
}

/* Getters */
float Reward::getValue()
{
	return value;
}

/* Setters */
void Reward::setValue(float aValue)
{
	value = aValue;
}

