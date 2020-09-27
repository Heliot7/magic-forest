#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <QString>
#include <QFont>
#include <QFontDatabase>

#include "../headers/render/SceneGUI.h"

#define LINE_WIDTH 2.0f
#define MARGIN_ICONS 5.0f
#define OFFSET (MARGIN_ICONS-LINE_WIDTH)
#define FONT_SIZE 18
#define FONT_FAMILY "RussianQuality"

using namespace std;

// Constructor
SceneGUI::SceneGUI(QWidget* parent, int width, int height) : QGLWidget(parent)
{
	mTime = new QTimer(this);
	// FPS = 30
	mTime->start(1000/30);
	connect( mTime, SIGNAL( timeout() ), SLOT( updateGL() ) );
	// Set default text
	QFontDatabase::addApplicationFont(":/fonts/russianquality.ttf");
	QFontDatabase::addApplicationFont(":/fonts/vivala.ttf");
	textLife = new QString("");
	textMana = new QString("");
	textExp = new QString("");
	textTime = new QString("");

	// Set In-game values
	mLife = 10.0f;
	mMana = 0.0f;
	mExp = 0.0f;
	maxLife = 10.0f;
	maxMana = 0.0f;
	minExp = 0.0f;
	maxExp = 25.0f;
}

// Destructor
SceneGUI::~SceneGUI()
{
}

/* OpenGL methods */
void SceneGUI::initializeGL()
{
	// Basic scene charactistics
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glShadeModel(GL_SMOOTH); // GL_SMOOTH
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// Normalized values in the pipeline
	glEnable(GL_NORMALIZE);
	// Hidden parts are deleted
	glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
	// Blending
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize textures
	createTextures(":/textures/GUI/background3.jpg", &texBackground);
	createTextures(":/textures/GUI/heart1.png", &texHeart);
	createTextures(":/textures/GUI/mana2.png", &texMana);
	createTextures(":/textures/GUI/exp1.png", &texExp);
	createTextures(":/textures/GUI/time4.png", &texTime);

	// Setup camera and scene
	vrp = Point(0.0f, 0.0f, 0.0f);
	camPos = Point(vrp.x, vrp.y, 1.0f);
	vuv = Point(0.0f, 1.0f, 0.0f);
	Znear = 0.1f; // camPos.z - 1;
	Zfar = 100.0f; // camPos.z + 1;
}

void SceneGUI::resizeGL(int w, int h)
{
	mWidth = w;
	mHeight = 75;
	glViewport(0, 0, mWidth, mHeight);
	// Make it smaller than the main openGL window
	setFixedSize(mWidth, mHeight);
	setAutoFillBackground(false);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(0, mWidth, 0, mHeight, Znear, Zfar);
}

// Draw the scene
void SceneGUI::paintGL()
{	
	glDisable(GL_LIGHTING);
	// All is reset
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Setup camera
	gluLookAt(camPos.x, camPos.y, camPos.z, vrp.x, vrp.y, vrp.z, vuv.x, vuv.y, vuv.z);

    glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);

	// Draw Back Wall
	drawBackground();
	// Draw Heart - life
	drawIcon(MARGIN_ICONS, OFFSET + mHeight/2-MARGIN_ICONS, mHeight/2-MARGIN_ICONS, mHeight/2-MARGIN_ICONS, texHeart);
	int min = mHeight/2 + MARGIN_ICONS;
	glColor4f((1.0f - (float)mLife/maxLife)*1.0f, (float)mLife/maxLife*0.7f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(min, min + 2, 0.5f);
		glVertex3f(min + (float)mLife/maxLife*mWidth/5, min + 2, 0.5f);
		glVertex3f(min + (float)mLife/maxLife*mWidth/5, mHeight-MARGIN_ICONS - 5, 0.5f);
		glVertex3f(min, mHeight-MARGIN_ICONS - 5, 0.5f);
	glEnd();
	glLineWidth(LINE_WIDTH);
	glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	glBegin(GL_LINES);
		glVertex3f(min, min + 2, 0.6f); glVertex3f(min + mWidth/5, min + 2, 0.6f);
		glVertex3f(min + mWidth/5, min + 2, 0.6f); glVertex3f(min + mWidth/5, mHeight-MARGIN_ICONS - 5, 0.6f);
		glVertex3f(min + mWidth/5, mHeight-MARGIN_ICONS - 5, 0.6f); glVertex3f(min, mHeight-MARGIN_ICONS - 5, 0.6f);
		glVertex3f(min, mHeight-MARGIN_ICONS - 5, 0.6f); glVertex3f(min, min + 2, 0.6f);
	glEnd();
	// Draw potion - magic points (mana)
	drawIcon(MARGIN_ICONS, MARGIN_ICONS, mHeight/2-MARGIN_ICONS, mHeight/2-MARGIN_ICONS - OFFSET, texMana);
	glColor4f(0.0f, 0.0f, 0.8f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(min, MARGIN_ICONS + 5, 0.5f);
		float aux;
		if(maxMana == 0.0f)
			aux = min;
		else
			aux = min + (float)mMana/maxMana*mWidth/5;
		glVertex3f(aux, MARGIN_ICONS + 5, 0.5f);
		glVertex3f(aux, mHeight/2 - OFFSET - 4, 0.5f);
		glVertex3f(min, mHeight/2 - OFFSET - 4, 0.5f);
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	glBegin(GL_LINES);
		glVertex3f(min, MARGIN_ICONS + 5, 0.6f); glVertex3f(min + mWidth/5, MARGIN_ICONS + 5, 0.6f);
		glVertex3f(min + mWidth/5, MARGIN_ICONS + 5, 0.6f); glVertex3f(min + mWidth/5, mHeight/2 - OFFSET - 4, 0.6f);
		glVertex3f(min + mWidth/5, mHeight/2 - OFFSET - 4, 0.6f); glVertex3f(min, mHeight/2 - OFFSET - 4, 0.6f);
		glVertex3f(min, mHeight/2 - OFFSET - 4, 0.6f); glVertex3f(min, MARGIN_ICONS + 5, 0.6f);
	glEnd();
	// Draw Experience
	drawIcon(2.1/5*mWidth, MARGIN_ICONS, mHeight-2*MARGIN_ICONS, mHeight-2*MARGIN_ICONS, texExp);
	// Draw Experience bar
	glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
	float offsetExp = 2.1/5*mWidth + mHeight;
	glBegin(GL_QUADS);
		glVertex3f(offsetExp, min + 2, 0.5f);
		glVertex3f(offsetExp + (float)(mExp-minExp)/(maxExp-minExp)*mWidth/3.2, min + 2, 0.5f);
		glVertex3f(offsetExp + (float)(mExp-minExp)/(maxExp-minExp)*mWidth/3.2, mHeight-MARGIN_ICONS - 5, 0.5f);
		glVertex3f(offsetExp, mHeight-MARGIN_ICONS - 5, 0.5f);
	glEnd();
	glLineWidth(LINE_WIDTH);
	glColor4f(0.5f, 0.5f, 0.0f, 0.8f);
	glBegin(GL_LINES);
		glVertex3f(offsetExp, min + 2, 0.6f); glVertex3f(offsetExp + mWidth/3.2, min + 2, 0.6f);
		glVertex3f(offsetExp + mWidth/3.2, min + 2, 0.6f); glVertex3f(offsetExp + mWidth/3.2, mHeight-MARGIN_ICONS - 5, 0.6f);
		glVertex3f(offsetExp + mWidth/3.2, mHeight-MARGIN_ICONS - 5, 0.6f); glVertex3f(offsetExp, mHeight-MARGIN_ICONS - 5, 0.6f);
		glVertex3f(offsetExp, mHeight-MARGIN_ICONS - 5, 0.6f); glVertex3f(offsetExp, min + 2, 0.6f);
	glEnd();
	// Draw Time left
	drawIcon(mWidth - mWidth/6, MARGIN_ICONS, mHeight-2*MARGIN_ICONS, mHeight-2*MARGIN_ICONS, texTime);

	// Draw Sub-screen border
	glPolygonMode(GL_FRONT_AND_BACK, 0);
	glLineWidth(LINE_WIDTH);
	glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	glBegin(GL_LINES);
		glVertex3f(0.0f, LINE_WIDTH/2, 0.1f); glVertex3f(mWidth, LINE_WIDTH/2, 0.1f);
		glVertex3f(mWidth-LINE_WIDTH/2, LINE_WIDTH/2, 0.1f); glVertex3f(mWidth-LINE_WIDTH/2, mHeight-LINE_WIDTH/2, 0.1f);
		glVertex3f(mWidth, mHeight-LINE_WIDTH/2, 0.1f); glVertex3f(0.0f, mHeight-LINE_WIDTH/2, 0.1f);
		glVertex3f(LINE_WIDTH/2, mHeight-LINE_WIDTH/2, 0.1f); glVertex3f(LINE_WIDTH/2, LINE_WIDTH/2, 0.1f);
	glEnd();

	// Print text
	int auxHeight = mHeight/2 - FONT_SIZE/1.5;
	int auxWidth = 70;
	drawText(min + 3*MARGIN_ICONS + mWidth/5, mHeight/2 + 1.25*MARGIN_ICONS, QColor(0, 0, 0, 255), QFont(FONT_FAMILY, FONT_SIZE, QFont::DemiBold), textLife);
	drawText(min + 3*MARGIN_ICONS + mWidth/5, 2*MARGIN_ICONS, QColor(0, 0, 0, 255), QFont(FONT_FAMILY, FONT_SIZE, QFont::DemiBold), textMana);
	drawText(2.25*mWidth/5+auxWidth, 2.5*MARGIN_ICONS, QColor(0, 0, 0, 255), QFont(FONT_FAMILY, FONT_SIZE, QFont::DemiBold), textExp);
	drawText(mWidth - mWidth/6 + auxWidth, auxHeight, QColor(0, 0, 0, 255), QFont(FONT_FAMILY, FONT_SIZE, QFont::DemiBold), textTime);

	glEnable(GL_LIGHTING);
}

/* Draw objects */
void SceneGUI::drawBackground()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texBackground);
	glColor4f(1.0f, 1.0f, 7.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
		glTexCoord2f(0.5f, 0.0f); glVertex3f(mWidth, 0.0f, 0.0f);
		glTexCoord2f(0.5f, 0.1f); glVertex3f(mWidth, mHeight, 0.0f);
		glTexCoord2f(0.0f, 0.1f); glVertex3f(0.0f, mHeight, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void SceneGUI::drawIcon(float posX, float posY, float sizeX, float sizeY, GLuint tex)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(posX, posY, 0.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(posX+sizeX, posY, 0.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(posX+sizeX, posY+sizeY, 0.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(posX, posY+sizeY, 0.5f);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

/* Textures */
void SceneGUI::createTextures(const char* filename, GLuint* textureId)
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageGL.width(), imageGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imageGL.bits());
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

/* Text */
void SceneGUI::drawText(float x, float y, QColor color, QFont font, QString *text)
{
	glColor4f(color.red()/255.0f, color.green()/255.0f, color.blue()/255.0f, color.alpha()/255.0f);
        renderText(x + LINE_WIDTH, mHeight - y - LINE_WIDTH, *text, font);
}

/* In-game functionalities: update of the game stats */
void SceneGUI::updateLife(int changeLife, int maxLife)
{
	this->maxLife = maxLife;
	if(changeLife > maxLife)
		mLife = maxLife;
	else if(changeLife < 0.0f)
		mLife = 0.0f;
	else 
		mLife = changeLife;
	textLife->clear();
	textLife->append(QString("%1 / %2").arg(mLife).arg(maxLife));
}

void SceneGUI::updateMana(int changeMana, int maxMana)
{
	this->maxMana = maxMana;
	if(changeMana > maxMana)
		mMana = maxMana;
	else if(changeMana < 0.0f)
		mMana = 0.0f;
	else 
		mMana = changeMana;
	textMana->clear();
	textMana->append(QString("%1 / %2").arg(mMana).arg(maxMana));
}

void SceneGUI::updateExp(int changeExp, int minExp, int maxExp, int level)
{
	this->minExp = minExp;
	this->maxExp = maxExp;
	this->mExp = changeExp;
	textExp->clear();
	textExp->append(QString("LEVEL: %1 - %2 exp").arg(level).arg(mExp));
}

void SceneGUI::updateTime(int changeTime)
{
	if(changeTime < 0)
		changeTime = 0;
	textTime->setNum(changeTime);
}

void SceneGUI::stopRefresh()
{
	mTime->stop();
}

