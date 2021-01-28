#include "plugindemo.h"
#include<QDebug>
#include <QPushButton>

QWidget *pluginDemo::centerWidget()
{
    auto btn = new QPushButton("One");
    return  btn;
}

QString pluginDemo::name() {
    return "one";
}

QString pluginDemo::information() {
    return "my name is one.";
}
