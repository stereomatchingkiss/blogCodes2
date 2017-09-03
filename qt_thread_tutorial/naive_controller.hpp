#ifndef NAIVE_CONTROLLER_HPP
#define NAIVE_CONTROLLER_HPP

#include <QObject>
#include <QThread>

class naive_worker;

class naive_controller : public QObject
{
    Q_OBJECT
public:
    explicit naive_controller(QObject *parent = nullptr);
    ~naive_controller();

    void print_working_thread_by_normal_call();

signals:
    void print_working_thread_by_signal_and_slot();

private:
    QThread thread_;
    naive_worker *worker_;
};


#endif // NAIVE_WORKER_THREAD_HPP
