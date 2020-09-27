#include "headers/logic/GameInfo.h"

GameInfo* GameInfo::pInstance = NULL;

GameInfo* GameInfo::Instance(Diff diff, bool sound, bool shadows)
{
	if(pInstance == NULL)
	{
		pInstance = new GameInfo(diff, sound, shadows);
	}
	return pInstance;
}

// Constructor
GameInfo::GameInfo(Diff diff, bool aSound, bool aShadows)
{
	difficulty = diff;
	sound = aSound;
	shadows = aShadows;
}