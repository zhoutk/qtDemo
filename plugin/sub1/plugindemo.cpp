#include "plugindemo.h"
#include<QDebug>
#include <QPushButton>

QWidget *pluginDemo::centerWidget()
{
    auto btn = new QPushButton("One");
    return  btn;
}
