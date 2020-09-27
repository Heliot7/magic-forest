#ifndef _GAME_H_
#define _GAME_H_

#include "ui_Game.h"

class Game : public QDialog, private Ui::Game
{
	Q_OBJECT

public:

	Game(QDialog* parent = 0);
	~Game();

	void setSound(bool isSound);

protected:

private:

signals:

	void goBackMainMenuView(int);
	void goGame(int);
	void soundChanged(bool);

public slots:

protected slots:
	
	void on_buttonBack_clicked();
	void on_buttonSound_clicked();
	void on_buttonGame_clicked();

private slots:

};

#endif
