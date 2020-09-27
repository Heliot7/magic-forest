#include "headers/ui/PopUpMenu.h"

#include <QDesktopWidget>

// Constructor
PopUpMenu::PopUpMenu(QWidget* parent) : QDialog(parent)
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
PopUpMenu::~PopUpMenu()
{
}

/* SLOTS */
// Closes the dialog
void PopUpMenu::on_buttonBack_clicked()
{
	emit goBackGame();
}	

// Exits the current game
void PopUpMenu::on_buttonExit_clicked()
{
	emit exitGame();
}
