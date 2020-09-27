#include <iostream>
#include <fstream>
// #include <io.h>

#include "headers/ui/MainWindow.h"
#include "headers/logic/GameInfo.h"

using namespace std;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setupUi(this);
	// showFullScreen();
	
	//Initialize main menu
	uiMainMenu = new MainMenu();
	uiGame = new Game();
        uiScores = new Scores();
	uiOptions = new Options();
	uiInfo = new Info();

	// Stack all menus
	myStack = new QStackedWidget();
	myStack->addWidget(uiMainMenu);
	myStack->addWidget(uiGame);
	myStack->addWidget(uiScores);
	myStack->addWidget(uiOptions);
	myStack->addWidget(uiInfo);

	// And set it as the main frame of the main window 
	// TODO: stack not appended -> QGLWidget loaded automatically (for tests)!
	//this->setCentralWidget(new Scene());
	this->setCentralWidget(myStack);
	
	// Connect menus with main window
	connect( uiMainMenu, SIGNAL( changeView(int) ), this, SLOT( setView(int) ) );
	connect( uiGame, SIGNAL( goBackMainMenuView(int) ), this, SLOT( setView(int) ) );
	connect( uiGame, SIGNAL( goGame(int) ), this, SLOT( setView(int) ) );
	connect( uiScores, SIGNAL( goBackMainMenuView(int) ), this, SLOT( setView(int) ) );
	connect( uiOptions, SIGNAL( goBackMainMenuView(int) ), this, SLOT( setView(int) ) );
	connect( uiInfo, SIGNAL( goBackMainMenuView(int) ), this, SLOT( setView(int) ) );
	// Connect sound to all interfaces
	connect( uiMainMenu, SIGNAL( soundChanged(bool) ), this, SLOT( setSound(bool) ) );
	connect( uiGame, SIGNAL( soundChanged(bool) ), this, SLOT( setSound(bool) ) );
	connect( uiScores, SIGNAL( soundChanged(bool) ), this, SLOT( setSound(bool) ) );
	connect( uiOptions, SIGNAL( soundChanged(bool) ), this, SLOT( setSound(bool) ) );
	connect( uiInfo, SIGNAL( soundChanged(bool) ), this, SLOT( setSound(bool) ) );

	// Sound
        //sound = irrklang::createIrrKlangDevice();
        initSound();
        cout << ":init sound" << endl;
	setSound(true);
}

MainWindow::~MainWindow()
{
	delete uiGame;
	delete uiScores;
	delete uiInfo;
	delete uiMainMenu;

        Mix_FreeMusic(music);
        music = NULL;
        Mix_CloseAudio();
        SDL_Quit();
        cout << ":release music" << endl;
}

/* SLOTS */
void MainWindow::setView(int iNewView, bool byeGame)
{
	if(iNewView == UI_RENDER)
	{
		mScene = new Scene();
		myStack->addWidget(mScene);
		connect( mScene, SIGNAL( exitGame(int, bool) ), this, SLOT( setView(int, bool) ) );
		connect( mScene, SIGNAL( updateScores()), uiScores, SLOT( updateTable() ) );
	}
	
	if(byeGame)
	{
		myStack->removeWidget(mScene);
		delete mScene;
	}

	myStack->setCurrentIndex(iNewView);
}

void MainWindow::setSound(bool isSound)
{
	uiGame->setSound(isSound);
	uiInfo->setSound(isSound);
	uiOptions->setSound(isSound);
	uiMainMenu->setSound(isSound);
	uiScores->setSound(isSound);
	if(isSound)
        {
            loadMusic(music, "/resources/sounds/background1.ogg", GameInfo::Instance()->isSound(), true);
        }
	else
        {
            Mix_HaltMusic();
            cout << ":stop sound" << endl;
        }
}

void MainWindow::loadMusic(Mix_Music *music, char *path, bool isSound, bool loop)
{
        if(isSound)
        {
                QString pathTestObj = QDir::currentPath().append(path);
                if(getenv("windir") != NULL)
                        pathTestObj.replace("/", "\\");
                //sound->play2D(pathTestObj.toAscii().data(), loop);
                music = Mix_LoadMUS(pathTestObj.toAscii().data());
                if (loop)
                {
                    Mix_PlayMusic(music, -1);
                }
                else
                {
                    Mix_PlayMusic(music, 0);
                }
        }
}

