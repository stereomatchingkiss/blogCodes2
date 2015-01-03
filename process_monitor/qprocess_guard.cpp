#include "qprocess_guard.hpp"
#include "utility.hpp"

#include <QDebug>

qprocess_guard::qprocess_guard(QProcess *process,
                               QString const &program,
                               QStringList const &arguments,
                               QObject *parent) :
    QObject(parent),
    close_handle_{kill_qprocess},
    enable_restart_(true),
    finish_time_{3000},
    process_{process},
    restart_handle_{restart_qprocess}
{    
    connect(process_, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(restart(QProcess::ProcessError)));
    process_->start(program, arguments);
}

qprocess_guard::~qprocess_guard()
{
    disconnect(process_, SIGNAL(error(QProcess::ProcessError)),
               this, SLOT(restart(QProcess::ProcessError)));
    if(process_ && process_->state() == QProcess::Running){
        if(!process_->waitForFinished(finish_time_)){
            close_handle_(std::ref(*process_));
        }
    }
}

void qprocess_guard::enable_restart(bool value) noexcept
{
    enable_restart_ = value;
}

/**
 * @brief setup the error handle
 * @param func the functor try to handle the error
 */
void qprocess_guard::set_close_handle(std::function<void(QProcess&)> func)
{
    close_handle_ = std::move(func);
}

void qprocess_guard::set_finish_time(int msecs) noexcept
{
    finish_time_ = msecs;
}

void qprocess_guard::set_restart_handle(std::function<void (QProcess &, QProcess::ProcessError)> func)
{
    restart_handle_ = std::move(func);
}

void qprocess_guard::restart(QProcess::ProcessError error) noexcept
{
    if(process_ && enable_restart_){
        restart_handle_(std::ref(*process_), error);
    }
}

