#ifndef PLUGINDEMO_H
#define PLUGINDEMO_H

#include "../plugindemoplugin.h"

class pluginDemo2 : public QObject, QtPluginDemoInterface
{
    Q_OBJECT
    Q_INTERFACES(QtPluginDemoInterface)
    Q_PLUGIN_METADATA(IID PluginDemoInterface_iid)

public:
    pluginDemo2(){};
    ~pluginDemo2(){};

    virtual QString name() {return "tow";};
    virtual QString information() {return "my name is tow.";};
    virtual QWidget *centerWidget() override;
};

#endif // PLUGINDEMO_H
