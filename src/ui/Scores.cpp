#include "headers/ui/MainWindow.h"
#include "headers/ui/Scores.h"
#include "headers/logic/GameInfo.h"
#include "headers/data/ScoreManager.h"

#include <QFont>
#include <QFontDatabase>
#include <QTableWidgetItem>

// Constructor
Scores::Scores(QDialog* parent) : QDialog(parent)
{
	QFontDatabase::addApplicationFont(":/fonts/russianquality.ttf");
	QFontDatabase::addApplicationFont(":/fonts/vivala.ttf");
	QFontDatabase::addApplicationFont(":/fonts/buschgardenz.ttf");
	QFontDatabase::addApplicationFont(":/fonts/neverwinter.ttf");
	setupUi(this);
	updateTable();
}

// Destroyer
Scores::~Scores()
{
}

void Scores::setSound(bool isSound)
{
	buttonSound->setChecked(isSound);
}

/* SLOTS */
// Closes the dialog
void Scores::on_buttonBack_clicked()
{
	emit goBackMainMenuView(UI_MAIN);
}

void Scores::on_buttonSound_clicked()
{
	GameInfo::Instance()->setSound(buttonSound->isChecked());
	emit soundChanged(buttonSound->isChecked());
}
void Scores::updateTable()
{
	vector<QStringList> *lines = new vector<QStringList>();
	ScoreManager::readFile("/resources/database/highscores.txt", lines);

	vector<QTableWidgetItem*> *items = new vector<QTableWidgetItem*>();
	QTableWidgetItem *prototype = new QTableWidgetItem();
	prototype->setTextAlignment(Qt::AlignCenter);
	int row = 0;
	for(int n = 0; n < lines->size(); ++n)
	{
		QStringList listElems = lines->at(n);
		for(int i = 0; i < listElems.size(); ++i)
		{
			QTableWidgetItem *item =  prototype->clone();
			item->setText(listElems.at(i));
			items->push_back(item);
		}
		row++;
	}

	// Dimension of the table
	tableWidget->setRowCount(row);
	tableWidget->setColumnCount(4);
	
	// Header of the table
	QStringList header;
	header.push_back("Level");
	header.push_back("Experience");
	header.push_back("Enemies");
	header.push_back("Difficulty");
	for(int i = 0; i < 4; ++i)
	{
		QTableWidgetItem *item =  prototype->clone();
		item->setText(header.at(i));
		tableWidget->setHorizontalHeaderItem(i, item);
	}

	tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	for(int i = 0; i < row; ++i)
	{
		for(int j = 0; j < 4; ++j)
			tableWidget->setItem(i, j, items->at(i*4+j));

		QTableWidgetItem *item =  prototype->clone();
		item->setText(QString().setNum(i+1));
		tableWidget->setVerticalHeaderItem(i, item);
	}

	tableWidget->sortByColumn(0);
	tableWidget->viewport()->setFocusPolicy(Qt::NoFocus);
	tableWidget->setContentsMargins(25, 25, 25, 25);
}