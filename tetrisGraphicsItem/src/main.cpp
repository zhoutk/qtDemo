#include "mainwindow.h"
#include "./common/snowflake.h"
#include <QApplication>

SnowFlake snowFlakeId;

int isRunning = 0;      //0-stop; 1-run; 2-relay; 5-pause;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow * w = MainWindow::GetApp();
    w->show();
	return a.exec();
}
