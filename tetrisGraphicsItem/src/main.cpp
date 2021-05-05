#include "mainwindow.h"
#include "./common/snowflake.h"
#include <QApplication>

SnowFlake snowFlakeId;

bool isRunning = false;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow * w = MainWindow::GetApp();
    w->show();
	return a.exec();
}
