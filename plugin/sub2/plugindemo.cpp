#include "plugindemo.h"
#include<QDebug>
#include <QPushButton>

QWidget *pluginDemo2::centerWidget()
{
    auto btn = new QPushButton("Two");
    return  btn;
}
