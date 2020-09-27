#ifndef _POPUPINIT_H_
#define _POPUPINIT_H_

#include "ui_PopUpInit.h"

class PopUpInit : public QDialog, private Ui::PopUpInit
{
	Q_OBJECT

public:

	PopUpInit(QWidget* parent = 0);
	~PopUpInit();

	void updateText(int);

protected:

private:
	
signals:

public slots:

protected slots:

private slots:

};

#endif
