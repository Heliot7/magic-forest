#ifndef _INFO_H_
#define _INFO_H_

#include "ui_Info.h"

class Info : public QDialog, private Ui::Info
{
	Q_OBJECT

public:

	Info(QDialog* parent = 0);
	~Info();

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

private slots:

};

#endif
