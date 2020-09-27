#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>
#include <vector>

#include "headers/logic/Character.h"

#define AVATAR_POS_X 0.0f
#define AVATAR_POS_Y 0.0f
#define AVATAR_POS_Z -5.0f
#define AVATAR_SPEED 20.0f

class Player : public Character
{
public:

	Player(Point pos = Point(AVATAR_POS_X, AVATAR_POS_Y, AVATAR_POS_Z), Vector aDir = Vector(), float aLife = 10.0f, 
		float aDamage = 1.0f, float mana = 3.0f, int exp = 0, bool isImmune = false);
	~Player();

	// Getters
	int getLevel() { return level; }
	float getMana();
	float getMaxMana();
	int getExp();
	int getMinExp() { return minExp; }
	int getMaxExp() { return maxExp; }
	bool isImmune();
	bool isImmuneHit();
	bool attacks();
	int getEnemiesDown() { return enemiesDown; }
	// Setters
	void levelUp();
	void setMana(float mana);
	void setMaxMana(float mana);
	void updateMana(float mana);
	void setExp(int exp);
	void addExp(int exp);
	void setMinExp(int exp) { minExp = exp; }
	void setMaxExp(int exp) { maxExp = exp; }
	void setImmune(bool immune);
	void setImmuneHit(bool immuneHit);
	void setAttack(bool attack);
	void incrEnemiesDown() { enemiesDown++; }

private:
	
	int level;
	float mana;
	float maxMana;
	int exp;
	int minExp, maxExp;
	bool immune;
	bool immuneHit;
	bool attack;

	int enemiesDown;

};

#endif
