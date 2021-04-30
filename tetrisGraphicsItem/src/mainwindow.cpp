#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customGraphItem/customgraphtetrisblock.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CustomGraphTetrisBlock * block = new CustomGraphTetrisBlock(QPoint(1,1));
}

MainWindow::~MainWindow()
{
    delete ui;
}

