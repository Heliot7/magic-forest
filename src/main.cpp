#include <QApplication>
#include <QPixmap>
#include <QCursor>
#include <iostream>
#include <QDesktopWidget>

#include "headers/ui/MainWindow.h"

using namespace std;

/* MAIN */
int main( int argc, char ** argv )
{
	QApplication a( argc, argv );
	a.setOverrideCursor(QCursor(QPixmap(":/fonts/cursor2.gif"), 0, 0));

	cout << "Starting Application:" << endl;
	cout << "---------------------\n" << endl;
	cout << "==========================================" << endl;
	cout << "           Kinect Lab by Team 3          " << endl;
	cout << "             > RWTH AACHEN <              " << endl;
	cout << "==========================================" << endl;
	MainWindow window(NULL);
	a.setActiveWindow(&window);
	QDesktopWidget *d = a.desktop();
	int w = d->width();
	int h = d->height();
	int mw = window.width();
	int mh = window.height();
	int cw = (w/2) - (mw/2);
	int ch = (h/2) - (mh/2);
	window.move(cw,ch);
	window.showFullScreen();
	window.show();

	a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
	
	return a.exec();
}
