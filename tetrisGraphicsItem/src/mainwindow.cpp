#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QSpacerItem>

MainWindow* MainWindow::APP = nullptr;

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
    gameView->setSceneRect(0, 0, 300/*this->geometry().width()*/, 600/*this->geometry().height()*/);
    gameView->setMaximumWidth(301);
    gameView->setMaximumHeight(601);
	mainLayout->addWidget(gameView);
	blockView = new TetrisView();
	blockView->setScene(MainScene);
	blockView->setSceneRect(330, 0, 120, 120);
    blockView->setMaximumWidth(121);
    blockView->setMaximumHeight(121);
	QVBoxLayout* rightLayout = new QVBoxLayout;
	rightLayout->addWidget(blockView);
	rightLayout->addItem(new QSpacerItem(20, 50, QSizePolicy::Fixed, QSizePolicy::Minimum));
	rightLayout->addWidget(ui->pushButton1);
	rightLayout->addWidget(ui->pushButton2);
	rightLayout->addWidget(ui->pushButton3);
	rightLayout->addWidget(ui->pushButton4);
	rightLayout->addWidget(ui->pushButton5);
	rightLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
	mainLayout->addLayout(rightLayout);
	centerView->setLayout(mainLayout);
	this->setCentralWidget(centerView);
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

void MainWindow::on_pushButton1_clicked()
{
	if (!game)
		game = new Game();
	game->start();
}

void MainWindow::on_pushButton2_clicked()
{
	//block->moveDown();
}

void MainWindow::on_pushButton3_clicked()
{
	//block->moveLeft();
}

void MainWindow::on_pushButton4_clicked()
{
	//block->moveRight();
}

void MainWindow::on_pushButton5_clicked()
{
	//blockNext->rotate();
	//block->rotate();
}

