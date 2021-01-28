#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCoreApplication>
#include <QDir>
#include <QMainWindow>
#include <QPluginLoader>
#include <QString>
#include <QMenuBar>
#include <QPushButton>
#include "plugindemoplugin.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int loadPlugins();
    void populateMenus(QObject * pluginInterface,QtPluginDemoInterface*i );
    void slt_WidgetActionTriggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
