#include "headers/logic/Player.h"

using namespace std;

// Constructor
Player::Player(Point aPos, Vector aDir, float aLife, float aDamage, float aMana, int aExp, bool aImmune): Character(aPos, aDir, aLife, aDamage)
{
	level = 1;
	mana = 3;
	maxMana = 3;
	exp = 0;
	minExp = 0;
	maxExp = 5;
	immune = false;
	immuneHit = false;
	attack = false;
	enemiesDown = 0;
}

// Destructor
Player::~Player()
{	
}

/* Getters */
float Player::getMana()
{
	return mana;
}

float Player::getMaxMana()
{
	return maxMana;
}

int Player::getExp()
{
	return exp;
}

bool Player::isImmune()
{
	return immune;
}

bool Player::isImmuneHit()
{
	return immuneHit;
}

bool Player::attacks()
{
	return attack;
}

/* Setters */
void Player::levelUp()
{
	// 1 level more
	level ++;
	// Update life
	maxLife = maxLife + sqrt((float)exp + maxLife/10*( (float)rand()/RAND_MAX - 0.5 ) );
	life += maxLife/4;
	if(life > maxLife)
		life = maxLife;
	// Update mana
	maxMana = maxMana + sqrt((float)enemiesDown + maxMana/10*( (float)rand()/RAND_MAX - 0.5 ) );
	mana += maxMana/5;
	if(mana > maxMana)
		mana = maxMana;
	// Update experience
	minExp = maxExp;
	maxExp = maxExp*2;
}
void Player::setMana(float aMana)
{
	mana = aMana;
}

void Player::setMaxMana(float aMana)
{
	maxMana = aMana;
}

void Player::updateMana(float aMana)
{
	mana += aMana;
	if(mana > maxMana)
		mana = maxMana;
}

void Player::setExp(int aExp)
{
	exp = aExp;
}

void Player::addExp(int addExp)
{
	exp += addExp;
}

void Player::setImmune(bool aImmune)
{
	immune = aImmune;
}

void Player::setImmuneHit(bool aImmuneHit)
{
	immuneHit = aImmuneHit;
}

void Player::setAttack(bool isAttacking)
{
	attack = isAttacking;
}

