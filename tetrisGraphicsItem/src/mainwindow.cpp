#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include "customGraphItem/customgraphtetrisblock.h"
#include "tetrisview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), MainScene(nullptr)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	MainScene = new QGraphicsScene(this);
	MainScene->setSceneRect(0, 0, this->geometry().width(), this->geometry().height());
	TetrisView* view = new TetrisView;
	view->setScene(MainScene);

	this->setCentralWidget(view);

    CustomGraphTetrisBlock * block = new CustomGraphTetrisBlock(QPoint(100,100), 0);
	MainScene->addItem(block);
}

MainWindow::~MainWindow()
{
    delete ui;
	delete MainScene;
}

