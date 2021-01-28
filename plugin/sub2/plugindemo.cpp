#include "plugindemo.h"
#include<QDebug>
#include <QPushButton>

QWidget *pluginDemo2::centerWidget()
{
    auto btn = new QPushButton("Two");
    return  btn;
}

QString pluginDemo2::name() {
    return "two";
}

QString pluginDemo2::information() {
    return "my name is two.";
}
