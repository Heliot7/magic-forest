#include <iostream>
#include <fstream>
// #include <io.h>
#include <QFont>
#include <QFontDatabase>

#include "headers/logic/GameInfo.h"
#include "headers/ui/MainWindow.h"
#include "headers/ui/MainMenu.h"

// Constructor
MainMenu::MainMenu(QDialog* parent) : QDialog(parent)
{
	QFontDatabase::addApplicationFont(":/fonts/russianquality.ttf");
	QFontDatabase::addApplicationFont(":/fonts/vivala.ttf");
	setupUi(this);
}

// Destroyer
MainMenu::~MainMenu()
{
}

void MainMenu::setSound(bool isSound)
{
	buttonSound->setChecked(isSound);
}

/* SLOTS */
void MainMenu::on_buttonGame_clicked()
{
	emit changeView(UI_GAME);
}

void MainMenu::on_buttonScores_clicked()
{
	emit changeView(UI_SCORES);
}

void MainMenu::on_buttonOptions_clicked()
{
	emit changeView(UI_OPTIONS);
}

void MainMenu::on_buttonInfo_clicked()
{
	emit changeView(UI_INFO);
}

void MainMenu::on_buttonExit_clicked()
{
	std::cout << "Exit app..." << std::endl;
	exit(1);
}

void MainMenu::on_buttonSound_clicked()
{
	GameInfo::Instance()->setSound(buttonSound->isChecked());
	emit soundChanged(buttonSound->isChecked());
}
