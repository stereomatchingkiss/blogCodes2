#include "../incl/whatever.h"

#include <QObject>
#include <QDebug>

whatever::whatever() : QObject(0)
{}

void whatever::print() { qDebug()<<"lala"; }

//#include "moc_whatever.cpp"
