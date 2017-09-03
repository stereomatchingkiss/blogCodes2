#ifndef NAIVE_WORKER_HPP
#define NAIVE_WORKER_HPP

#include <QObject>

class naive_worker : public QObject
{
    Q_OBJECT
public:
    explicit naive_worker(QObject *obj = nullptr);

    void print_working_thread();
};

#endif // NAIVE_WORKER_HPP
