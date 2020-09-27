#include "headers/ui/MainWindow.h"
#include "headers/ui/Info.h"
#include "headers/logic/GameInfo.h"

#include <QFont>
#include <QFontDatabase>

// Constructor
Info::Info(QDialog* parent) : QDialog(parent)
{
	QFontDatabase::addApplicationFont(":/fonts/russianquality.ttf");
	QFontDatabase::addApplicationFont(":/fonts/vivala.ttf");
	QFontDatabase::addApplicationFont(":/fonts/buschgardenz.ttf");
	QFontDatabase::addApplicationFont(":/fonts/neverwinter.ttf");
	setupUi(this);
}

void Info::setSound(bool isSound)
{
	buttonSound->setChecked(isSound);
}

// Destroyer
Info::~Info()
{
}

/* SLOTS */
// Closes the dialog
void Info::on_buttonBack_clicked()
{
	emit goBackMainMenuView(UI_MAIN);
}

void Info::on_buttonSound_clicked()
{
	GameInfo::Instance()->setSound(buttonSound->isChecked());
	emit soundChanged(buttonSound->isChecked());
}