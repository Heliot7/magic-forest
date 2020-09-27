#ifndef COMBO_H
#define COMBO_H

#include <iostream>
#include <string>
#include <vector>

#include "headers/logic/Attack.h"

class Combo : public Attack
{
public:

	Combo(Point pos = Point(), Vector dir = Vector());
	~Combo();

	// Getters
	float getAoE();
	int getTime();
	// Setters
	void setAoE(float aAoE);
	void setTime(int aTime);

private:
	float AoE;
	int time;

};

#endif
