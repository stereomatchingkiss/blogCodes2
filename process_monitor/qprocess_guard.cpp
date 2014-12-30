#include "qprocess_guard.hpp"
#include "kill_qprocess.hpp"

#include <QProcess>

qprocess_guard::qprocess_guard(QProcess *process) :
    error_handle_(kill_qprocess),
    finish_time_(3000),
    process_(process)
{

}

qprocess_guard::~qprocess_guard()
{
    if(process_ && process_->state() == QProcess::Running){
        if(!process_->waitForFinished(finish_time_)){
            error_handle_(*process_);
        }
    }
}

/**
 * @brief setup the error handle
 * @param func the functor try to handle the error
 */
void qprocess_guard::set_error_handle(std::function<void (QProcess &)> func)
{
    error_handle_ = func;
}

void qprocess_guard::set_finish_time(int msecs)
{
    finish_time_ = msecs;
}

void qprocess_guard::set_qprocess(QProcess *process)
{
    process_ = process;
}

