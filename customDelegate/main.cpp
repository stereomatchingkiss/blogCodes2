#include "drag_drop.hpp"
#include "mainwindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringList>
#include <QTextStream>

QString read_from_file()
{
    QFile file("test.vms");
    QTextStream stream(&file);
    if(file.open(QIODevice::Text | QIODevice::ReadOnly)){
         return stream.readAll();
    }

    return "";
}

void split_string(QString const &str)
{
    auto const Splitted = str.split("\t");
    qDebug()<<"size = "<<Splitted.size();
    for(auto const &data : Splitted){
        qDebug()<<data;
    }
}

void write_to_file()
{
    QFile file("test.vms");
    QTextStream stream(&file);
    if(file.open(QIODevice::Text | QIODevice::WriteOnly)){
        stream<<"wahaha\thazuki\tdoremi";
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    drag_drop dd;
    dd.show();

    //split_string(read_from_file());

    return a.exec();
}
