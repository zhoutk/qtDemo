#ifndef PLUGINDEMO_H
#define PLUGINDEMO_H

#include "../plugindemoplugin.h"

class pluginDemo : public QObject, QtPluginDemoInterface
{
    Q_OBJECT
    Q_INTERFACES(QtPluginDemoInterface)
    Q_PLUGIN_METADATA(IID PluginDemoInterface_iid)

public:
    pluginDemo(){};
    ~pluginDemo(){};

    virtual QString name() override;
    virtual QString information() override;
    virtual QWidget *centerWidget() override;
};

#endif // PLUGINDEMO_H
