#ifndef _MAINMENU_H_
#define _MAINMENU_H_

#include <QString>

#include "ui_MainMenu.h"

class MainMenu : public QDialog, private Ui::MainMenu
{
	Q_OBJECT

public:

	MainMenu(QDialog* parent = 0);
	~MainMenu();

	void setSound(bool isSound);

protected:

private:
	
signals:

	void changeView(int);
	void soundChanged(bool);

public slots:

protected slots:

	/** Menu button */
	void on_buttonGame_clicked();
	void on_buttonScores_clicked();
	void on_buttonOptions_clicked();
	void on_buttonInfo_clicked();

	/** Specific buttons */
	// Closes application
	void on_buttonExit_clicked();
	void on_buttonSound_clicked();

private slots:

};

#endif
