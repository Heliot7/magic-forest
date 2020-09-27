#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <ctime>

#include <QVBoxLayout>
#include <QSpacerItem>
#include <QString>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QWidget>

#include "headers/render/Scene.h"
#include "headers/logic/Physics.h"
#include "headers/logic/GameInfo.h"
#include "headers/data/ScoreManager.h"

#include "LinearMath/btConvexHull.h"

using namespace std;

#define FONT_FAMILY "Vivala"

#define MAX_SCENE_WIDTH 25.0f
#define MAX_SCENE_GAME_DEPTH 20.0f
#define MAX_SCENE_DEPTH 2500.0f
#define GRAVITY 30.0f
#define IMMUNITY_PER_HIT 2000
#define IMMUNITY 7000


Scene::Scene(QWidget* parent) : QGLWidget(parent)
{
	end = false;
        gameTime = 100000; // gameTime = 60000; // 60 seconds
	currentTime = 0;
	rotation = 0.0f; // TESTER

	// Make it QMainWindow size
	//setFixedSize(800, 600);

	// Set default text
	QFontDatabase::addApplicationFont(":/fonts/russianquality.ttf");
	QFontDatabase::addApplicationFont(":/fonts/vivala.ttf");
	QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));

	// Give keyboard input focus
	setFocusPolicy(Qt::StrongFocus);

	// Waits for kinect to be tracking OK!
	mKinect = new QTimer(this);
	connect( mKinect, SIGNAL( timeout() ), SLOT( kinectTracking() ) );
	// Rendering Refresh (FPS = 30)
	mRefresh = new QTimer(this);
	connect( mRefresh, SIGNAL( timeout() ), SLOT( updateGL() ) );
	// Timer of the game
	mTime = new QTimer(this);
	connect( mTime, SIGNAL( timeout() ), SLOT( updateTime() ) );
	// mTime->start(1000);
	// Timer when player is immune
	mImmunity = new QTimer(this);
	connect( mImmunity, SIGNAL( timeout() ), SLOT( noImmunity() ) );

	// Shader manager initialization
	makeCurrent();
	glewInit();
	celTex = new ShaderManager();
	celMtl = new ShaderManager();
	hit = new ShaderManager("effect");
	immune = new ShaderManager("effect");

	// Insert subwindow (SceneGUI)
	viewGUI = new SceneGUI(this, width(), height()/8);
	QVBoxLayout *gLayout = new QVBoxLayout();
	gLayout->addWidget(viewGUI);
	gLayout->setMargin(0);
	gLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
	setLayout(gLayout);
	// PopUpMenu
	pop = new PopUpMenu();
	connect( pop, SIGNAL( goBackGame() ), this, SLOT( backGame() ) );
	connect( pop, SIGNAL( exitGame() ), this, SLOT( backMainMenu() ) );
	popEnd = new PopUpEndGame();
	connect( popEnd, SIGNAL( exitGame() ), this, SLOT( backMainMenu() ) );
	popInit = new PopUpInit();

	/* In-game stuff */
	// Avatar
	player = new Player();
	for(int i = 0; i < 8; i++)
	{
		Element *p = new Element();
		legoParts.push_back(p);
	}
	isFront = isBack = isLeft = isRight = false;
	// Scene elements
	// - ENEMIES
	enemies = new list<NPC*>();
	// - REWARDS
	items = new list<Reward*>();
	// - OBSTACLES
	obstacles = new list<Obstacle*>();

	// Difficulty
	Diff difficulty = GameInfo::Instance()->getDifficulty();
	switch(difficulty)
	{
		case EASY:
		{
			diffAttack = 1.0;
			diffSpeed = 1.0;
			diffProb = 0.990;
			diffNum = 1.0;
			break;
		}
		case MEDIUM:
		{
			diffAttack = 2.0;
			diffSpeed = 2.0;
			diffProb = 0.97;
			diffNum = 1.5;
			break;
		}
		case HARD:
		{
			diffAttack = 3.0;
			diffSpeed = 4.0;
			diffProb = 0.90;
			diffNum = 4.0;
			break;
		}
	}

	// Scene
	sceneMove = 0.0f;
	GROUND_SPEED = 5.0f * diffSpeed;
	messages = new list<Message>();

	// Sound
        this -> sound = NULL;
        //sound = irrklang::createIrrKlangDevice();
}

/* OpenGL methods */
void Scene::initializeGL()
{	
	// Initialize Kinect
	if (kinect.init() != XN_STATUS_OK) 
	{
		isKinectConnected = false;
		mTime->start(1000);
		mRefresh->start(1000/30);
		cout << "Kinect: Error" << endl;
	}
	else 
	{
		isKinectConnected = true;
		kinect.draw_avatar(legoParts, false);
		mKinect->start(1000/30);
		popInit->setVisible(true);
		cout << "Kinect: Success" << endl;
	}

	// Basic scene charactistics
	glClearColor(0.7f, 0.7f, 1.0f, 1.0f);
	glShadeModel(GL_FLAT); // GL_SMOOTH
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// Normalized values in the pipeline
	glEnable(GL_NORMALIZE);
	// Hidden parts are deleted
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	// Thick lines for cel-shading
	glLineWidth(2.5f);
	// Activate textures
	glEnable(GL_TEXTURE_2D);
	// Blending
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup camera and scene
	cout << "Scene dimension: " << MAX_SCENE_DEPTH << " (" << MAX_SCENE_GAME_DEPTH << ") X " << MAX_SCENE_WIDTH << endl;
	vrp = Point(player->getPos().x, player->getPos().y + 5.0f, player->getPos().z);
	cout << "VRP: " << vrp;
	camPos = Point(vrp.x, 1.5*vrp.y, -2.0*vrp.z);
	cout << "CAMERA POS: " << camPos;
	vuv = Point(0.0f, 1.0f, 0.0f);
	cout << "VUV: " << vuv;
	Znear = 0.1f;
	Zfar = 50.0f; // MAX_SCENE_GAME_DEPTH - player->getPos().z;
	cout << "Znear: " << Znear << " Zfar: " << Zfar << endl;

	// Setup lighting
	isLight = true;
	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	setLight();

	// Setup shadows
	isShadow = GameInfo::Instance()->isShadows();
	
	// Setup materials
	initMaterials();
	// We use glColor3f as materials, but only diffuse
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matWhite);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matBlack);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matBlack);

	// Loading object models in display lists
	numElems = 35;
	dList = new GLuint[numElems];
	objList = new Object*[numElems];
	dList[PLAYER] = glGenLists(numElems);
	setDisplayLists();
	
	// Physics
	initPhysics();

	// Model of avatar with no kinect
	if (!isKinectConnected)
	{
		player->setModel(objList[PLAYER]);
		player->setType(PLAYER);
		createRigidBody(player, btVector3(player->getBB().distX()/2, player->getBB().distY()/2, player->getBB().distZ()/2), 
			btVector3(player->getPos().x, player->getPos().y, player->getPos().z), 99.0f);
	}
	else // Model of avatar from kinect
	{
		legoParts[HEAD]->setModel("/resources/models/kinect/Head.obj");
		legoParts[BODY]->setModel("/resources/models/kinect/Body.obj");
		legoParts[LEFT_ARM]->setModel("/resources/models/kinect/LowerArm.obj");
		//legoParts[LEFT_UPPER_ARM]->setModel("/resources/models/kinect/UpperArm.obj");
		legoParts[LEFT_LEG]->setModel("/resources/models/kinect/LowerLeg.obj");
		legoParts[LEFT_UPPER_LEG]->setModel("/resources/models/kinect/UpperLeg.obj");
		legoParts[RIGHT_ARM]->setModel("/resources/models/kinect/LowerArm.obj");
		//legoParts[RIGHT_UPPER_ARM]->setModel("/resources/models/kinect/UpperArm.obj");
		legoParts[RIGHT_LEG]->setModel("/resources/models/kinect/LowerLeg.obj");
		legoParts[RIGHT_UPPER_LEG]->setModel("/resources/models/kinect/UpperLeg.obj");
	
		// We create the rigid bodies per avatar limb
		Box bb;
		Point p;
		for(int i = 0; i < 8; ++i)
		{
			bb = legoParts[i]->getBB();
			p = legoParts[i]->getPos();
			createRigidBody(legoParts[i], btVector3(bb.distX()/2, bb.distY()/2, bb.distZ()/2), 
			btVector3(p.x, p.y + bb.distY()/2, p.z), 100.0f, true);
		}
	}

	// Embedding shaders
	celTex->loadShader(":/shaders/celTex.vert");
	celTex->loadShader(":/shaders/celTex.frag");
	celMtl->loadShader(":/shaders/celMtl.vert");
	celMtl->loadShader(":/shaders/celMtl.frag");
	hit->loadShader(":/shaders/hit.vert");
	hit->loadShader(":/shaders/hit.frag");
	immune->loadShader(":/shaders/immune.vert");
	immune->loadShader(":/shaders/immune.frag");
	immuneCounter = 0;
	immuneIncr = 1;

	// Loading textures
	if(!isKinectConnected)
		createTextures(":/models/Texture.png", player->getModel()->getTextureId());
	createTextures(":/textures/path.jpg", &texGround);
	createTextures(":/textures/forestSubCenterLow.jpg", &texBackCenter);
	createTextures(":/textures/forestSubLeftLow.jpg", &texBackLeft);
	createTextures(":/textures/forestSubRightLow.jpg", &texBackRight);
	createTextures(":/models/walls/Wall.jpg", &texWallBrick);
	createTextures(":/models/walls/Wall3.jpg", &texWallBush);
}

void Scene::resizeGL(int w, int h)
{
	mWidth = w;
	mHeight = h;
	glViewport( 0, 0, w, h );
	projectionPerspective(w, h);
}


// Draw the scene
void Scene::paintGL()
{
	// All is reset
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	// Paint mode (Lines - GL_LINE or Polygons - GL_POLYGON)
    glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);
	// Setup camera
	gluLookAt(camPos.x, camPos.y, camPos.z, vrp.x, vrp.y, vrp.z, vuv.x, vuv.y, vuv.z);

	/** BACKGROUND ELEMENTS **/
	// Draw image
	glPushMatrix();
		Point p0 = Point(-MAX_SCENE_WIDTH/2, -3.0f, -MAX_SCENE_GAME_DEPTH);
		Point p1 = Point(MAX_SCENE_WIDTH/2, MAX_SCENE_WIDTH, -MAX_SCENE_GAME_DEPTH);
		drawWall(p0, p1, &texBackCenter);
	glPopMatrix();
	// Left Wall
	p0 = Point(-MAX_SCENE_WIDTH/2, -5.0f, 0.0f);
	p1 = Point(-MAX_SCENE_WIDTH/2, MAX_SCENE_WIDTH, -MAX_SCENE_GAME_DEPTH);
	drawWall(p0, p1, &texBackLeft);
	// Right Wall
	p0.x = MAX_SCENE_WIDTH/2;
	p1.x = MAX_SCENE_WIDTH/2;
	drawWall(p0, p1, &texBackRight, true);

	/** GROUND (Shadows applied here if desired) **/
	// Static (sceneMove == 0) or dynamic (!= 0) game level.
	// Also affects other scene elements.
	if(isShadow)
		createShadows();
	else
	{
		glPushMatrix();
			glTranslatef(0.0f, 0.0f, sceneMove + MAX_SCENE_GAME_DEPTH );
			p0 = Point(-MAX_SCENE_WIDTH/2, 0.0f, 0.0f);
			p1 = Point(MAX_SCENE_WIDTH/2, 0.0f, -MAX_SCENE_DEPTH);
			drawGround(p0, p1);
		glPopMatrix();
	}

	// Activate shader
	glDisable(GL_TEXTURE_2D);
	glUseProgram(celMtl->getProgram());
	/** SCENE ELEMENTS **/
	// - ENEMIES	
	drawEnemies();
	// - REWARDS
	drawRewards();
	// - OBSTACLES
	drawObstacles();
	// - RANGED SHOOTS
	drawRangedShoots();
	/** AVATAR **/
	drawPlayer(false);
	// FIREBALL
	drawFireball();
	glEnable(GL_TEXTURE_2D);
	// Deactivate shader
	glUseProgram(0);

	// Draw countdown message
	/*
	if(showMessage)
	{
		QColor color = QColor(0, 0, 0, 255);
		QFont font = QFont(FONT_FAMILY, mWidth/10, QFont::DemiBold);
		glColor4f(color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f, color.alpha()/255.0f);
		QString message = QString("GO!");
		if(countdown >= 500)
			message.setNum(countdown/500);
		renderText(mWidth/2, mHeight/2, message, font);
		return;
	}
	
	else
	{
	*/
		// New position of avatar and enemies updated
		movementUpdate();
		// Update status of the game (life, remove elements, add elements,...)
		sceneUpdate();
		// Update and removes messages that show stat changes
		messagesUpdate();
/*
	}

	if(kinect.getTrackingStatus() != TRACKING && isKinectConnected)
	{
		// Draw message
		QColor color = QColor(255, 150, 0, 255);
		QFont font = QFont("Vivala", mWidth/20, QFont::DemiBold);
		glColor4f(color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f, color.alpha()/255.0f);
		QString text = QString("Calibrate with your kinect...");
		renderText(mWidth/2 - mWidth/7.5, mHeight/2 + mWidth/10, text, font);
	}
	*/
}

/* Draw objects */
void Scene::drawModel(Object *o, bool isShadow, bool normalFace)
{
	unsigned int i, j;
	float x, y, z;

	glPushMatrix();
		// Draw the whole object
		for (i = 0; i < o->faces.size(); i++)
		{
			Face f = o->faces[i];
			// Draws thick black border
			if(!isShadow)
			{
				glPolygonMode (GL_BACK, GL_LINE);
				glCullFace(GL_FRONT);
				glDisable(GL_LIGHTING);
				glBegin(GL_TRIANGLES);
					glColor3f(0.0f, 0.0f, 0.0f);
					for (j = 0; j < f.vertices.size(); j++)
					{
						x = o->vertices[f.vertices[j]].coord.x;
						y = o->vertices[f.vertices[j]].coord.y;
						z = o->vertices[f.vertices[j]].coord.z;
						glVertex3f(x, y ,z);
					}
				glEnd();
				glEnable(GL_LIGHTING);
			}
			// Draws character
			Material mat = o->getMatLib()->material(f.material);
			glPolygonMode (GL_FRONT_AND_BACK, GL_TRIANGLES);
			glCullFace(GL_BACK);	
			glBegin(GL_TRIANGLES);
				if(normalFace)
					glNormal3f(f.normal.x, f.normal.y, f.normal.z);
				glColor3f(mat.ka.r, mat.ka.g, mat.ka.b);
				for (j = 0; j < f.vertices.size(); j++)
				{
					float u = o->texCoords[f.texCoords[j]].x;
					float v = o->texCoords[f.texCoords[j]].y;
					glTexCoord2f(u, v);
					if(!isShadow && !normalFace)
					{
						Vector normal = o->vertices[f.vertices[j]].normal;
						glNormal3f(normal.x, normal.y, normal.z);
					}
					x = o->vertices[f.vertices[j]].coord.x;
					y = o->vertices[f.vertices[j]].coord.y;
					z = o->vertices[f.vertices[j]].coord.z;
					glVertex3f(x, y ,z);
				}
			glEnd();
		}
	glPopMatrix();
}

void Scene::drawPlayer(bool isShadow)
{
	glPushMatrix();
		glTranslatef(player->getPos().x, player->getPos().y, player->getPos().z);
		glRotatef(rotation, 0.0f, 1.0f, 0.0f);
		if(player->isImmuneHit())
		{
			glUseProgram( hit->getProgram() );
			uint seed = rand();
			float time = Simple(seed)/2;
			GLint uniform = glGetUniformLocation( hit->getProgram(), "time");
			glUniform1f(uniform, time);
			if(!isKinectConnected)
				glCallList(dList[PLAYER_HIT]);
			else
			{
				glPushMatrix();
					glLoadIdentity();
					gluLookAt(camPos.x, camPos.y, camPos.z, vrp.x, vrp.y, vrp.z, vuv.x, vuv.y, vuv.z);
					kinect.draw_avatar(legoParts, true);
				glPopMatrix();
			}
			glUseProgram( 0 );
		}
		else if(player->isImmune())
		{
			immuneCounter += immuneIncr;
			if(immuneCounter == 20)
				immuneIncr = -1;
			else if(immuneCounter == 0)
				immuneIncr = 1;
			glUseProgram( immune->getProgram() );
			GLint uniform = glGetUniformLocation(immune->getProgram(), "size");
			glUniform1f(uniform, (float)immuneCounter/100);
			if(!isKinectConnected)
				glCallList(dList[PLAYER_IMMUNE]);
			else
			{
				glPushMatrix();
					glLoadIdentity();
					gluLookAt(camPos.x, camPos.y, camPos.z, vrp.x, vrp.y, vrp.z, vuv.x, vuv.y, vuv.z);
					kinect.draw_avatar(legoParts, true);
				glPopMatrix();
			}
			glUseProgram( 0 );
		}
		else if(!isShadow)
		{
			if(!isKinectConnected)
			{
				glCallList(dList[PLAYER]);
			}
			else
			{
				glPushMatrix();
					glLoadIdentity();
					gluLookAt(camPos.x, camPos.y, camPos.z, vrp.x, vrp.y, vrp.z, vuv.x, vuv.y, vuv.z);
					kinect.draw_avatar(legoParts, true);
				glPopMatrix();
			}
		}
		else
		{
			if(!isKinectConnected)
				glCallList(dList[PLAYER_SHADOW]);
			else
			{
				glPushMatrix();
					glLoadIdentity();
					gluLookAt(camPos.x, camPos.y, camPos.z, vrp.x, vrp.y, vrp.z, vuv.x, vuv.y, vuv.z);
					kinect.draw_avatar(legoParts, true);
				glPopMatrix();
			}
		}
	glPopMatrix();
}

void Scene::drawEnemies(bool isShadow)
{
	for(list<NPC*>::iterator it = enemies->begin(); it != enemies->end(); ++it)
	{
		NPC* npc = *it;
		uint seed = clock();
		if(npc->getHit() >= 2 && Simple(seed) > 0.5)
			continue;

		if(*npc->getModel()->getTextureId() != NULL)
			glBindTexture(GL_TEXTURE_2D, *npc->getModel()->getTextureId());
		glPushMatrix();
			
			glTranslatef(npc->getPos().x, npc->getPos().y, npc->getPos().z);

			if(npc->getType() == ENE_GHOST)
			{
			}
			else if(npc->getType() == ENE_TREE)
			{
				glPushMatrix();
					glTranslatef(0.0f, npc->getBB().distY()/2, 0.0f);
					glRotatef(npc->getRotLeft(), 1.0f, 0.0f, 0.0f);
					glTranslatef(0.0f, -npc->getBB().distY()/2, 0.0f);
					glCallList(dList[ENE_TREE_LA]);
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0.0f, npc->getBB().distY()/2, 0.0f);
					glRotatef(npc->getRotRight(), 1.0f, 0.0f, 0.0f);
					glTranslatef(0.0f, -npc->getBB().distY()/2, 0.0f);
					glCallList(dList[ENE_TREE_RA]);
				glPopMatrix();
			}
			else if(npc->getType() == ENE_MOLE)
			{
				glPushMatrix();
					glTranslatef(0.0f, -npc->getPos().y, 0.0f);
					glCallList(dList[ENE_MOLE_BASE]);
				glPopMatrix();
			}
			else if(npc->getType() == ENE_SPIDER)
			{
			}
			else if(npc->getType() == ENE_MUSHROOM)
			{
				float scaleM = npc->getModel()->getScale();
				glScalef(scaleM, scaleM, scaleM);
			}
			if(!isShadow)
				glCallList(dList[npc->getType()]);
			else
				glCallList(dList[npc->getType()+1]);
		glPopMatrix();
	}
}

void Scene::drawRewards(bool isShadow)
{
	for(list<Reward*>::iterator it = items->begin(); it != items->end(); ++it)
	{
		Reward* item = *it;
		glPushMatrix();
			glTranslatef(item->getPos().x, item->getPos().y, item->getPos().z);
			if(!isShadow)
				glCallList(dList[item->getType()]);
			else
				glCallList(dList[item->getType()+1]);			
		glPopMatrix();
	}
}
void Scene::drawObstacles(bool isShadow)
{
	for(list<Obstacle*>::iterator it = obstacles->begin(); it != obstacles->end(); ++it)
	{
		Obstacle* obs = *it;
		glPushMatrix();
			glTranslatef(obs->getPos().x, obs->getPos().y, obs->getPos().z);
			if(!isShadow)
			{
				if(obs->isWall())
				{
					glBindTexture(GL_TEXTURE_2D, obs->isWall() == 1 ? texWallBrick : texWallBush );
					glCallList(dList[OBS_WALL]);
				}
				else
					glCallList(dList[OBS_SPIKES]);
			}
			else
			{
				if(obs->isWall())
					glCallList(dList[OBS_WALL_SHADOW]);
				else
					glCallList(dList[OBS_SPIKES_SHADOW]);
			}
		glPopMatrix();
	}
}

void Scene::drawRangedShoots()
{
	list<Attack*>::iterator it;
	it = player->getShoots()->begin();
	while(it != player->getShoots()->end())
	{
		Attack *bullet = *it;
		glPushMatrix();
			glTranslatef(bullet->getPos().x, bullet->getPos().y, bullet->getPos().z);
			float scale = bullet->getScale().x;
			glScalef(scale, scale, scale);
			glCallList(dList[bullet->getType()]);
		glPopMatrix();	
		++it;
	}
}

void Scene::drawGround(Point p0, Point p1)
{
	glBindTexture(GL_TEXTURE_2D, texGround);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( p1.x, p1.y,  p0.z);
		glTexCoord2f(1.0f, MAX_SCENE_DEPTH/8);
		glVertex3f( p1.x, p1.y, p1.z);
		glTexCoord2f(0.0f, MAX_SCENE_DEPTH/8);
		glVertex3f(p0.x, p0.y, p1.z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(p0.x, p1.y, p0.z);
	glEnd();
}

void Scene::drawWall(Point p0, Point p1, GLuint *tex, bool rear)
{
	if(tex != NULL)
		glBindTexture(GL_TEXTURE_2D, *tex);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		if(tex != NULL)
			glTexCoord2f(0.0f, 0.0f); 
		glVertex3f(p0.x, p0.y, p0.z);
		if(tex != NULL)
			glTexCoord2f(1.0f, 0.0f);
		if(!rear)
			glVertex3f(p1.x, p0.y, p1.z);
		else
			glVertex3f(p0.x, p1.y, p0.z);
		if(tex != NULL)
			glTexCoord2f(1.0f, 1.0f);
		glVertex3f(p1.x, p1.y, p1.z);
		if(tex != NULL)
			glTexCoord2f(0.0f, 1.0f);
		if(!rear)
			glVertex3f(p0.x, p1.y, p0.z);
		else
			glVertex3f(p1.x, p0.y, p1.z);
	glEnd();
}

void Scene::drawTrees(bool isShadow)
{
	// Draw Environment trees
	glPushMatrix();
		glTranslatef(-MAX_SCENE_WIDTH/2.5, 0.0f, -15.0f);
		glScalef(10.0f, 10.f, 10.0f);
		if(!isShadow)
			glCallList(dList[TREE]);
		else
			glCallList(dList[TREE_SHADOW]);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-MAX_SCENE_WIDTH/2.5, 0.0f, -7.5f);
		glScalef(7.5f, 7.5f, 7.5f);
		if(!isShadow)
			glCallList(dList[TREE]);
		else
			glCallList(dList[TREE_SHADOW]);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(MAX_SCENE_WIDTH/2.5, 0.0f, -15.0f);
		glScalef(10.0f, 10.f, 10.0f);
		if(!isShadow)
			glCallList(dList[TREE]);
		else
			glCallList(dList[TREE_SHADOW]);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(MAX_SCENE_WIDTH/2.5, 0.0f, -7.5f);
		glScalef(7.5f, 7.5f, 7.5f);
		if(!isShadow)
			glCallList(dList[TREE]);
		else
			glCallList(dList[TREE_SHADOW]);
	glPopMatrix();
}

void Scene::drawRectangle(float Xsize, float Ysize, float Zsize)
{
	glBegin(GL_QUADS);
		Point p1 = Point( Xsize, Ysize,-Zsize);
		Point p2 = Point(-Xsize, Ysize,-Zsize);
		Point p3 = Point(-Xsize, Ysize, Zsize);
		Vector normal = Vector::normal(&p1, &p2, &p3);
		glNormal3f(normal.x, normal.y, normal.z);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f( Xsize, Ysize,-Zsize);
		glVertex3f(-Xsize, Ysize,-Zsize);
		glVertex3f(-Xsize, Ysize, Zsize);
		glVertex3f( Xsize, Ysize, Zsize);

		p1 = Point( Xsize, -Ysize, Zsize);
		p2 = Point(-Xsize, -Ysize, Zsize);
		p3 = Point(-Xsize, -Ysize, -Zsize);
		normal = Vector::normal(&p1, &p2, &p3);
		glNormal3f(normal.x, normal.y, normal.z);		
		glColor3f(1.0f,1.0f,0.0f);
		glVertex3f( Xsize,-Ysize, Zsize);
		glVertex3f(-Xsize,-Ysize, Zsize);
		glVertex3f(-Xsize,-Ysize,-Zsize);
		glVertex3f( Xsize,-Ysize,-Zsize);

		p1 = Point( Xsize, Ysize, Zsize);
		p2 = Point(-Xsize, Ysize, Zsize);
		p3 = Point(-Xsize, -Ysize, Zsize);
		normal = Vector::normal(&p1, &p2, &p3);
		glNormal3f(normal.x, normal.y, normal.z);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( Xsize, Ysize, Zsize);
		glVertex3f(-Xsize, Ysize, Zsize);
		glVertex3f(-Xsize,-Ysize, Zsize);
		glVertex3f( Xsize,-Ysize, Zsize);

		p1 = Point( Xsize, -Ysize, -Zsize);
		p2 = Point(-Xsize, -Ysize, -Zsize);
		p3 = Point(-Xsize, Ysize, -Zsize);
		normal = Vector::normal(&p1, &p2, &p3);
		glNormal3f(normal.x, normal.y, normal.z);
		glColor3f(1.0f,1.0f,0.0f);
		glVertex3f( Xsize,-Ysize,-Zsize);
		glVertex3f(-Xsize,-Ysize,-Zsize);
		glVertex3f(-Xsize, Ysize,-Zsize);
		glVertex3f( Xsize, Ysize,-Zsize);

		p1 = Point(-Xsize, Ysize, Zsize);
		p2 = Point(-Xsize, Ysize, -Zsize);
		p3 = Point(-Xsize, -Ysize, -Zsize);
		normal = Vector::normal(&p1, &p2, &p3);
		glNormal3f(normal.x, normal.y, normal.z);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(-Xsize, Ysize, Zsize);
		glVertex3f(-Xsize, Ysize,-Zsize);
		glVertex3f(-Xsize,-Ysize,-Zsize);
		glVertex3f(-Xsize,-Ysize, Zsize);

		p1 = Point( Xsize, Ysize, -Zsize);
		p2 = Point( Xsize, Ysize, Zsize);
		p3 = Point( Xsize, -Ysize, Zsize);
		normal = Vector::normal(&p1, &p2, &p3);
		glNormal3f(normal.x, normal.y, normal.z);
		glColor3f(1.0f,0.0f,1.0f);
		glVertex3f( Xsize, Ysize,-Zsize);
		glVertex3f( Xsize, Ysize, Zsize);
		glVertex3f( Xsize,-Ysize, Zsize);
		glVertex3f( Xsize,-Ysize,-Zsize);
	glEnd();
}

void Scene::drawFireball()
{
	static float scale = 0.0;
	static int aoeTime = 0;
	const float maxScale = 1.5;
	static bool canSound = true;
	
	// if there is mana
	if (player->getMana() >= 1.0f)
	{
		// shoot bullet
		int shoot = kinect.isPushDetected();
		if (shoot)
		{
			cout << ":fire!" << endl;
			if (shoot == LEFT)
			{
				XnPoint3D lhand = kinect.getJoint(XN_SKEL_LEFT_HAND);
				this->drawShootBullet(lhand);
			}
			if (shoot == RIGHT)
			{
				XnPoint3D rhand = kinect.getJoint(XN_SKEL_RIGHT_HAND);
				this->drawShootBullet(rhand);
			}
		}

		// special punch with hands in the air first and fire
		else if (!kinect.isDrawingFireball && kinect.isBothHandInAir() && player->getMana() >= 3.0f)
		{
			kinect.isDrawingFireball = true;
		}
		
		// draw fireball
		if (kinect.isDrawingFireball)
		{
			// grow fireball if hands are still in the air
			if (kinect.isBothHandInAir())
			{
				scale = scale + 0.025;
				if (scale > maxScale)
				{
					scale = maxScale;
				}
			}
			XnPoint3D rightHandPos = kinect.getJoint(XN_SKEL_RIGHT_HAND);
			if(!kinect.isDrawingAOE)
			{
				canSound = true;
				glPushMatrix();
					glTranslatef(rightHandPos.X, rightHandPos.Y, rightHandPos.Z);
					Box bb = objList[RNG_AOE1]->boundingBox();
					glTranslatef(0.0f, scale*bb.distY()/2, 0.0f);
					glScalef(scale, scale, scale);
					glCallList(dList[RNG_AOE1]);
				glPopMatrix();
			}

			// shoot fireball
			if (kinect.isShootDetected())
			{
				kinect.isDrawingAOE = true;
				if(canSound)
				{
					loadSound(sound,"/resources/sounds/aoe.ogg", GameInfo::Instance()->isSound());
					canSound = false;
					XnPoint3D rhand = kinect.getJoint(XN_SKEL_RIGHT_HAND);
					drawShootBullet(rhand, RNG_AOE1, Point(scale,scale,scale));
				}
			}
			
			if(kinect.isDrawingAOE)
			{
				aoeTime++;
				if(aoeTime > 60)
				{
					aoeTime = 0;
					scale = 0;
					kinect.isDrawingAOE = false;
					kinect.isDrawingFireball = false;
				}
			}
		}
	}

}

void Scene::drawShootBullet(XnPoint3D pos, int type, Point scale)
{
	Attack *bullet = new Attack();
	if(type == RNG_BULLET)
		loadSound(sound,"/resources/sounds/bullet.ogg", GameInfo::Instance()->isSound());
	else
		bullet->setScale(scale);
	bullet->setType(type);
	bullet->setModel(objList[type]);
	createRigidBody(bullet, btVector3(bullet->getBB().distX()/2, bullet->getBB().distY()/2, bullet->getBB().distZ()/2),
		btVector3(pos.X, pos.Y, pos.Z - scale.z*bullet->getBB().distZ()/2), 1000000000.0f);
	bullet->setPos(Point(pos.X, pos.Y, pos.Z - scale.z*bullet->getBB().distZ()/2));
	XnPoint3D p1 = kinect.getJoint(XN_SKEL_RIGHT_HAND);
	XnPoint3D p0 = kinect.getJoint(XN_SKEL_RIGHT_ELBOW);
	bullet->setDir(Vector(50*(p1.X-p0.X), 0.0f, 50*(p1.Z-p0.Z)));
	bullet->getBody()->setLinearVelocity(btVector3(50*(p1.X-p0.X), 0.0f, 50*(p1.Z-p0.Z)));
	player->addShoot(bullet);
	if(type == RNG_BULLET)
		player->updateMana(-1.0f);
	if(type == RNG_AOE1)
		player->updateMana(-3.0f);
}

void Scene::createShadows()
{
	// Stencil buffer activated to store data
	glEnable(GL_STENCIL_TEST);
	// We start drawing in the Stencil Buffer
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// Ground first drawn
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, sceneMove + MAX_SCENE_GAME_DEPTH );
		Point p0 = Point(-MAX_SCENE_WIDTH/2, 0.0f, 0.0f);
		Point p1 = Point(MAX_SCENE_WIDTH/2, 0.0f, -MAX_SCENE_DEPTH);
		drawGround(p0, p1);
	glPopMatrix();
	// Ocluded drawn for cleaning all what will be shadow
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE, GL_FALSE);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
	// We apply the projection matrix for shadows
	glPushMatrix();
		Point normal = Point(0.0f, 1.0f, 0.0f);
		shadowProjectionMatrix(normal);
		glMultMatrixf((const GLfloat*)shadowMatrix);
		// Draw all scene elements with shadows:
		// - AVATAR
		// drawPlayer(true);
		// - RANGED SHOOTS
		drawRangedShoots();
		// - ENEMIES
		drawEnemies(true);
		// - REWARDS
		drawRewards(true);
		// - OBSTACLES
		drawObstacles(true);
		// - TREES
		drawTrees(true);
	glPopMatrix();
	// Draw the dark region of the ground - proper shadow
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glStencilFunc(GL_EQUAL, 0, 1);
	glDisable(GL_LIGHT0);
	// Then we repaint the ground but only in those regions selected by the Stencil Buffer
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, sceneMove + MAX_SCENE_GAME_DEPTH );
		p0 = Point(-MAX_SCENE_WIDTH/2, 0.0f, 0.0f);
		p1 = Point(MAX_SCENE_WIDTH/2, 0.0f, -MAX_SCENE_DEPTH);
		drawGround(p0, p1);
	glPopMatrix();			
	glEnable(GL_LIGHT0);
	// Clear the Stencil Buffer
	glClear(GL_STENCIL_BUFFER_BIT);
	glDepthFunc(GL_LESS);
	// Stencil deactivated
	glDisable(GL_STENCIL_TEST);
}

void Scene::setLight()
{
	lightAmbient[0] = 0.75f; lightAmbient[1] = 0.75f; lightAmbient[2] = 0.75f; lightAmbient[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);	
	lightDiffuse[0] = 0.75f; lightDiffuse[1] = 0.75f; lightDiffuse[2] = 0.5f; lightDiffuse[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	lightSpecular[0] = 0.3f; lightSpecular[1] = 0.3f; lightSpecular[2] = 0.3f; lightSpecular[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	lightPos[0] = 0.0f; lightPos[1] = camPos.y; lightPos[2] = -10.0f; lightPos[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
	// Only calculated in front side
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);	
}

/* Other private functions */
void Scene::movementUpdate()
{	
	uint seed = clock();

	// Update Dynamic movement of the scene
	sceneMove += GROUND_SPEED / 60;

	if (mDynamicsWorld)
		mDynamicsWorld->stepSimulation((float)1/30);
	btTransform trans;

	// Update avatar movement
	if(!isKinectConnected)
	{
		player->getBody()->getMotionState()->getWorldTransform(trans);
		Point pPos;
		pPos.x = trans.getOrigin().getX();
		pPos.y = trans.getOrigin().getY();
		pPos.z = trans.getOrigin().getZ();
		player->setPos(pPos);
	}
	else // update rigid bodies from kinect input
	{
		// Loop of all lego parts to update their respective btRigidBodies
		for(int i = 0; i < 8; ++i)
		{
			// Update position
			Point p = legoParts[i]->getPos();
			trans.setIdentity();
			trans.setOrigin(btVector3(p.x, p.y + legoParts[i]->getBB().distY()/2, p.z));
			// Update Rotation
			//trans.setRotation(angle(btVector3(1.0, 1.0, 0.0), btVector3(0.0, 1.0, 0.0)));
			legoParts[i]->getBody()->setCenterOfMassTransform(trans);
			Point pPos;
			pPos.x = trans.getOrigin().getX();
			pPos.y = trans.getOrigin().getY();
			pPos.z = trans.getOrigin().getZ();
			// Drawing test!
			/*
			glPushMatrix();
				glTranslatef(pPos.x, pPos.y, pPos.z);
				Scene::drawRectangle(0.25,0.25,0.25);
			glPopMatrix();
			*/
		}
	}

	// Avatar ranged shoots movements
	list<Attack*>::iterator it;
	it = player->getShoots()->begin();
	while(it != player->getShoots()->end())
	{
		Attack *bullet = *it;
		bullet->getBody()->getMotionState()->getWorldTransform(trans);
		bullet->getBody()->setLinearVelocity(btVector3(bullet->getDir().x, bullet->getDir().y, bullet->getDir().z));
		Point bPos;
		bPos.x = trans.getOrigin().getX();
		bPos.y = trans.getOrigin().getY();
		bPos.z = trans.getOrigin().getZ();
		bullet->setPos(bPos);
		++it;
/*		
		glPushMatrix();
			glTranslatef(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
			drawRectangle(0.5,0.5,0.5);
		glPopMatrix();
*/
	}

	// Enemies movements 
	list<NPC*>::iterator itNPC = enemies->begin();
	while(itNPC != enemies->end())

	{
		// Update position depending on Bullet
		NPC* npc = *itNPC;
		npc->getBody()->getMotionState()->getWorldTransform(trans);
		Point pos;
		pos.x = trans.getOrigin().getX();
		pos.y = trans.getOrigin().getY();
		pos.z = trans.getOrigin().getZ();
		npc->setPos(pos);

		// Update vector of velocity or shape
		if(!npc->getHit())
		{
			if(npc->getType() == ENE_GHOST)
			{
				if(npc->getBody()->getLinearVelocity().getY() < -4.0)
					npc->setUp(true);
				if(npc->getBody()->getLinearVelocity().getY() > 4.0)
					npc->setUp(false);
				if(npc->getBody()->getLinearVelocity().getY() < 4.0 && npc->isUp())
					npc->getBody()->setLinearVelocity(btVector3(npc->getBody()->getLinearVelocity().getX() + 3.4*Simple(seed), npc->getBody()->getLinearVelocity().getY() + 1.2, GROUND_SPEED*1.5));
				if(npc->getBody()->getLinearVelocity().getY() > -4.0 && !npc->isUp())
					npc->getBody()->setLinearVelocity(btVector3(npc->getBody()->getLinearVelocity().getX() - Simple(seed), npc->getBody()->getLinearVelocity().getY() + 0.3, GROUND_SPEED*1.5));
			}
			else if(npc->getType() == ENE_TREE)
			{
				npc->updateRotLeft(+5.0);
				npc->updateRotRight(-5.0);
				npc->getBody()->setLinearVelocity(btVector3(0.0, 0.0, GROUND_SPEED));
			}
			else if(npc->getType() == ENE_MOLE)
			{
				if(pos.y <= npc->getBB().min.y + 0.2 && Simple(seed) > 0.95)
					npc->getBody()->setLinearVelocity(btVector3(0.0, GRAVITY/1.5, GROUND_SPEED));
				else
					npc->getBody()->setLinearVelocity(btVector3(0.0, npc->getBody()->getLinearVelocity().getY(), GROUND_SPEED));
				pos.y = trans.getOrigin().getY() - npc->getBB().distY()/2;
				npc->setPos(pos);
			}
			else if(npc->getType() == ENE_SPIDER)
			{
				if(pos.y <= npc->getBB().min.y + 0.2)
					npc->getBody()->setLinearVelocity(btVector3((float)(rand()%5)-2.5, GRAVITY/2, GROUND_SPEED*1.5));
			}
			else if(npc->getType() == ENE_MUSHROOM)
			{
				if(npc->getModel()->getScale() <= 0.3)
					npc->setUp(true);
				else if(npc->getModel()->getScale() >= 1.1)
					npc->setUp(false);
				if(npc->isUp())
					npc->getModel()->updateScaleFactor(+0.01);
				else
					npc->getModel()->updateScaleFactor(-0.01);
				npc->getBody()->setLinearVelocity(btVector3(0.0, 0.0, GROUND_SPEED));
			}
		}
		else if(npc->getHit() == 1)
		{
			npc->upgradeHit();
			npc->getBody()->setLinearVelocity(btVector3(0.0, 15.0, -5.0));
		}
		else if(npc->getHit() >= 40)
		{
			deleteElement(npc);
			itNPC = enemies->erase(itNPC);
			continue;
		}
		else if(npc->getHit() >= 2)
		{
			npc->upgradeHit();
		}
		itNPC++;
	}

	// Rewards movements
	for(list<Reward*>::iterator it = items->begin(); it != items->end(); ++it)
	{
		Reward* item = *it;
		item->getBody()->getMotionState()->getWorldTransform(trans);
		Point pos;
		pos.x = trans.getOrigin().getX();
		pos.y = trans.getOrigin().getY();
		pos.z = trans.getOrigin().getZ();
		item->setPos(pos);
		item->getBody()->setLinearVelocity(btVector3(0.0, 0.0, GROUND_SPEED));
	}

	// Obstacles movements
	for(list<Obstacle*>::iterator it = obstacles->begin(); it != obstacles->end(); ++it)
	{
		Obstacle* obs = *it;
		obs->updatePosZ(obs->getPos().z + GROUND_SPEED / 60);
	}
}

void Scene::initPhysics()
{	
	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	mDispatcher = new	btCollisionDispatcher(mCollisionConfiguration);
	mBroadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* mSolver = new btSequentialImpulseConstraintSolver;
	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher,mBroadphase,mSolver,mCollisionConfiguration);	
	mDynamicsWorld->setGravity(btVector3(0,-GRAVITY,0));
	// Ground
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0), 1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,-1,0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0,0,0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	// Add the body to the dynamics world
	mDynamicsWorld->addRigidBody(groundRigidBody);

	// Create all possible collision shapes
	for(unsigned int i = 0; i < numElems; ++i)
	{
		if(objList[i] != NULL)
		{
			Object *o = objList[i];
			btConvexHullShape *hull = new btConvexHullShape();
			for (unsigned int nv = 0; nv < o->vertices.size(); nv++)
			{
				Point v = o->vertices[nv].coord;
				hull->addPoint(btVector3( v.x, v.y, v.z ));
			}
			mCollisionShapes.push_back(hull);
		}
		else
			mCollisionShapes.push_back(NULL);
	}
}

void Scene::createRigidBody(Element* elem, btVector3 bb, btVector3 initPos, float mass, bool inWorld)
{
	// Physics info for movement
	btCollisionShape* shape;
	if(mass == 100.0)
	{
		shape = new btBoxShape(bb);
	}
	else if(mass == 1000000000.0)
	{
		shape = new btSphereShape(bb.x());
	}
	else
	{
		shape = mCollisionShapes.at(elem->getType());
	}

	btVector3 localInertia(0, 0, 0);
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(initPos);
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	elem->setBody(mass, myMotionState, shape, localInertia);
	// Extra arms
	/*
	if(elem->getType() == ENE_TREE)
	{
		shape = mCollisionShapes.at(ENE_TREE_LA);
		((NPC*)elem)->setBodyLeft(mass, myMotionState, shape, localInertia);
		mDynamicsWorld->addRigidBody(((NPC*)elem)->left);
		shape = mCollisionShapes.at(ENE_TREE_RA);
		((NPC*)elem)->setBodyRight(mass, myMotionState, shape, localInertia);
		mDynamicsWorld->addRigidBody(((NPC*)elem)->right);	
	}*/
	if(inWorld)
		mDynamicsWorld->addRigidBody(elem->getBody());	
}

void Scene::sceneUpdate()
{
	// If time is over... finish the game!
	if(gameTime - currentTime < 0)
		endGame();
	// If player has no life, game is also finished!
	else if(player->getLife() <= 0.0f)
		endGame();

	// Update list of enemies (collision || out of range)
	list<NPC*>::iterator itNPC = enemies->begin();
	while(itNPC != enemies->end())
	{
		NPC* npc = *itNPC;
		// Delete out of range enemies
		if(outOfRange(npc))
		{
			deleteElement(npc);
			itNPC = enemies->erase(itNPC);
			continue;
		}

		// Detect Collisions (delete if collides with bullet)
		bool bulletCollide = false;
		bool isAoe = false;
		list<Attack*>::iterator itShoot = player->getShoots()->begin();
		while(itShoot != player->getShoots()->end())
		{
			Attack *bullet = *itShoot;
			CollisionCallback collisionCallBack;
			collisionCallBack.collision = false;
			mDynamicsWorld->contactPairTest(npc->getBody(), bullet->getBody(), collisionCallBack);
			if(collisionCallBack.collision )
			{
				bulletCollide = true;
				loadSound(sound,"/resources/sounds/hit.ogg", GameInfo::Instance()->isSound());
				if(bullet->getType() == RNG_BULLET)
				{
					deleteElement(bullet);
					itShoot = player->getShoots()->erase(itShoot);
					break;
				}
				else
				{
					++itShoot;
					isAoe = true;
				}
			}
			else
				++itShoot;
		}
		if(bulletCollide)
		{
			player->incrEnemiesDown();
			player->addExp(npc->getExpPoints()/2);
			if(!npc->getHit() && !isAoe)
				npc->upgradeHit();
			else if(!npc->getHit() && isAoe)
			{
				deleteElement(npc);
				itNPC = enemies->erase(itNPC);
				continue;
			}
		}

		// Detect Collisions (delete or update player status)		
		bool collision = false;
		bool colParts[8] = {false};
		if (isKinectConnected) {
			//collision = isCollision(player->getBoundingBox(), player->getPos(), npc->getBoundingBox(), npc->getPos());
			//collision = isCollision(legoParts, colParts, npc->getBB(), npc->getPos());
			collision = isCollision(legoParts, colParts, mDynamicsWorld, npc);
		}
		else if(!isKinectConnected)
		{
			collision = isCollision(player->getBB(), player->getPos(), npc->getBB(), npc->getPos());
		}
		if(collision)
		{
			//cout << "COLLISION NPC!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			// Proceed with stats update
			if(!npc->getHit())
			{
				if (kinect.isLeftHandInAir() && colParts[LEFT_ARM] || kinect.isRightHandInAir() && colParts[RIGHT_ARM])
				{
					cout << ":punch" << endl;
					player->addExp(npc->getExpPoints());
					addMessage(QString("EXP +%1").arg(npc->getExpPoints()), QColor(255,255,0));
					player->incrEnemiesDown();
					loadSound(sound,"/resources/sounds/hit.ogg", GameInfo::Instance()->isSound());
					npc->upgradeHit();
				}
				else if(!isKinectConnected)
				{
					player->addExp(npc->getExpPoints());
					addMessage(QString("EXP +%1").arg(npc->getExpPoints()), QColor(255,255,0));
					player->incrEnemiesDown();
					loadSound(sound,"/resources/sounds/hit.ogg", GameInfo::Instance()->isSound());
					npc->upgradeHit();
				}
				else if(!player->isImmune())
				{
					// cout << "Injured by enemy!!" << endl;
					// Player is injured
					loadSound(sound,"/resources/sounds/damage2.ogg", GameInfo::Instance()->isSound());
					player->updateLife(npc->getDamage());
					addMessage(QString("HP %1").arg((int)npc->getDamage()), QColor(255,0,0));
					mImmunity->start(IMMUNITY_PER_HIT);
					player->setImmuneHit(true);
					player->setImmune(true);
				}
			}
		}

		// Update collision boxes of mushrooms and trees
		if(npc->getType() == ENE_MUSHROOM)
		{/*
			float scale = npc->getModel()->getScale();
			btConvexHullShape *hull = new btConvexHullShape();
			Object *o = objList[ENE_MUSHROOM];
			for (unsigned int nv = 0; nv < o->vertices.size(); nv++)
			{
				Point v = o->vertices[nv].coord;
				hull->addPoint(btVector3( scale*v.x, scale*v.y, scale*v.z ));
			}
			npc->getBody()->setCollisionShape(hull);*/
		}
		else if(npc->getType() == ENE_TREE)
		{
			btConvexHullShape *hull = new btConvexHullShape();
			// Main trunk
			Object *o = objList[ENE_TREE];
			for (unsigned int nv = 0; nv < o->vertices.size(); nv++)
			{
				Point v = o->vertices[nv].coord;
				hull->addPoint(btVector3( v.x, v.y, v.z ));
			}
			// Left trunk
			o = objList[ENE_TREE_LA];
			for (unsigned int nv = 0; nv < o->vertices.size(); nv++)
			{
				Point v = o->vertices[nv].coord;
				hull->addPoint(btVector3( v.x, v.y, v.z ));
			}
			o = objList[ENE_TREE_RA];
			for (unsigned int nv = 0; nv < o->vertices.size(); nv++)
			{
				Point v = o->vertices[nv].coord;
				hull->addPoint(btVector3( v.x, v.y, v.z ));
			}
			npc->getBody()->setCollisionShape(hull);
		}


		itNPC++;
	}

	// Removes picked/destroyed elements
	// Removes elements out of range
	list<Reward*>::iterator itItem = items->begin();
	while(itItem != items->end())
	{
		Reward* item = *itItem;
		if(outOfRange(item))
		{
			deleteElement(item);
			itItem = items->erase(itItem);
			continue;
		}

		// Detect Collision
		bool collision = false;
		bool colParts[8] = {false};
		if (isKinectConnected)
		{
			//collision = isCollision(player->getBoundingBox(), player->getPos(), item->getBoundingBox(), item->getPos());
			// collision = isCollision(legoParts, colParts, item->getBB(), item->getPos());
			collision = isCollision(legoParts, colParts, mDynamicsWorld, item);
		}
		else if(!isKinectConnected)
		{
			collision = isCollision(player->getBB(), player->getPos(), item->getBB(), item->getPos());
		}
		if(collision)
		{
			// Give Life
			if(item->getType() == POT_LIFE)
			{
				if((int)item->getValue() && player->getMaxLife() != player->getLife())
					addMessage(QString("HP +%1").arg((int)item->getValue()), QColor(0,200,50));
				loadSound(sound,"/resources/sounds/life.ogg", GameInfo::Instance()->isSound());
				player->updateLife(item->getValue());
				cout << ":Get HP" << endl;
			}
			// Give Mana
			else if(item->getType() == POT_MANA)
			{
				loadSound(sound,"/resources/sounds/mana.ogg", GameInfo::Instance()->isSound());
				if((int)item->getValue() && player->getMaxMana() != player->getMana())
					addMessage(QString("MP +%1").arg((int)item->getValue()), QColor(0,120,200));
				player->updateMana(item->getValue());
				cout << ":Get MP" << endl;
			}
			// Give immunity
			else
			{
				mImmunity->stop();
				mImmunity->start(item->getValue());
				player->setImmune(true);
				player->setImmuneHit(false);
				loadSound(sound,"/resources/sounds/immunity.ogg", GameInfo::Instance()->isSound());
				addMessage(QString("%1 SECONDS IMMUNITY").arg((int)item->getValue()/1000), QColor(255,255,255));
				cout << ":Get IMMUNITY - 7s" << endl;
			}
			deleteElement(item);
			itItem = items->erase(itItem);
			continue;
		}
		itItem++;
	}
	list<Obstacle*>::iterator itObs = obstacles->begin();
	while(itObs != obstacles->end())
	{
		Obstacle* obs = *itObs;
		if(outOfRange(obs))
		{
			deleteElement(obs);
			itObs = obstacles->erase(itObs);
			continue;
		}

		// Detect Collision
		bool collision = false;
		bool colParts[8] = {false};
		if (isKinectConnected)
		{
			//collision = isCollision(player->getBoundingBox(), player->getPos(), obs->getBoundingBox(), obs->getPos());
			collision = isCollision(legoParts, colParts, obs->getBB(), obs->getPos());
			//collision = isCollision(legoParts, colParts, mDynamicsWorld, obs);
		}
		else if(!isKinectConnected)
		{
			collision = isCollision(player->getBB(), player->getPos(), obs->getBB(), obs->getPos());
		}
		if(collision)
		{
			// Lose life
			if(!obs->isWall() && !player->isImmune())
			{
				loadSound(sound,"/resources/sounds/damage2.ogg", GameInfo::Instance()->isSound());
				player->updateLife(obs->getDamage());
				addMessage(QString("HP %1").arg((int)obs->getDamage()), QColor(255,0,0));
				// 1 sec immunity for game clearness
				mImmunity->start(IMMUNITY_PER_HIT);
				player->setImmuneHit(true);
				player->setImmune(true);
			}
		}
		itObs++;
	}

	// Creates enemies
	srand(clock());
	uint seed = rand();
	if(Simple(seed) > diffProb && enemies->size() <  2.0*diffNum)
	{
		// Random point in scene
		float posX = (float)rand() / RAND_MAX * MAX_SCENE_WIDTH/2;
		posX -=	MAX_SCENE_WIDTH/4;
		float posZ = - MAX_SCENE_GAME_DEPTH - 1;
		NPC* npc = new NPC(Point(posX, 0.0f, posZ));
		float selection = Simple(seed);
		// Ghost
		if(selection >= 0.00 && selection < 0.25)
		{
			npc->setType(ENE_GHOST);
			npc->setModel(objList[ENE_GHOST]);
			npc->setDamage(-1.0f * (float)player->getLevel()/1.5 * diffAttack);
			npc->setExpPoints(5);
			npc->updatePosY(4.0f);
			createRigidBody(npc, btVector3(npc->getBB().distX()/2, npc->getBB().distY()/2, npc->getBB().distZ()/2),
				btVector3(posX, npc->getPos().y, posZ), 100000.0f);
		}
		// Tree
		else if(selection >= 0.25 && selection < 0.45)
		{
			npc->setType(ENE_TREE);
			npc->setModel(objList[ENE_TREE]);
			npc->setDamage(-2.0f * (float)player->getLevel()/1.5 * diffAttack);
			npc->setExpPoints(2);
			createRigidBody(npc, btVector3(npc->getBB().distX()/2, npc->getBB().distY()/2, npc->getBB().distZ()/2),
				btVector3(posX, 0.0, posZ), 100000.0f);
		}
		// Mole
		else if(selection >= 0.45 && selection < 0.65)
		{
			npc->setType(ENE_MOLE);
			npc->setModel(objList[ENE_MOLE]);
			npc->setDamage(-1.5f * (float)player->getLevel()/1.5 * diffAttack);
			npc->setExpPoints(3);
			createRigidBody(npc, btVector3(npc->getBB().distX()/2, npc->getBB().distY()/2, npc->getBB().distZ()/2),
				btVector3(posX, 0.0, posZ), 100000.0f);
		}
		// Spider
		else if(selection >= 0.65 && selection < 0.95)
		{
			npc->setType(ENE_SPIDER);
			npc->setModel(objList[ENE_SPIDER]);
			npc->setDamage(-1.0f * (float)player->getLevel()/1.5 * diffAttack);
			npc->setExpPoints(4);
			createRigidBody(npc, btVector3(npc->getBB().distX()/2, npc->getBB().distY()/2, npc->getBB().distZ()/2),
				btVector3(posX, 0.0, posZ), 100000.0f);
		}
		// Mushroom
		else
		{
			npc->setType(ENE_MUSHROOM);
			npc->setModel(objList[ENE_MUSHROOM]);
			npc->setDamage(-2.0f * (float)player->getLevel()/1.5 * diffAttack);
			npc->setExpPoints(1);
			npc->getModel()->setScaleFactor(Simple(seed));
			createRigidBody(npc, btVector3(npc->getBB().distX()/2, npc->getBB().distY()/2, npc->getBB().distZ()/2),
				btVector3(posX, 0.0, posZ), 100000.0f);
		}

		CollisionCallback collisionCallBack;
		collisionCallBack.collision = false;
		itNPC = enemies->begin();
		while(itNPC != enemies->end() && !collisionCallBack.collision)
		{
			NPC* auxNPC = *itNPC;
			mDynamicsWorld->contactPairTest(npc->getBody(), auxNPC->getBody(), collisionCallBack);
			itNPC++;
		}
		itItem = items->begin();
		while(itItem != items->end() && !collisionCallBack.collision)
		{
			Reward* auxItem = *itItem;
			mDynamicsWorld->contactPairTest(npc->getBody(), auxItem->getBody(), collisionCallBack);
			itItem++;
		}
		itObs = obstacles->begin();
		while(itObs != obstacles->end() && !collisionCallBack.collision)
		{
			Obstacle* auxObs = *itObs;
			collisionCallBack.collision = isCollision(npc->getBB(), npc->getPos(), auxObs->getBB(), auxObs->getPos());
			itObs++;
		}
		if(!collisionCallBack.collision)
		{
			npc->getModel()->setTextureId(NULL);
			enemies->push_back(npc);
		}
		else
			delete npc;
	}

	// Creates rewards
	seed = rand();
	if(Simple(seed) > 0.98 && items->size() < 4/diffNum)
	{
		// Random point in scene
		float posX = (float)rand() / RAND_MAX * MAX_SCENE_WIDTH/2;
		posX -= MAX_SCENE_WIDTH/4;
		float posZ = - MAX_SCENE_GAME_DEPTH - 1;
		Reward* item = new Reward(Point(posX, 0.0f, posZ));
		item->setDir(Vector(0.0f, 0.0f, GROUND_SPEED));
		float selection = Simple(seed);
		if(selection >= 0.00 && selection < 0.40)
		{
			// Life potion
			item->setType(POT_LIFE);
			item->setValue(player->getMaxLife()/5);
			item->setModel(objList[POT_LIFE]);
		}
		else if(selection >= 0.40 && selection < 0.95)
		{
			// Mana potion
			item->setType(POT_MANA);
			item->setValue(ceil(player->getMaxMana()/2));
			item->setModel(objList[POT_MANA]);
		}
		else
		{
			// Immunity potion
			item->setType(POT_IMMUNITY);
			item->setValue(IMMUNITY);
			item->setModel(objList[POT_IMMUNITY]);
		}

		createRigidBody(item, btVector3(item->getBB().distX()/2, item->getBB().distY()/2, item->getBB().distZ()/2),
			btVector3(posX, 0.0, posZ), 1.0f);

		CollisionCallback collisionCallBack;
		collisionCallBack.collision = false;
		itNPC = enemies->begin();
		while(itNPC != enemies->end() && !collisionCallBack.collision)
		{
			NPC* auxNPC = *itNPC;
			mDynamicsWorld->contactPairTest(item->getBody(), auxNPC->getBody(), collisionCallBack);
			itNPC++;
		}
		itItem = items->begin();
		while(itItem != items->end() && !collisionCallBack.collision)
		{
			Reward* auxItem = *itItem;
			mDynamicsWorld->contactPairTest(item->getBody(), auxItem->getBody(), collisionCallBack);
			itItem++;
		}
		itObs = obstacles->begin();
		while(itObs != obstacles->end() && !collisionCallBack.collision)
		{
			Obstacle* auxObs = *itObs;
			collisionCallBack.collision = isCollision(item->getBB(), item->getPos(), auxObs->getBB(), auxObs->getPos());
			itObs++;
		}
		if(!collisionCallBack.collision)
			items->push_back(item);
		else
			delete item;
	}	

	// Creates obstacles
	seed = rand();
	if(Simple(seed) > diffProb && obstacles->size() < 3*diffNum)
	{
		// Random point in scene
		float posX = (float)rand() / RAND_MAX * MAX_SCENE_WIDTH/2;
		posX -= MAX_SCENE_WIDTH/4;
		float posZ = (float)rand() / RAND_MAX * MAX_SCENE_WIDTH/8;
		posZ = -MAX_SCENE_GAME_DEPTH;
		Obstacle* obs = new Obstacle(Point(posX, 0.0f, posZ));
		obs->setDir(Vector(0.0f, 0.0f, GROUND_SPEED));
		if(Simple(seed) > 1.0)
		{
			// It is a simple wall to avoid (Bricks = 1; Bush = 2)
			if(Simple(seed) > 0.5)
				obs->setWall(1);
			else
				obs->setWall(2);
			obs->setModel(objList[OBS_WALL]);
		}
		else
		{
			// Spikes can damage the player
			obs->setWall(false);
			obs->setDamage(-2.5f * (float)player->getLevel()/1.5 * diffAttack);
			obs->setModel(objList[OBS_SPIKES]);
			obs->setType(OBS_SPIKES);
		}

		createRigidBody(obs, btVector3(obs->getBB().distX()/2, obs->getBB().distY()/2, obs->getBB().distZ()/2),
			btVector3(posX, 0.0f, posZ), 1.0f, false);
		obs->setShape(mCollisionShapes.at(OBS_SPIKES));

		CollisionCallback collisionCallBack;
		collisionCallBack.collision = false;
		itNPC = enemies->begin();
		while(itNPC != enemies->end() && !collisionCallBack.collision)
		{
			NPC* auxNPC = *itNPC;
			mDynamicsWorld->contactPairTest(obs->getBody(), auxNPC->getBody(), collisionCallBack);
			itNPC++;
		}
		itItem = items->begin();
		while(itItem != items->end() && !collisionCallBack.collision)
		{
			Reward* auxItem = *itItem;
			mDynamicsWorld->contactPairTest(obs->getBody(), auxItem->getBody(), collisionCallBack);
			itItem++;
		}
		itObs = obstacles->begin();
		while(itObs != obstacles->end() && !collisionCallBack.collision)
		{
			Obstacle* auxObs = *itObs;
			mDynamicsWorld->contactPairTest(obs->getBody(), auxObs->getBody(), collisionCallBack);
			itObs++;
		}
		if(!collisionCallBack.collision)
			obstacles->push_back(obs);
		else
			delete obs;
	}		

	// Clean old ranged shoots
	list<Attack*>::iterator itShoot = player->getShoots()->begin();
	while(itShoot != player->getShoots()->end())
	{
		Attack *bullet = *itShoot;
		if(bullet->getType() == RNG_BULLET && bullet->getPos().z < -MAX_SCENE_GAME_DEPTH - 5)
		{
			deleteElement(bullet);
			itShoot = player->getShoots()->erase(itShoot);
		}
		else if(bullet->getType() == RNG_AOE1 && bullet->getPos().z < -MAX_SCENE_GAME_DEPTH + 5)
		{
			deleteElement(bullet);
			itShoot = player->getShoots()->erase(itShoot);
		}
		else
			++itShoot;
	}

	// Update all stats if level up occurs
	updateLevel();
	// Update SceneGUI (life, mana, exp, points, time)
	updateGUI();
}

void Scene::updateLevel()
{
	if(player->getExp() >= player->getMaxExp())
	{
		player->levelUp();
		addMessage(QString("Level Up!"), QColor(255,127,0));
		loadSound(sound,"/resources/sounds/levelup.ogg", GameInfo::Instance()->isSound());
		currentTime -= 30000*log((float)player->getLevel());
		// Potions need to be updated accordingly to % of mana and life stats
		list<Reward*>::iterator itItem = items->begin();
		while(itItem != items->end())
		{
			Reward* item = *itItem;
			if(item->getType() == POT_LIFE)
				item->setValue(ceil(player->getMaxMana()/2));
			else if(item->getType() == POT_MANA)
				item->setValue(player->getMaxLife()/5);
			itItem++;
		}
	}
}

void Scene::updateGUI()
{
	viewGUI->updateLife(ceil(player->getLife()), ceil(player->getMaxLife()));
	viewGUI->updateMana(floor(player->getMana()), floor(player->getMaxMana()));
	viewGUI->updateExp(player->getExp(), player->getMinExp(), player->getMaxExp(), player->getLevel());
	viewGUI->updateTime( (gameTime - currentTime)/1000 );
}

bool Scene::outOfRange(Element* e)
{
	if(e->getPos().x > MAX_SCENE_WIDTH/2 || e->getPos().x < -MAX_SCENE_GAME_DEPTH/2)
		return true;
	//if(e->getPos().y > MAX_SCENE_WIDTH/2 || e->getPos().y < 0.0f)
	//	return true;
	if(e->getPos().z > 5.0f)
		return true;

	return false;
}

void Scene::messagesUpdate()
{
	GLdouble *modelview = new GLdouble[16];
	GLdouble *projection = new GLdouble[16];
	GLint *viewport = new GLint[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	GLdouble tx, ty, tz;

	Point v;
	if(!isKinectConnected)
		v = player->getPos();
	else if(kinect.getTrackingStatus() == TRACKING)
		v = legoParts[HEAD]->getPos();
	list<Message>::iterator it = messages->begin();
	int i = 0;
	while(it != messages->end())
	{
		// Updating its timing and removing in case it is more than 1 sec
		if(it->count >= 60)
		{
			it = messages->erase(it);
			continue;
		}
		else
			it->count++;

		// Rendering the message
		int offset = 0;
		if(!isKinectConnected)
			offset = ceil(player->getBB().distY()) + i;
		else if(kinect.getTrackingStatus() == TRACKING)
			offset = ceil(legoParts[HEAD]->getBB().distY()) + i;

		gluProject(v.x, v.y + offset, v.z, modelview, projection, viewport, &tx, &ty, &tz);
		glColor4f(it->color.red()/(float)255, it->color.green()/(float)255, it->color.blue()/(float)255, 1.0f - it->count/(float)60);
		renderText(tx - it->text.size()*7, mHeight - ty, it->text, it->font);
		it++;
		i++;
	}
}

void Scene::addMessage(QString text, QColor c)
{
	messages->push_back(Message(text, c));
}

void Scene::deleteElement(Element* elem)
{
	if(elem->getBody() != NULL)
	{
		if (elem->getBody()->getMotionState())
			delete elem->getBody()->getMotionState();
		mDynamicsWorld->removeRigidBody(elem->getBody());
		elem->getBody()->setCollisionShape(NULL);
		delete elem->getBody();
	}
	delete elem;
}

void Scene::projectionPerspective(int width, int height)
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	calcFov(width, height);
	gluPerspective( fov, (GLfloat)width/(GLfloat)height, Znear, Zfar); // fov, ar, Znear, Zfar
}

void Scene::projectionOrthogonal(int width, int height)
{
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(-width, width, -height, height, Znear, Zfar);
}

void Scene::createTextures(const char* filename, GLuint* textureId)
{
	QImage image, imageGL;
	if ( !image.load(filename) )
	{
		image = QImage();
		image.fill(Qt::green);
	}
	imageGL = QGLWidget::convertToGLFormat(image);
	glGenTextures(1, textureId);
	glBindTexture(GL_TEXTURE_2D, *textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imageGL.width(), imageGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageGL.bits());
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void Scene::initMaterials()
{
	matSpec[0] = 1.0f; matSpec[1] = 1.0f; matSpec[2] = 1.0f; matSpec[3] = 1.0f; 
	matRed[0] = 1.0f; matRed[1] = 0.0f; matRed[2] = 0.0f; matRed[3] = 1.0f;
	matGreen[0] = 0.0f; matGreen[1] = 1.0f; matGreen[2] = 0.0f; matGreen[3] = 1.0f;
	matBlue[0] = 0.0f; matBlue[1] = 0.0f; matBlue[2] = 1.0f; matBlue[3] = 1.0f;
	matBlack[0] = 0.0f; matBlack[1] = 0.0f; matBlack[2] = 0.0f; matBlack[3] = 1.0f;
	matWhite[0] = 1.0f; matWhite[1] = 1.0f; matWhite[2] = 1.0f; matWhite[3] = 1.0f;
}

void Scene::calcFov(int width, int height)
{
	float dist = Zfar;
	float radius = dist / 2;
	float ar = (float)width / (float)height;
	// cout << "Aspect ratio: " << ar << endl;

	// Angle calculation
	float alpha = 0.0, alphaH, lx, ly;
	// Depending on the AR, we adapt the angle...
	// If ar >= 1 Yaxis angle is enough
	if(ar >= 1)
		alpha = asin( radius / dist);
	// We must get an Yaxis angle based on Xaxis in order to see the whole scene
	else if(ar < 1)
	{
		alphaH = asin( radius / dist );
		lx = dist * tan( alphaH );
		ly = lx / ar;
		alpha = atan( ly / dist );
	}
	// Field of View is calculated converting from radiants to degrees
	fov = 2 * alpha*180/PI;
	// cout << "fov: " << fov << endl;
}

// Calculates the matrix for object projection on the ground for creating shadows
void Scene::shadowProjectionMatrix(Point normal)
{
	// Our reference: la normal of the ground given as an input parameter and the light point
	GLfloat aux = normal.x * lightPos[0] + normal.y * lightPos[1] + normal.z * lightPos[2] + 0.0f * lightPos[3];

	shadowMatrix[0][0] = aux - lightPos[0] * normal.x;
	shadowMatrix[1][0] = 0.0f - lightPos[0] * normal.y;
	shadowMatrix[2][0] = 0.0f - lightPos[0] * normal.z;
	shadowMatrix[3][0] = 0.0f - lightPos[0] * 0.0f;

	shadowMatrix[0][1] = 0.0f - lightPos[1] * normal.x;
	shadowMatrix[1][1] = aux - lightPos[1] * normal.y;
	shadowMatrix[2][1] = 0.0f - lightPos[1] * normal.z;
	shadowMatrix[3][1] = 0.0f - lightPos[1] * 0.0f;

	shadowMatrix[0][2] = 0.0f - lightPos[2] * normal.x;
	shadowMatrix[1][2] = 0.0f - lightPos[2] * normal.y;
	shadowMatrix[2][2] = aux - lightPos[2] * normal.z;
	shadowMatrix[3][2] = 0.0f - lightPos[2] * 0.0f;

	shadowMatrix[0][3] = 0.0f - lightPos[3] * normal.x;
	shadowMatrix[1][3] = 0.0f - lightPos[3] * normal.y;
	shadowMatrix[2][3] = 0.0f - lightPos[3] * normal.z;
	shadowMatrix[3][3] = aux - lightPos[3] * 0.0f;
}

void Scene::setDisplayLists()
{
	Object* obj = new Object();
	glNewList(dList[PLAYER], GL_COMPILE);
		Object::setModel("/resources/models/LegoMan.obj", obj);
		drawModel(obj, false);
		objList[PLAYER] = obj;
	glEndList();
	dList[PLAYER_SHADOW] = dList[PLAYER] + 1;
	glNewList(dList[PLAYER_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/LegoMan.obj", obj);
		drawModel(obj, true);
		objList[PLAYER_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[PLAYER_HIT] = dList[PLAYER_SHADOW] + 1;
	glNewList(dList[PLAYER_HIT], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/LegoMan.obj", obj);
		drawModel(obj, false);
		objList[PLAYER_HIT] = NULL;
		delete obj;
	glEndList();
	dList[PLAYER_IMMUNE] = dList[PLAYER_HIT] + 1;
	glNewList(dList[PLAYER_IMMUNE], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/LegoMan.obj", obj);
		drawModel(obj, false);
		objList[PLAYER_IMMUNE] = NULL;
		delete obj;
	glEndList();
	dList[TREE] = dList[PLAYER_IMMUNE] + 1;
	glNewList(dList[TREE], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/Tree.obj", obj);
		drawModel(obj, false);
		objList[TREE] = obj;
	glEndList();
	dList[TREE_SHADOW] = dList[TREE] + 1;
	glNewList(dList[TREE_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/Tree.obj", obj);
		drawModel(obj, true);
		objList[TREE_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[POT_LIFE] = dList[TREE_SHADOW] + 1;
	glNewList(dList[POT_LIFE], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/potions/Life.obj", obj);
		drawModel(obj, false);
		objList[POT_LIFE] = obj;
	glEndList();
	dList[POT_LIFE_SHADOW] = dList[POT_LIFE] + 1;
	glNewList(dList[POT_LIFE_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/potions/Life.obj", obj);
		drawModel(obj, true);
		objList[POT_LIFE_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[POT_MANA] = dList[POT_LIFE_SHADOW] + 1;
	glNewList(dList[POT_MANA], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/potions/Mana.obj", obj);
		drawModel(obj, false);
		objList[POT_MANA] = obj;
	glEndList();
	dList[POT_MANA_SHADOW] = dList[POT_MANA] + 1;
	glNewList(dList[POT_MANA_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/potions/Mana.obj", obj);
		drawModel(obj, true);
		objList[POT_MANA_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[POT_IMMUNITY] = dList[POT_MANA_SHADOW] + 1;
	glNewList(dList[POT_IMMUNITY], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/potions/Immunity.obj", obj);
		drawModel(obj, false);
		objList[POT_IMMUNITY] = obj;
	glEndList();
	dList[POT_IMMUNITY_SHADOW] = dList[POT_IMMUNITY] + 1;
	glNewList(dList[POT_IMMUNITY_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/potions/Immunity.obj", obj);
		drawModel(obj, true);
		objList[POT_IMMUNITY_SHADOW] = NULL;
		delete obj;
	glEndList();	
	dList[OBS_WALL] = dList[POT_IMMUNITY_SHADOW] + 1;
	glNewList(dList[OBS_WALL], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/walls/Wall.obj", obj);
		drawModel(obj, false);
		objList[OBS_WALL] = obj;
	glEndList();
	dList[OBS_WALL_SHADOW] = dList[OBS_WALL] + 1;
	glNewList(dList[OBS_WALL_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/walls/Wall.obj", obj);
		drawModel(obj, true);
		objList[OBS_WALL_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[OBS_SPIKES] = dList[OBS_WALL_SHADOW] + 1;
	glNewList(dList[OBS_SPIKES], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/walls/Spikes3.obj", obj);
		drawModel(obj, false, true);
		objList[OBS_SPIKES] = obj;
	glEndList();
	dList[OBS_SPIKES_SHADOW] = dList[OBS_SPIKES] + 1;
	glNewList(dList[OBS_SPIKES_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/walls/Spikes3.obj", obj);
		drawModel(obj, true, true);
		objList[OBS_SPIKES_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[RNG_BULLET] = dList[OBS_SPIKES_SHADOW] + 1;
	glNewList(dList[RNG_BULLET], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/bullets/Bullet.obj", obj);
		drawModel(obj, false);
		objList[RNG_BULLET] = obj;
	glEndList();
	dList[RNG_AOE1] = dList[RNG_BULLET] + 1;
	glNewList(dList[RNG_AOE1], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/bullets/AOE1.obj", obj);
		drawModel(obj, false);
		objList[RNG_AOE1] = obj;
	glEndList();
	dList[RNG_AOE2] = dList[RNG_AOE1] + 1;
	glNewList(dList[RNG_AOE2], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/bullets/AOE2.obj", obj);
		drawModel(obj, false);
		objList[RNG_AOE2] = obj;
	glEndList();
	dList[ENE_GHOST] = dList[RNG_AOE2] + 1;
	glNewList(dList[ENE_GHOST], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Ghost.obj", obj);
		drawModel(obj, false);
		objList[ENE_GHOST] = obj;
	glEndList();
	dList[ENE_GHOST_SHADOW] = dList[ENE_GHOST] + 1;
	glNewList(dList[ENE_GHOST_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Ghost.obj", obj);
		drawModel(obj, true);
		objList[ENE_GHOST_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[ENE_TREE] = dList[ENE_GHOST_SHADOW] + 1;
	glNewList(dList[ENE_TREE], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Tree.obj", obj);
		drawModel(obj, false);
		objList[ENE_TREE] = obj;
	glEndList();
	dList[ENE_TREE_SHADOW] = dList[ENE_TREE] + 1;
	glNewList(dList[ENE_TREE_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Tree.obj", obj);
		drawModel(obj, true);
		objList[ENE_TREE_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[ENE_TREE_LA] = dList[ENE_TREE_SHADOW] + 1;
	glNewList(dList[ENE_TREE_LA], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Tree_Left_Arm.obj", obj);
		drawModel(obj, false);
		objList[ENE_TREE_LA] = obj;
	glEndList();
	dList[ENE_TREE_LA_SHADOW] = dList[ENE_TREE_LA] + 1;
	glNewList(dList[ENE_TREE_LA_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Tree_Left_Arm.obj", obj);
		drawModel(obj, true);
		objList[ENE_TREE_LA_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[ENE_TREE_RA] = dList[ENE_TREE_LA_SHADOW] + 1;
	glNewList(dList[ENE_TREE_RA], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Tree_Right_Arm.obj", obj);
		drawModel(obj, false);
		objList[ENE_TREE_RA] = obj;
	glEndList();
	dList[ENE_TREE_RA_SHADOW] = dList[ENE_TREE_RA] + 1;
	glNewList(dList[ENE_TREE_RA_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Tree_Right_Arm.obj", obj);
		drawModel(obj, true);
		objList[ENE_TREE_RA_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[ENE_MOLE] = dList[ENE_TREE_RA_SHADOW] + 1;
	glNewList(dList[ENE_MOLE], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Mole.obj", obj);
		drawModel(obj, false);
		objList[ENE_MOLE] = obj;
	glEndList();
	dList[ENE_MOLE_SHADOW] = dList[ENE_MOLE] + 1;
	glNewList(dList[ENE_MOLE_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Mole.obj", obj);
		drawModel(obj, true);
		objList[ENE_MOLE_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[ENE_MOLE_BASE] = dList[ENE_MOLE_SHADOW] + 1;
	glNewList(dList[ENE_MOLE_BASE], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Mole_Base.obj", obj);
		drawModel(obj, false);
		objList[ENE_MOLE_BASE] = NULL;
		delete obj;
	glEndList();
	dList[ENE_MOLE_BASE_SHADOW] = dList[ENE_MOLE_BASE] + 1;
	glNewList(dList[ENE_MOLE_BASE_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Mole_Base.obj", obj);
		drawModel(obj, true);
		objList[ENE_MOLE_BASE_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[ENE_SPIDER] = dList[ENE_MOLE_BASE_SHADOW] + 1;
	glNewList(dList[ENE_SPIDER], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Spider.obj", obj);
		drawModel(obj, false);
		objList[ENE_SPIDER] = obj;
	glEndList();
	dList[ENE_SPIDER_SHADOW] = dList[ENE_SPIDER] + 1;
	glNewList(dList[ENE_SPIDER_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Spider.obj", obj);
		drawModel(obj, true);
		objList[ENE_SPIDER_SHADOW] = NULL;
		delete obj;
	glEndList();
	dList[ENE_MUSHROOM] = dList[ENE_SPIDER_SHADOW] + 1;
	glNewList(dList[ENE_MUSHROOM], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Mushroom.obj", obj);
		drawModel(obj, false);
		objList[ENE_MUSHROOM] = obj;
	glEndList();
	dList[ENE_MUSHROOM_SHADOW] = dList[ENE_MUSHROOM] + 1;
	glNewList(dList[ENE_MUSHROOM_SHADOW], GL_COMPILE);
		obj = new Object();
		Object::setModel("/resources/models/enemies/Mushroom.obj", obj);
		drawModel(obj, true);
		objList[ENE_MUSHROOM_SHADOW] = NULL;
		delete obj;
	glEndList();
}

/* SLOTS */
void Scene::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Qt::Key_P:
		{
			pauseGame();
			break;
		}
		case Qt::Key_O:
		{
			backMainMenu();
			break;
		}
		case Qt::Key_L:
		{
			isLight = !isLight;
			if(isLight)
			{
				glEnable(GL_LIGHTING);
				cout << "Change Light: ON" << endl;
			}
			else
			{
				glDisable(GL_LIGHTING);
				cout << "Change Light: OFF" << endl;
			}
			break;
		}
		case Qt::Key_S:
		{
			isShadow = !isShadow;
			if(isShadow)
			{
				cout << "Shadows: ON" << endl;
			}
			else
			{
				cout << "Shadows: OFF" << endl;
			}
			break;
		}
		case Qt::Key_Q:
		{
			// viewGUI->updateLife(-0.1f);
			break;
		}
		case Qt::Key_W:
		{
			// viewGUI->updateMana(-0.2f);
			break;
		}
		case Qt::Key_A:
		{
			// cout << "ATTACK!!!!" << endl;
			loadSound(sound,"/resources/sounds/hit.ogg", GameInfo::Instance()->isSound());
			player->setAttack(true);
			break;
		}
		case Qt::Key_Space:
		{
			Attack *bullet = new Attack();
			if(player->getMana() >= 1.0f)
			{
				if (!isKinectConnected)
				{
					loadSound(sound,"/resources/sounds/bullet.ogg", GameInfo::Instance()->isSound());
					bullet->setModel(objList[RNG_BULLET]);
					bullet->setType(RNG_BULLET);
					createRigidBody(bullet, btVector3(bullet->getBB().distX()/2, bullet->getBB().distY()/2, bullet->getBB().distZ()/2),
						btVector3(player->getPos().x, player->getBB().distY()/2 + bullet->getBB().distY()/2, player->getPos().z - bullet->getBB().distZ()/2), 1.0f);
					bullet->setPos(Point(player->getPos().x, player->getBB().distY()/2 + bullet->getBB().distY()/2, player->getPos().z));
					bullet->getBody()->setLinearVelocity(btVector3(0.0, 0.0, -50.0f));
					player->addShoot(bullet);
					player->updateMana(-1.0f);
				}
				else if(kinect.getTrackingStatus() == TRACKING) 
				{
					XnPoint3D rightHandPos = kinect.getJoint(XN_SKEL_RIGHT_HAND);
					Attack *bullet = new Attack();
					loadSound(sound,"/resources/sounds/bullet.ogg", GameInfo::Instance()->isSound());
					bullet->setModel(objList[RNG_BULLET]);
					createRigidBody(bullet, btVector3(bullet->getBB().distX()/2, bullet->getBB().distY()/2, bullet->getBB().distZ()/2),
						btVector3(rightHandPos.X, rightHandPos.Y/2 + bullet->getBB().distY()/2, rightHandPos.Z - bullet->getBB().distZ()/2), 1.0f);
					bullet->setPos(Point(rightHandPos.X,rightHandPos.Y/2 + bullet->getBB().distY()/2, rightHandPos.Z));
					bullet->getBody()->setLinearVelocity(btVector3(0.0, 0.0, -50.0f));
					player->addShoot(bullet);
					player->updateMana(-1.0f);		
				}
			}				
			break;
		}
		case Qt::Key_Up : 
		{
			if(!isKinectConnected)
				player->getBody()->setLinearVelocity(btVector3(0.0, 0.0, -AVATAR_SPEED));
			// isFront = true;
			break; 
		}
		case Qt::Key_Down : 
		{
			if(!isKinectConnected)
				player->getBody()->setLinearVelocity(btVector3(0.0, 0.0, AVATAR_SPEED));
			// isBack = true;
			break; 
		}
		case Qt::Key_Left : 
		{
			if(!isKinectConnected)
				player->getBody()->setLinearVelocity(btVector3(-AVATAR_SPEED, 0.0, 0.0));
			// isLeft = true;
			break;
		}
		case Qt::Key_Right : 
		{
			if(!isKinectConnected)
				player->getBody()->setLinearVelocity(btVector3(AVATAR_SPEED, 0.0, 0.0));
			// isRight = true;
			break;
		}
		case Qt::Key_PageDown : 
		{	
			rotation += 5.0f;
			break;
		}
		case Qt::Key_PageUp: 
		{
			rotation -= 5.0f;
			break;
		}
		default:
			break;			
	}
}

void Scene::keyReleaseEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Qt::Key_A:
		{
			// cout << "no attack" << endl;
			player->setAttack(false);
			break;
		}
		case Qt::Key_Up :
		{
			if(!isKinectConnected)
				player->getBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
			// isFront = false;
			break; 
		}
		case Qt::Key_Down :
		{
			if(!isKinectConnected)
				player->getBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
			// isBack = false;
			break; 
		}
		case Qt::Key_Left : 
		{
			if(!isKinectConnected)
				player->getBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
			// isLeft = false;
			break;
		}
		case Qt::Key_Right : 
		{
			if(!isKinectConnected)
				player->getBody()->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
			// isRight = false;
			break;
		}
	}
}

// Wait till kinect calibrated and proceed
void Scene::kinectTracking()
{
	kinect.draw_avatar(legoParts, false);
	popInit->updateText(kinect.getTrackingStatus());
	if(kinect.getTrackingStatus() == TRACKING)
	{
		mKinect->stop();
		// Time in screen updated each second
		mTime->start(1000);
		mRefresh->start(1000/30);
		popInit->setVisible(false);
	}
}

void Scene::updateTime()
{
	currentTime += 1000;	
}

void Scene::endGame()
{
	loadSound(sound,"/resources/sounds/end.ogg", GameInfo::Instance()->isSound());
	currentTime = gameTime;
	viewGUI->stopRefresh();
	mRefresh->stop();
	mTime->stop();

	mEnd = new QTimer(this);
	connect( mEnd, SIGNAL( timeout() ), SLOT( showResults() ) );
	mEnd->start(500);

	QString diff;
	switch(GameInfo::Instance()->getDifficulty())
	{
		case EASY:
			{
				diff = QString("Easy");
				break;
			}
		case MEDIUM:
		{
			diff = QString("Medium");
			break;
		}
		case HARD:
		{
			diff = QString("Hard");
			break;
		}
	}
	if(!end)
		ScoreManager::writeScores(player->getLevel(), player->getExp(), player->getEnemiesDown(), diff);
	end = true;
	emit updateScores();
}

void Scene::showResults()
{
	mEnd->stop();
	QApplication::setOverrideCursor(QCursor(QPixmap(":/fonts/cursor2.gif"), 0, 0));
	
	// Update labels with final results!
	popEnd->updateText(player->getLevel(), player->getExp(), player->getEnemiesDown());
	// Make the result pop up menu visible
	popEnd->setVisible(true);
}

void Scene::noImmunity()
{
	player->setImmune(false);
	player->setImmuneHit(false);
	immuneCounter = 0;
	immuneIncr = 1;
	mImmunity->stop();
}

void Scene::backMainMenu()
{
	pop->setVisible(false);
	popEnd->setVisible(false);
	delete pop;
	delete popEnd;
	delete popInit;

	// Freeee
	delete celTex;
	delete celMtl;
	delete hit;
	delete immune;
	delete viewGUI;
	delete player;
	delete enemies;
	delete items;
	delete obstacles;
	delete messages;
	delete dList;
	delete objList;

	QApplication::setOverrideCursor(QCursor(QPixmap(":/fonts/cursor2.gif"), 0, 0));
	emit exitGame(0, true);
}

void Scene::pauseGame()
{
	pop->setVisible(true);
	QApplication::setOverrideCursor(QCursor(QPixmap(":/fonts/cursor2.gif"), 0, 0));
	mRefresh->stop();
	mTime->stop();
}

void Scene::backGame()
{
	pop->setVisible(false);
	QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
	mRefresh->start(1000/30);
	mTime->start(1000);
}

void loadSound(Mix_Chunk *sound, char *path, bool isSound, bool loop)
{
	if(isSound)
	{
		QString pathTestObj = QDir::currentPath().append(path);
		if(getenv("windir") != NULL)
			pathTestObj.replace("/", "\\");
                //sound->play2D(pathTestObj.toAscii().data(), loop);
                sound = Mix_LoadWAV(pathTestObj.toAscii().data());
                if (loop)
                {
                    Mix_PlayChannel(-1, sound, -1);
                }
                else
                {
                    Mix_PlayChannel(-1, sound, 0);
                }
	}
}

void initSound()
{
  	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 4096;

	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers);
}
