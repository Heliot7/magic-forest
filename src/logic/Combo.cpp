#include "headers/logic/Combo.h"

using namespace std;

// Constructor
Combo::Combo(Point pos, Vector dir): Attack(pos, dir)
{
	
}

// Destructor
Combo::~Combo()
{
	
}

/* Getters */
float Combo::getAoE()
{
	return AoE;
}

int Combo::getTime()
{
	return time;
}

/* Setters */
void Combo::setAoE(float aAoE)
{
	AoE = aAoE;
}

void Combo::setTime(int aTime)
{
	time = aTime;
}

