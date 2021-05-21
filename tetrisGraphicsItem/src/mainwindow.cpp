#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QSpacerItem>
#include "customGraphItem/customgraphtetristext.h"

extern int isRunning;
MainWindow* MainWindow::APP = nullptr;
DbBase* db;

MainWindow* MainWindow::GetApp()
{
	if (MainWindow::APP == nullptr)
		MainWindow::APP = new MainWindow();
	return MainWindow::APP;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), MainScene(nullptr), gameView(nullptr),blockView(nullptr), ui(new Ui::MainWindow), game(nullptr)
{
    ui->setupUi(this);
	QHBoxLayout* mainLayout = new QHBoxLayout;
	QWidget* centerView = new QWidget;
	MainScene = new QGraphicsScene(this);
	//MainScene->setSceneRect(0, 0, 300/*this->geometry().width()*/, 600/*this->geometry().height()*/);
	gameView = new TetrisView();
	gameView->setScene(MainScene);
    gameView->setSceneRect(0, 0, 300/*this->geometry().width()*/, 601/*this->geometry().height()*/);
    gameView->setMaximumWidth(301);
    gameView->setMaximumHeight(601);
	QVBoxLayout* leftLayout = new QVBoxLayout;
	leftLayout->addWidget(gameView);
	leftLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
	blockView = new TetrisView();
	blockView->setScene(MainScene);
	blockView->setSceneRect(330, 0, 120, 120);
    blockView->setMaximumWidth(121);
    blockView->setMaximumHeight(121);
	QVBoxLayout* rightLayout = new QVBoxLayout;
	rightLayout->addWidget(blockView);
	rightLayout->addItem(new QSpacerItem(20, 50, QSizePolicy::Fixed, QSizePolicy::Minimum));
	rightLayout->addWidget(ui->labelSpeed);
	rightLayout->addWidget(ui->lineEditSpeed);
	rightLayout->addWidget(ui->labelLevels);
	rightLayout->addWidget(ui->lineEditLevels);
	rightLayout->addWidget(ui->labelScores);
	rightLayout->addWidget(ui->lineEditScores);
	rightLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
	rightLayout->addWidget(ui->checkBoxAutoPlay);
	rightLayout->addWidget(ui->pushButtonStart);
	rightLayout->addWidget(ui->comboBoxRecords);
	rightLayout->addWidget(ui->pushButtonPlayback);
	rightLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
	mainLayout->addLayout(leftLayout);
	mainLayout->addLayout(rightLayout);
	centerView->setLayout(mainLayout);
	this->setCentralWidget(centerView);

	initDatabase();
}

MainWindow::~MainWindow()
{
    delete ui;
	delete MainScene;
	delete game;
}

QGraphicsScene* MainWindow::GetScene()
{
	return MainScene;
}

Game* MainWindow::GetGame()
{
	return game;
}

void MainWindow::updateScore()
{
	ui->lineEditSpeed->setText(QString::number(game->getSpeed()));
	ui->lineEditLevels->setText(QString::number(game->getLevels()));
	ui->lineEditScores->setText(QString::number(game->getScores()));
}

void MainWindow::initDatabase()
{
	QString curDir = QDir::currentPath();
	db = new DbBase(curDir + "/log.db");

	Qjson ret = db->execSql(QString::fromLocal8Bit(
			"CREATE TABLE IF NOT EXISTS 'gameRecords' ('id' text NOT NULL PRIMARY KEY,\
			'gameId' text NOT NULL, 'blockType' integer NOT NULL,\
			'rotateNumber' integer DEFAULT 0,'LocateX' integer NOT NULL,'LocateY' integer NOT NULL,\
			'stepId' integer NOT NULL, 'create_time' text DEFAULT (datetime('now','localtime')));"));
	db->execSql(QString::fromLocal8Bit(
		"CREATE TABLE IF NOT EXISTS 'gameLists' ('id' text NOT NULL PRIMARY KEY,\
			'speed' integer DEFAULT 1,'levels' integer DEFAULT 0,'scores' integer DEFAULT 0,\
			'steps' integer DEFAULT 0,'create_time' text DEFAULT (datetime('now','localtime')));"));
	//qDebug() << ret.GetJsonString();
}

void MainWindow::slotGameOver()
{
	ui->checkBoxAutoPlay->setEnabled(true);
	ui->pushButtonStart->setText("Start");
	ui->pushButtonStart->setEnabled(true);
	ui->pushButtonPlayback->setEnabled(true);
	MainWindow::GetApp()->GetScene()->addItem(new CustomGraphTetrisText(QPoint(1, 8)));
}

void MainWindow::slotUpdateScore()
{
	this->updateScore();
}

void MainWindow::on_pushButtonStart_clicked()
{
	if (!game) {
		game = new Game();
		connect(game, &Game::signalGameOver, this, &MainWindow::slotGameOver);
		connect(game, &Game::signalUpdateScore, this, &MainWindow::slotUpdateScore);
	}
	if (isRunning == 0) {
		ui->pushButtonStart->setText("Pause");
		ui->pushButtonPlayback->setEnabled(false);
		foreach(auto al, MainWindow::GetApp()->GetScene()->items())
		{
			if (((CustomGraphBase*)al)->getBlockType() > 0) {
				MainWindow::GetApp()->GetScene()->removeItem(al);
				delete al;
			}
		}
		game->start();
		this->updateScore();
	}
	else if(isRunning == 1)
	{
		ui->pushButtonStart->setText("Resume");
		game->pause();
	}
	else {
		ui->pushButtonStart->setText("Pause");
		game->resume();
	}
	gameView->setFocus();

}

void MainWindow::on_pushButtonPlayback_clicked()
{
	ui->checkBoxAutoPlay->setEnabled(false);
	ui->pushButtonStart->setEnabled(false);
	ui->pushButtonPlayback->setEnabled(false);

	if (!game) {
		game = new Game();
		connect(game, &Game::signalGameOver, this, &MainWindow::slotGameOver);
		connect(game, &Game::signalUpdateScore, this, &MainWindow::slotUpdateScore);
	}

	QString lastID;
	Qjson psid;
	psid.AddValueBase("size", "1");
	switch (ui->comboBoxRecords->currentIndex())
	{
	case 1:
		psid.AddValueBase("sort", "scores desc");
		psid.AddValueBase("page", "1");
		break;
	case 2:
		psid.AddValueBase("sort", "scores desc");
		psid.AddValueBase("page", "2");
		break;
	case 3:
		psid.AddValueBase("sort", "scores desc");
		psid.AddValueBase("page", "3");
		break;
	default:
		psid.AddValueBase("sort", "create_time desc");
		psid.AddValueBase("page", "1");
		break;
	}
	Qjson idObj = db->select("gameLists", psid);
	qDebug() << idObj.GetJsonString();
	if (idObj["code"] == "200") {
		lastID = idObj.GetObjectsArrayByKey("data")[0]["id"];
	}
	if (lastID.size() > 10)
	{
		Qjson ps;
		ps.AddValueBase("gameId", lastID);
		ps.AddValueBase("sort", "stepId");
		Qjson rs = db->select("gameRecords", ps);

		foreach(auto al, MainWindow::GetApp()->GetScene()->items())
		{
			if (((CustomGraphBase*)al)->getBlockType() > 0) {
				MainWindow::GetApp()->GetScene()->removeItem(al);
				delete al;
			}
		}

		game->playback(rs.GetObjectsArrayByKey("data"));
		this->updateScore();
	} 
	else {
		ui->checkBoxAutoPlay->setEnabled(true);
		ui->pushButtonStart->setEnabled(true);
		ui->pushButtonPlayback->setEnabled(true);
	}
}

