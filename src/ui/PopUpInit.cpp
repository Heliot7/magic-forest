#include "headers/ui/PopUpInit.h"
#include "headers/kinect/Kinect.h"

#include <QDesktopWidget>

// Constructor
PopUpInit::PopUpInit(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
        // setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
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
PopUpInit::~PopUpInit()
{
}

void PopUpInit::updateText(int num)
{
	switch(num)
	{
		case TRACKING:
		{
			label->setText("Kinect is tracking...");
			break;
		}
		case CALIBRATING:
		{
			label->setText("Kinect is calibrating...");
			break;
		}
		case NIL:
		case POSING:
		{
			label->setText("Kinect is looking for pose...");
			break;
		}
	}
}
