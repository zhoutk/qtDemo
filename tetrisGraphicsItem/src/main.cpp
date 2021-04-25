#include "mainwindow.h"
#include "./common/snowflake.h"
#include <QApplication>

SnowFlake snowFlakeId;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
