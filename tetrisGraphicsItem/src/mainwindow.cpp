#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include "tetrisview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), MainScene(nullptr)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	MainScene = new QGraphicsScene(this);
	MainScene->setSceneRect(0, 0, 440/*this->geometry().width()*/, this->geometry().height());
	TetrisView* view = new TetrisView(this->centralWidget());
	view->setScene(MainScene);

	//this->setCentralWidget(view);

    block = new CustomGraphTetrisBlock(QPoint(100,100), 5);
	MainScene->addItem(block);
}

MainWindow::~MainWindow()
{
    delete ui;
	delete MainScene;
}

void MainWindow::on_pushButton1_clicked()
{
	block->relocate(QPoint(0,0));
	MainScene->update();
}

void MainWindow::on_pushButton2_clicked()
{
	block->relocate(QPoint(100, 100));
	MainScene->update();
}

