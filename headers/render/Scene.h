#ifndef _SCENE_H_
#define _SCENE_H_

#include "headers/render/ShaderManager.h"
#include "headers/render/Geometry.h"
#include "headers/render/SceneGUI.h"
#include "headers/kinect/Kinect.h"

#include "headers/logic/Player.h"
#include "headers/logic/NPC.h"
#include "headers/logic/Reward.h"
#include "headers/logic/Obstacle.h"
#include "headers/ui/PopUpMenu.h"
#include "headers/ui/PopUpEndGame.h"
#include "headers/ui/PopUpInit.h"

#include <iostream>
#include <QGLWidget>
#include <QKeyEvent>
#include <QTimer>
#include <vector>
#include <list>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>


using namespace std;

enum elems {PLAYER, PLAYER_SHADOW, PLAYER_HIT, PLAYER_IMMUNE, TREE, TREE_SHADOW, POT_LIFE, POT_LIFE_SHADOW, POT_MANA, POT_MANA_SHADOW,
	POT_IMMUNITY, POT_IMMUNITY_SHADOW, OBS_WALL, OBS_WALL_SHADOW, OBS_SPIKES, OBS_SPIKES_SHADOW, RNG_BULLET, RNG_AOE1, RNG_AOE2,
	ENE_GHOST, ENE_GHOST_SHADOW, ENE_TREE, ENE_TREE_SHADOW, ENE_TREE_LA, ENE_TREE_LA_SHADOW, ENE_TREE_RA, ENE_TREE_RA_SHADOW,
	ENE_MOLE, ENE_MOLE_SHADOW, ENE_MOLE_BASE, ENE_MOLE_BASE_SHADOW, ENE_SPIDER, ENE_SPIDER_SHADOW, ENE_MUSHROOM, ENE_MUSHROOM_SHADOW};


struct Message
{
	Message(QString t, QColor c)
	{
		text = t;
		font = QFont("RussianQuality", 18, QFont::DemiBold);
		color = c;
		count = 0;
	}

	QString text;
	QColor color;
	QFont font;
	int count;
};

class Scene : public QGLWidget
{
	Q_OBJECT

public:

        Scene(QWidget* parent = 0);

	~Scene()
	{
                //sound->drop();

	}

	// needed in kinect.cpp
	static void drawModel(Object *o, bool isShadow = false, bool normalFace = false);
	static void drawRectangle(float Xsize = 1.0f, float Ysize = 1.0f, float Zsize = 1.0f);

protected:

	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();

	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);

private:

	// Kinect instance
	Kinect kinect;
	bool isKinectConnected;
	bool end;
	// Timings
	// For waiting to calibrate
	QTimer *mKinect;
	// For UpdateGL()
	QTimer *mRefresh; 
	// Time left to finish the game
	QTimer *mTime;
	int gameTime;
	int currentTime;
	// Time without being harmed after an enemy hit or reward purchase
	QTimer *mImmunity;
	// To show final results at the end of the game
	QTimer *mEnd;

	// Subwindows
	// - Showing stats (GUI)
	SceneGUI *viewGUI;
	void updateGUI();
	// - PopupMenu
	PopUpMenu *pop;
	// - PopUpEndGame
	PopUpEndGame *popEnd;
	// - PopUpInit
	PopUpInit *popInit;

	// Camera spec
	Point camPos, vrp, vuv;

	// Scene spec
	int mWidth, mHeight; // of viewport
	float Znear;
	float Zfar;
	float fov;
	void calcFov(int width, int height);
	void projectionPerspective(int width, int height);
	void projectionOrthogonal(int width, int height);

	// Lighting
	void setLight();
	bool isLight;
	GLfloat lightAmbient[4]; 
	GLfloat lightDiffuse[4];
	GLfloat lightSpecular[4];
	GLfloat lightPos[4];

	// Materials
	GLfloat matSpec[4];
	GLfloat matRed[4];
	GLfloat matGreen[4];
	GLfloat matBlue[4];
	GLfloat matBlack[4];
	GLfloat matWhite[4];
	void initMaterials();

	// Vetex and Fragment Shader Manager
	ShaderManager *celTex, *celMtl, *hit, *immune;
	int immuneCounter, immuneIncr;

	// Display lists
	int numElems;
	GLuint *dList;
	Object **objList;
	void setDisplayLists();

	// Drawn objects
	//void drawModel(Object *o, ShaderManager *sm, bool isShadow = false);
	void drawPlayer(bool isShadow = false);
	void drawEnemies(bool isShadow = false);
	void drawRewards(bool isShadow = false);
	void drawObstacles(bool isShadow = false);
	void drawRangedShoots();
	void drawGround(Point p0, Point p1);
	void drawWall(Point p0, Point p1, GLuint *tex = NULL, bool rear = false);
	void drawTrees(bool isShadow = false);
	//void drawRectangle(float Xsize = 1.0f, float Ysize = 1.0f, float Zsize = 1.0f);
	void drawFireball();
	void drawShootBullet(XnPoint3D pos, int type = RNG_BULLET, Point scale = Point());
	float rotation;

	// Textures
	void createTextures(const char* filename, GLuint* textureId);
	GLuint texGround;
	GLuint texBackground, texBackCenter, texBackLeft, texBackRight;
	GLuint texWallBrick, texWallBush;

	// Shadows
	bool isShadow;
	void createShadows();
	GLfloat shadowMatrix[4][4];
	void shadowProjectionMatrix(Point normal);

	// Input movements
	bool isFront, isBack, isLeft, isRight;
	void movementUpdate();
	bool outOfRange(Element* element);

	// Physics
	void initPhysics();
        void createRigidBody(Element* elem, btVector3 bb, btVector3 initPos, float mass, bool inWorld = true);
	void deleteElement(Element* elem);
	btAlignedObjectArray<btConvexHullShape*> mCollisionShapes;
	btBroadphaseInterface *mBroadphase;
	btCollisionDispatcher *mDispatcher;
	btConstraintSolver *mSolver;
	btDefaultCollisionConfiguration *mCollisionConfiguration;
	btDynamicsWorld *mDynamicsWorld;
	btRigidBody *groundRigidBody;

	// In-game elements
	Player *player;
	vector<Element *> legoParts;
	list<NPC*> *enemies;
	list<Reward*> *items;
	list<Obstacle*> *obstacles;
	list<Message> *messages;
	void messagesUpdate();
	void addMessage(QString text, QColor c);

	// In-game manipulation
	float sceneMove;	
	void sceneUpdate();
	void updateLevel();
	float GROUND_SPEED;
	float diffAttack;
	float diffSpeed;
	float diffProb;
	float diffNum;

	// In-game sound
       // irrklang::ISoundEngine *sound;
        Mix_Chunk *sound;

	// Simple (Random candidates search -> look for a proper random value to be a future candidate)
	static double Simple(uint& seed)
	{
		static uint M = 1664525, C = 1013904223;
		seed = seed * M + seed + C;

		// tempering (from Matsumoto)
		seed ^= (seed >> 11);
		seed ^= (seed << 7) & 0x9d2c5680UL;
		seed ^= (seed << 15) & 0xefc60000UL;
		seed ^= (seed >> 18);

		static unsigned char p[256] = { 148, 201, 203, 34, 85, 225, 163,
										200, 174, 137, 51, 24, 19, 252, 107,
										173, 110, 251, 149, 69, 180, 152, 141, 132, 22, 20, 147,
										219, 37, 46, 154, 114, 59, 49, 155,
										161, 239, 77, 47, 10, 70, 227, 53, 235, 30, 188, 143,
										73, 88, 193, 214, 194, 18, 120, 176,
										36, 212, 84, 211, 142, 167, 57, 153, 71, 159, 151, 126,
										115, 229, 124, 172, 101, 79, 183, 32,
										38, 68, 11, 67, 109, 221, 3, 4, 61, 122, 94, 72, 117,
										12, 240, 199, 76, 118, 5, 48, 197, 128,
										62, 119, 89, 14, 45, 226, 195, 80, 50, 40, 192, 60, 65,
										166, 106, 90, 215, 213, 232, 250,
								
										207, 104, 52, 182, 29, 157, 103, 242, 97, 111, 17, 8,
										175, 254, 108, 208, 224, 191, 112, 105,
										187, 43, 56, 185, 243, 196, 156, 246, 249, 184, 7, 135,
										6, 158, 82, 130, 234, 206, 255, 160,
										236, 171, 230, 42, 98, 54, 74, 209, 205, 33, 177, 15,
										138, 178, 44, 116, 96, 140, 253, 233,
										125, 21, 133, 136, 86, 245, 58, 23, 1, 75, 165, 92, 217,
										39, 0, 218, 91, 179, 55, 238, 170,
										134, 83, 25, 189, 216, 100, 129, 150, 241, 210, 123, 99,
										2, 164, 16, 220, 121, 139, 168, 64,
										190, 9, 31, 228, 95, 247, 244, 81, 102, 145, 204, 146,
										26, 87, 113, 198, 181, 127, 237, 169,
										28, 93, 27, 41, 231, 248, 78, 162, 13, 186, 63, 66, 131,
										202, 35, 144, 222, 223 };

		union
		{
			uint i;
			unsigned char c[4];
		} u1, u2;
		
		u1.i = seed;
		u2.c[3] = p[u1.c[0]];
		u2.c[2] = p[(u1.c[1] + u2.c[3]) & 0xff];
		u2.c[1] = p[(u1.c[2] + u2.c[2]) & 0xff];
		u2.c[0] = p[(u1.c[3] + u2.c[1]) & 0xff];

		// scale to [0.0 .. 1.0]
		double res = u2.i * (1.0 / 4294967295U); //UINT32_MAX);
		return res;
	}

signals:

	// Finishes the game and comes back to main menu
	void exitGame(int, bool);
	// Update scores
	void updateScores();

public slots:

	// Slots for different timings
	void updateTime();
	void endGame();
	void noImmunity();

	// With Qt GUI
	void backMainMenu();
	void backGame();
	void pauseGame();
	void showResults();

	// Wait till kinect calibrated and proceed
	void kinectTracking();

protected slots:

private slots:

};

void loadSound(Mix_Chunk *sound, char *path, bool isSound = true, bool loop = false);
void initSound();

#endif
