#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <iostream>
#include <QString>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTextStream>

class ScoreManager
{
public:

	static void readFile(char* filePath, vector<QStringList> *lines)
	{
		// Get scores and add into the list
		QString path = QDir::currentPath().append(filePath);
		if(getenv("windir") != NULL)
			path.replace("/", "\\");
		QFile file(path);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			cout << "Error opening the file" << endl;
		QTextStream in(&file);
		QString line;		
		while (!in.atEnd())
		{
			line = in.readLine();
			QStringList listElems = line.split(QString(","));
			if(listElems.size() == 4)
				lines->push_back(listElems);
		}
		file.close();
	}

	static void writeScores(int level, int exp, int enemies, QString diff)
	{
		QString path = QDir::currentPath().append("/resources/database/highscores.txt");
		if(getenv("windir") != NULL)
			path.replace("/", "\\");
		QFile file(path);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
			cout << "Error opening the file" << endl;
		else
		{
			QTextStream store(&file);
			QString line = QString("%1,%2,%3,%4")
				.arg( level )
				.arg( exp )
				.arg( enemies )
				.arg( diff );
			store << endl << line.toAscii().data();
			file.close();
		}
		file.close();
	}
};

#endif
