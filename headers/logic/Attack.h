#ifndef ATTACK_H
#define ATTACK_H

#include <iostream>
#include <string>
#include <vector>

#include "headers/render/Geometry.h"
#include "headers/logic/Element.h"

class Attack : public Element
{
public:

	Attack(Point aPos = Point(), Vector aDir = Vector());

	~Attack() {}

	// Getters
	float getDamage();
	// Setters
	void setDamage(float aDam);

protected:

	float damage;
};

#endif
