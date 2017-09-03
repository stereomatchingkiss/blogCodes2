#include "naive_worker.hpp"

#include <QDebug>
#include <QThread>

naive_worker::naive_worker(QObject *obj) : QObject(obj)
{
}

void naive_worker::print_working_thread()
{
    qDebug()<<QThread::currentThread();
}
