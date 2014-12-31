#include "start_qprocess_repeat.hpp"

#include <QDebug>
#include <QProcess>

start_qprocess_repeat::start_qprocess_repeat(QProcess &process,
                                             const QString &program,
                                             const QStringList &arguments)
    : QObject(nullptr),
      arguments_(arguments),
      process_(&process),
      program_(program),
      restart_limit_(3),
      restart_time_(0)
{
    connect(&process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(restart(QProcess::ProcessError)));

    process.start(program, arguments);
    process.waitForStarted();
}

start_qprocess_repeat::~start_qprocess_repeat()
{

}

start_qprocess_repeat&
start_qprocess_repeat::operator=(start_qprocess_repeat &&data) noexcept
{
    arguments_ = std::move(data.arguments_);
    process_ = data.process_;
    data.process_ = nullptr;
    program_ = std::move(data.program_);

    restart_limit_ = data.restart_limit_;
    restart_time_ = data.restart_time_;

    return *this;
}

start_qprocess_repeat::
start_qprocess_repeat(start_qprocess_repeat &&data) noexcept:
    arguments_{std::move(data.arguments_)},
    process_{data.process_},
    program_{std::move(data.program_)},
    restart_limit_{data.restart_limit_},
    restart_time_{data.restart_time_}
{

}

void start_qprocess_repeat::set_restart_limit(size_t restart_limit) noexcept
{
    restart_limit_ = restart_limit;
}

/**
 * @brief restart the process if\n
 * 1 : error == QProcess::FailedToStart\n
 * 2 : restart time < restart limit
 * @param error error codes
 */
void start_qprocess_repeat::restart(QProcess::ProcessError error) noexcept
{
    qDebug()<<"process ["<<process_->program()<<"] error == "<<error;
    if(restart_time_ < restart_limit_){
        if(error == QProcess::FailedToStart){
            process_->start(program_, arguments_);
            process_->waitForStarted();
        }
        ++restart_time_;
    }
}

