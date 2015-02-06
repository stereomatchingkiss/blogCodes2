#include "../incl/whatever.h"
#include "../incl/mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QString>
#include <QObject>

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;
    qDebug()<<"ttt";

    whatever what;
    what.print();

    try{
        QApplication a(argc, argv);
        //QLabel label;
        //label.show();
        MainWindow win;
        win.show();

        return a.exec();
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
        return -1;
    }

    return 0;
}

