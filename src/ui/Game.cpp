#include "headers/ui/MainWindow.h"
#include "headers/ui/Game.h"
#include "headers/logic/GameInfo.h"

#include <QFont>
#include <QFontDatabase>

// Constructor
Game::Game(QDialog* parent) : QDialog(parent)
{
	QFontDatabase::addApplicationFont(":/fonts/russianquality.ttf");
	QFontDatabase::addApplicationFont(":/fonts/vivala.ttf");
	QFontDatabase::addApplicationFont(":/fonts/buschgardenz.ttf");
	QFontDatabase::addApplicationFont(":/fonts/neverwinter.ttf");
	setupUi(this);
}

// Destroyer
Game::~Game()
{
}

void Game::setSound(bool isSound)
{
	buttonSound->setChecked(isSound);
}

/* SLOTS */
// Launches the render (QGLWidget)
void Game::on_buttonGame_clicked()
{
	emit goGame(UI_RENDER);
}

// Closes the dialog
void Game::on_buttonBack_clicked()
{
	emit goBackMainMenuView(UI_MAIN);
}

void Game::on_buttonSound_clicked()
{
	GameInfo::Instance()->setSound(buttonSound->isChecked());
	emit soundChanged(buttonSound->isChecked());
}