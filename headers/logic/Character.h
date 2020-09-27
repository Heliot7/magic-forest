#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <string>
#include <list>

#include "headers/logic/Attack.h"
#include "headers/render/Geometry.h"
#include "headers/logic/Element.h"

class Character : public Element
{
public:
	
	Character(Point aPos = Point(), Vector aDir = Vector(), float aLife = 1.0f, float aDamage = 1.0f): Element(aPos, aDir)
	{
		life = aLife;
		maxLife = aLife;
		damage = aDamage;
		shoots = new list<Attack*>();
	}
	~Character() {}

	// Getters
	float getLife() { return life; }
	float getMaxLife() { return maxLife; }
	float getDamage() { return damage; }
	list<Attack*>* getShoots() { return shoots; }
	// Setters
	void setLife(float aLife) { life = aLife; }
	void setMaxLife(float aLife) { maxLife = aLife; }
	void updateLife(float offset) { life += offset; if(life > maxLife) life = maxLife; }
	void setDamage(float aDamage) { damage = aDamage; }
	void addShoot(Attack* bullet) { shoots->push_back(bullet); }

protected:

	float life;
	float maxLife;
	float damage;
	// ranged shoots of character
	list<Attack*> *shoots;
};

#endif
