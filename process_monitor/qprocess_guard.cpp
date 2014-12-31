#include "qprocess_guard.hpp"
#include "kill_qprocess.hpp"

qprocess_guard::qprocess_guard(QProcess *process,
                               QObject *parent) :
    QObject(parent),
    enable_restart_(true),
    error_handle_{kill_qprocess},
    finish_time_{3000},
    process_{process}
{
    connect(process_, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(restart(QProcess::ProcessError)));
}

qprocess_guard::~qprocess_guard()
{
    if(process_ && process_->state() == QProcess::Running){
        if(!process_->waitForFinished(finish_time_)){
            error_handle_(*process_);
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
void qprocess_guard::set_error_handle(std::function<void(QProcess&)> func)
{
    error_handle_ = std::move(func);
}

void qprocess_guard::set_finish_time(int msecs) noexcept
{
    finish_time_ = msecs;
}

void qprocess_guard::set_qprocess(QProcess *process) noexcept
{
    process_ = process;
}

void qprocess_guard::restart(QProcess::ProcessError error) noexcept
{
    if(process_ && error == QProcess::Crashed && enable_restart_){
        process_->start(process_->program(), process_->arguments());
    }
}

