#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QStackedWidget>
#include <QString>

#include "ui_MainWindow.h"

#include "headers/ui/MainMenu.h"
#include "headers/ui/Game.h"
#include "headers/ui/Scores.h"
#include "headers/ui/Options.h"
#include "headers/ui/Info.h"
#include "headers/render/Scene.h"

#define UI_MAIN		0
#define UI_GAME		1
#define UI_SCORES	2
#define UI_OPTIONS	3
#define UI_INFO		4
#define UI_RENDER	5

class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

public:

	MainWindow(QWidget* parent = 0);
        ~MainWindow ();

protected:
	
private:

	QStackedWidget* myStack;
	MainMenu* uiMainMenu;
	Game* uiGame;
	Options* uiOptions;
        Scores* uiScores;
	Info* uiInfo;

	Scene* mScene;

	// In-game sound
        //irrklang::ISoundEngine *sound;
        Mix_Music *music;

        void loadMusic(Mix_Music *music, char *path, bool isSound, bool loop);

signals:

public slots:	

protected slots:

	void setView(int, bool byeGame = false);
	void setSound(bool isSound);

private slots:

};

#endif

