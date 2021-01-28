#ifndef PLUGINDEMOPLUGIN_H
#define PLUGINDEMOPLUGIN_H
#include<QWidget>
#include<QString>

class QtPluginDemoInterface
{
public:
    virtual ~QtPluginDemoInterface() {}
    virtual QString name() = 0;
    virtual QString information() = 0;
    //返回一个Widget设置到centerwidget中进行显示
    virtual QWidget *centerWidget() = 0;
};
//step 2 声明接口
#define PluginDemoInterface_iid "com.Interface.MainInterface"
Q_DECLARE_INTERFACE(QtPluginDemoInterface, PluginDemoInterface_iid)
#endif // PLUGINDEMOPLUGIN_H
