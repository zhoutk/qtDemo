#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QSpacerItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), MainScene(nullptr), index(1),gameView(nullptr),blockView(nullptr)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	QHBoxLayout* mainLayout = new QHBoxLayout;
	QWidget* centerView = new QWidget;
	MainScene = new QGraphicsScene(this);
	//MainScene->setSceneRect(0, 0, 300/*this->geometry().width()*/, 600/*this->geometry().height()*/);
	gameView = new TetrisView();
	gameView->setScene(MainScene);
	gameView->setSceneRect(0, 0, 300/*this->geometry().width()*/, 600/*this->geometry().height()*/);
	mainLayout->addWidget(gameView);
	blockView = new TetrisView();
	blockView->setScene(MainScene);
	blockView->setSceneRect(320, 0, 120, 120);
	blockView->setMaximumHeight(120);
	QVBoxLayout* rightLayout = new QVBoxLayout;
	rightLayout->addWidget(blockView);
	rightLayout->addItem(new QSpacerItem(20, 50, QSizePolicy::Fixed, QSizePolicy::Minimum));
	rightLayout->addWidget(ui->pushButton1);
	rightLayout->addWidget(ui->pushButton2);
	rightLayout->addWidget(ui->pushButton3);
	rightLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
	mainLayout->addLayout(rightLayout);
	centerView->setLayout(mainLayout);
	this->setCentralWidget(centerView);

	block = new CustomGraphTetrisBlock(QPoint(0, 0), 6);
	blockNext = new CustomGraphTetrisBlock(QPoint(320, 0), 3);
	MainScene->addItem(block);
	MainScene->addItem(blockNext);
}

MainWindow::~MainWindow()
{
    delete ui;
	delete MainScene;
}

void MainWindow::on_pushButton1_clicked()
{
	block->relocate(QPoint(0,0));
}

void MainWindow::on_pushButton2_clicked()
{
	block->relocate(QPoint(100, 100));
}

void MainWindow::on_pushButton3_clicked()
{
	QPointF centerPos = block->boundingRect().center();
	block->setTransformOriginPoint(centerPos.x(), centerPos.y());
	block->setRotation(90 * index++ % 360);		//* index++ % 360
}

