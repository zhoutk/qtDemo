#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->loadPlugins();
}

MainWindow::~MainWindow()
{
    delete ui;
}

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
}

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
    QtPluginDemoInterface * plg = qobject_cast<QtPluginDemoInterface*>(sender()->parent());
    auto centerWidget = plg->centerWidget();
    (qobject_cast<QPushButton*>(centerWidget))->setText(plg->information());
    setCentralWidget(centerWidget);
}

