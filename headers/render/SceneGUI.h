#ifndef _SCENEGUI_H_
#define _SCENEGUI_H_

#include "../headers/render/ShaderManager.h"
#include "../headers/render/Geometry.h"

#include <iostream>
#include <QGLWidget>
#include <QKeyEvent>
#include <QTimer>
#include <vector>

using namespace std;

class SceneGUI : public QGLWidget
{
	Q_OBJECT

public:

	SceneGUI(QWidget* parent = 0, int width = 800, int height = 150);
	~SceneGUI();

	void updateLife(int changeLife, int maxLife);
	void updateMana(int changeMana, int maxMana);
	void updateExp(int changeExp, int minExp, int maxExp, int level);
	void updateTime(int changeTime);

	void stopRefresh();

protected:

	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();

private:
	
	// for UpdateGL()
	QTimer *mTime;

	// Camera spec
	Point camPos, vrp, vuv;
	float mWidth, mHeight;

	// Scene spec
	float Znear;
	float Zfar;
	float fov;

	// Vetex and Fragment Shader Manager
	ShaderManager *shaderManager; 

	// Drawn objects
	void drawBackground();
	void drawIcon(float posX, float posY, float sizeX, float sizeY, GLuint tex);

	// Textures
	void createTextures(const char* filename, GLuint* textureId);
	GLuint texBackground;
	GLuint texHeart;
	GLuint texMana;
	GLuint texExp;
	GLuint texTime;

	// Text
	void drawText(float x, float y, QColor color, QFont font, QString *text);
	QString *textLife, *textMana, *textExp, *textTime;

	// In-game variables
	float mLife, mMana, mExp;
	float maxLife, maxMana, minExp, maxExp;

signals:

public slots:

protected slots:

private slots:

};

#endif
