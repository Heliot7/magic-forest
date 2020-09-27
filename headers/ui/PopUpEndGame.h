#ifndef _POPUPENDGAME_H_
#define _POPUPENDGAME_H_

#include "ui_PopUpEndGame.h"

class PopUpEndGame : public QDialog, private Ui::PopUpEndGame
{
	Q_OBJECT

public:

	PopUpEndGame(QWidget* parent = 0);
	~PopUpEndGame();

	void updateText(int, int, int);

protected:

private:
	
signals:

	void exitGame();

public slots:

protected slots:
	
	void on_buttonExit_clicked();

private slots:

};

#endif
