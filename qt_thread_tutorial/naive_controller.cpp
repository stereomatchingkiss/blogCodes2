#include "naive_controller.hpp"

#include "naive_worker.hpp"

#include <QDebug>

naive_controller::naive_controller(QObject *parent) :
    QObject(parent),
    worker_(new naive_worker)
{
    worker_->moveToThread(&thread_);

    connect(&thread_, &QThread::finished, worker_, &QObject::deleteLater);
    connect(this, &naive_controller::print_working_thread_by_signal_and_slot,
            worker_, &naive_worker::print_working_thread);
    thread_.start();
}

naive_controller::~naive_controller()
{
    thread_.quit();
    thread_.wait();
}

void naive_controller::print_working_thread_by_normal_call()
{
    worker_->print_working_thread();
}
