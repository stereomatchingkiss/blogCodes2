#include "drag_drop.hpp"
#include "mainwindow.hpp"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QRegularExpression>
#include <QStandardItemModel>
#include <QStringList>
#include <QTextStream>
#include <QSaveFile>
#include <QTableview>

#include <iostream>
#include <set>

struct pair_id
{
    pair_id(int a = 0, int b = 0) : a_(a), b_(b) {}

    int a_;
    int b_;
};

inline
bool operator<(pair_id const &lhs, pair_id const &rhs)
{
    return lhs.a_ < rhs.a_ || lhs.b_ < rhs.b_;
}

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
    MainWindow w;
    w.show();

    //drag_drop dd;
    //dd.show();

    /*auto file = QFileDialog::getSaveFileName(0, "waha", QDir::currentPath() + "/ppp");
    QSaveFile safe(file);
    if(safe.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream stream(&safe);
        stream << "wahahaha";
        safe.commit();
    }else{
        qDebug()<<"cannot open";
    }*/

    //split_string(read_from_file());
    /*std::set<pair_id> test_set;
    test_set.emplace(0, 0);
    test_set.emplace(0, 1);
    test_set.emplace(0, 2);
    test_set.emplace(0, 3);
    test_set.emplace(-1, 3);
    test_set.emplace(-2, 3);
    test_set.emplace(-1, 2);
    test_set.emplace(-1, 4);
    for(auto const &data : test_set){
        std::cout<<data.a_<<", "<<data.b_<<std::endl;
    }

    QStandardItemModel model;
    model.setItem(0, 0, new QStandardItem{"wahaha1"});
    model.setItem(0, 1, new QStandardItem{"wahaha2"});
    model.setItem(1, 0, new QStandardItem{"wahaha3"});
    model.setItem(1, 1, new QStandardItem{"wahaha4"});
    QTableView view;
    view.setModel(&model);
    view.show();*/

    return a.exec();
}
