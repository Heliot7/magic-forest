#include "headers/ui/PopUpEndGame.h"

#include <QDesktopWidget>

// Constructor
PopUpEndGame::PopUpEndGame(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	QDesktopWidget *d = QApplication::desktop();
	int w = d->width();
	int h = d->height();
	int mw = width();
	int mh = height();
	int cw = (w/2) - (mw/2);
	int ch = (h/2) - (mh/2);
	move(cw,ch);
}

// Destroyer
PopUpEndGame::~PopUpEndGame()
{
}

void PopUpEndGame::updateText(int level, int exp, int enemies)
{
	labelLevel->setText(QString().setNum(level));
	labelExp->setText(QString().setNum(exp));
	labelEnemies->setText(QString().setNum(enemies));
}

/* SLOTS */
// Exits the current game
void PopUpEndGame::on_buttonExit_clicked()
{
	emit exitGame();
}
