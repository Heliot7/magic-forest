#ifndef _REWARD_H
#define _REWARD_H

#include <iostream>
#include <string>
#include <vector>

#include "headers/logic/Element.h"

class Reward : public Element
{
public:

	Reward(Point pos = Point(), Vector aDir = Vector(), float aValue = 0.0f);
	~Reward();

	// Getters
	float getValue();
	// Setters
	void setValue(float aValue);

private:
	float value;

};

#endif
