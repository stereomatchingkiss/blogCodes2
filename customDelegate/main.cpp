#include "drag_drop.hpp"
#include "mainwindow.hpp"
#include "range_delegate.hpp"

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
    }*/

    /*QStandardItemModel model;
    auto int_value = new QStandardItem;
    int_value->setData(1000, Qt::DisplayRole);
    model.setItem(0, 0, int_value);

    QTableView view;
    auto range = new range_delegate({0, 100});
    view.setModel(&model);
    view.setItemDelegateForColumn(0, range);
    model.setData(model.index(0, 0), 1000);
    view.show();

    QVariant var;
    var = QColor(Qt::red);
    qDebug()<<var.value<QColor>();*/

    return a.exec();
}
