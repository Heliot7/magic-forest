#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <string>

using namespace std;

enum Diff { EASY, MEDIUM, HARD };

class GameInfo
{

  public:

	static GameInfo* Instance(Diff diff = MEDIUM, bool sound = true, bool shadows = true);
	~GameInfo();
	// Getters
	Diff getDifficulty() { return difficulty; }
	bool isSound() { return sound; }
	bool isShadows() { return shadows; }
	// Setters
	void setDifficulty(Diff diff) { difficulty = diff; }
	void setSound(bool s) { sound = s; }
	void setShadows(bool s) { shadows = s; }

protected:

	GameInfo(Diff diff, bool sound, bool shadows);

private:

	static GameInfo* pInstance;

	// Attributes
	Diff difficulty;
	bool sound;
	bool shadows;
};

#endif

