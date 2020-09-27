#include "headers/logic/GameInfo.h"
#include "headers/ui/MainWindow.h"
#include "headers/ui/Options.h"

#include <QFont>
#include <QFontDatabase>

// Constructor
Options::Options(QDialog* parent) : QDialog(parent)
{
	/*
	easy->setStyleSheet("#easy:hover { color: #FFAA00 }");
	medium->setStyleSheet("#medium:hover { color: #FFAA00 }");
	hard->setStyleSheet("#hard:hover { color: #FFAA00 }");
	*/

	QFontDatabase::addApplicationFont(":/fonts/russianquality.ttf");
	QFontDatabase::addApplicationFont(":/fonts/vivala.ttf");
	QFontDatabase::addApplicationFont(":/fonts/buschgardenz.ttf");
	QFontDatabase::addApplicationFont(":/fonts/neverwinter.ttf");
	setupUi(this);
	switch(GameInfo::Instance()->getDifficulty())
	{
		case EASY:
			easy->setStyleSheet("#easy { color: #FFAA00 }");
			break;
		case MEDIUM:
			medium->setStyleSheet("#medium { color: #FFAA00 }");
			break;
		case HARD:
			hard->setStyleSheet("#hard { color: #FFAA00 }");
			break;
	}

	if(GameInfo::Instance()->isShadows())
			shadows->setStyleSheet("#shadows { color: #FFAA00 }");
	else
		noShadows->setStyleSheet("#noShadows { color: #FFAA00 }");
}

// Destroyer
Options::~Options()
{
}

void Options::setSound(bool isSound)
{
	buttonSound->setChecked(isSound);
}

/* SLOTS */
// Closes the dialog
void Options::on_buttonBack_clicked()
{
	emit goBackMainMenuView(UI_MAIN);
}

void Options::on_buttonSound_clicked()
{
	GameInfo::Instance()->setSound(buttonSound->isChecked());
	emit soundChanged(buttonSound->isChecked());
}

void Options::on_easy_clicked()
{
	GameInfo::Instance()->setDifficulty(EASY);
	easy->setStyleSheet("#easy { color: #FFAA00 }");
	medium->setStyleSheet("#medium { color: #FFFFDD }");
	hard->setStyleSheet("#hard { color: #FFFFDD }");
}

void Options::on_medium_clicked()
{
	GameInfo::Instance()->setDifficulty(MEDIUM);
	medium->setStyleSheet("#medium { color: #FFAA00 }");
	easy->setStyleSheet("#easy { color: #FFFFDD }");
	hard->setStyleSheet("#hard { color: #FFFFDD }");
}

void Options::on_hard_clicked()
{
	GameInfo::Instance()->setDifficulty(HARD);
	hard->setStyleSheet("#hard { color: #FFAA00 }");
	medium->setStyleSheet("#medium { color: #FFFFDD }");
	easy->setStyleSheet("#easy { color: #FFFFDD }");
}

void Options::on_shadows_clicked()
{
	GameInfo::Instance()->setShadows(true);
	shadows->setStyleSheet("#shadows { color: #FFAA00 }");
	noShadows->setStyleSheet("#noShadows { color: #FFFFDD }");
}

void Options::on_noShadows_clicked()
{
	GameInfo::Instance()->setShadows(false);
	noShadows->setStyleSheet("#noShadows { color: #FFAA00 }");
	shadows->setStyleSheet("#shadows { color: #FFFFDD }");
}