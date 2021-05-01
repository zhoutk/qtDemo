#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include "tetrisview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), MainScene(nullptr), index(1)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	MainScene = new QGraphicsScene(this);
	MainScene->setSceneRect(0, 0, 300/*this->geometry().width()*/, 600/*this->geometry().height()*/);
	TetrisView* view = new TetrisView(this->centralWidget());
	view->setScene(MainScene);
	//this->setCentralWidget(view);

    block = new CustomGraphTetrisBlock(QPoint(0,0), 6);
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

