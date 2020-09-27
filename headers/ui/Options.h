#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "ui_Options.h"

class Options : public QDialog, private Ui::Options
{
	Q_OBJECT

public:

	Options(QDialog* parent = 0);
	~Options();

	void setSound(bool isSound);

protected:

private:

signals:

	void goBackMainMenuView(int);
	void soundChanged(bool);

public slots:

protected slots:
	
	void on_buttonBack_clicked();
	void on_buttonSound_clicked();
	void on_easy_clicked();
	void on_medium_clicked();
	void on_hard_clicked();
	void on_shadows_clicked();
	void on_noShadows_clicked();

private slots:

};

#endif
