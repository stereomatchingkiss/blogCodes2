#include "mainwindow.hpp"
#include <QApplication>

#include <QDebug>

#include <type_traits>

template<typename T>
void print_val(T val)
{
    static_assert(std::is_floating_point<T>::value, "T must be floating point");

    qDebug()<<val;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    print_val(3.0f);

    MainWindow w;
    w.show();

    return a.exec();
}
