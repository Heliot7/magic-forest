#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_

#include <iostream>
#include <fstream>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QByteArray>

#include <GL/glew.h>

using namespace std;

class ShaderManager
{

public:

	ShaderManager(char* name = "unknown")
	{
		mProgram = glCreateProgram();
		mName = name;
	}

	~ShaderManager() {}

	// A Shader is loaded from its raw file (.vert or .frag) within solution resources
        void loadShader(char* filename)
	{
		// Check type of shader
		GLenum mShader;
		QString sFile = filename;
		if(sFile.toStdString().substr(sFile.length()-4) == "vert")
			mShader = glCreateShader(GL_VERTEX_SHADER);	
		else
			mShader = glCreateShader(GL_FRAGMENT_SHADER);	

		// Read the file
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return;
		QTextStream in(&file);
		QString shader;
		while (!in.atEnd())
			shader.append(in.readLine());
		QByteArray byteArray = shader.toUtf8();
		char* mShaderFile = byteArray.data();
		//char* mShaderFile = new char[byteArray.length()];
		//for(int pos = 0; pos < byteArray.length(); ++pos)
		//	mShaderFile[pos] = byteArray.data()[pos];

		// Copy shader source code from the file into "mShader"
		glShaderSource(mShader, 1, (const char**)&mShaderFile, NULL );
		
		// Compile shader
		glCompileShader(mShader);

		// Attach shader into a running program
		glAttachShader(mProgram, mShader);
		glLinkProgram(mProgram);
	}

	// Getters
        GLenum getProgram() { return mProgram; }
        char* getName(){ return mName; }

private:

	GLenum mProgram;
	char* mName;
};

#endif


