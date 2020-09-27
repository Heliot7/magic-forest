#ifndef _POPUPMENU_H_
#define _POPUPMENU_H_

#include "ui_PopUpMenu.h"

class PopUpMenu : public QDialog, private Ui::PopUpMenu
{
	Q_OBJECT

public:

	PopUpMenu(QWidget* parent = 0);
	~PopUpMenu();

protected:

private:
	
signals:

	void goBackGame();
	void exitGame();

public slots:

protected slots:
	
	void on_buttonBack_clicked();
	void on_buttonExit_clicked();

private slots:

};

#endif
