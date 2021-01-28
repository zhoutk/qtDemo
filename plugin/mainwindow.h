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

    int MainWindow::loadPlugins()
    {
        int count  = 0;
        QDir pluginsDir = QDir(QCoreApplication::applicationDirPath());
        if(!pluginsDir.cd("plugins")) return -1;
        foreach (QString fileName, pluginsDir.entryList(QDir::Files))
        {
            QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = pluginLoader.instance();
            if(plugin)
            {
                auto centerInterface = qobject_cast<QtPluginDemoInterface*>(plugin);
                if(centerInterface)
                {
                    ++count;
                    //加载插件后生成menu
                    populateMenus(plugin,centerInterface);
                }
            }
        }
        return count;
    };

    void MainWindow::populateMenus(QObject * pluginInterface,QtPluginDemoInterface*i )
    {
        static auto menu = menuBar()->addMenu("widgets");
        auto act  = new QAction(i->name(),pluginInterface);
        //单击menu调用插件
        connect(act,&QAction::triggered,this,&MainWindow::slt_WidgetActionTriggered);
        menu->addAction(act);
    }

    void MainWindow::slt_WidgetActionTriggered()
    {
        auto centerWidget = qobject_cast<QtPluginDemoInterface*>(sender()->parent())->centerWidget();
        setCentralWidget(centerWidget);
    }

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
