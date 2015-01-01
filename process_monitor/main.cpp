#include "utility.hpp"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    get_running_process();

    return a.exec();
}
