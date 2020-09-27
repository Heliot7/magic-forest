#ifndef _SCORES_H_
#define _SCORES_H_

#include "ui_Scores.h"

class Scores : public QDialog, private Ui::Scores
{
	Q_OBJECT

public:

	Scores(QDialog* parent = 0);
	~Scores();

	void setSound(bool isSound);

protected:

private:
	
signals:

		void goBackMainMenuView(int);
		void soundChanged(bool);

public slots:

	// Reads a text file with the highscores
	void updateTable();

protected slots:
	
	void on_buttonBack_clicked();
	void on_buttonSound_clicked();

private slots:

};

#endif
