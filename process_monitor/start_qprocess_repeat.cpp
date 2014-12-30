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
            this, SLOT(restart(QProcess::ProcessError error)));

    process.start(program, arguments);
    process.waitForStarted();
}

start_qprocess_repeat::~start_qprocess_repeat()
{

}

void start_qprocess_repeat::set_restart_limit(size_t restart_limit)
{
    restart_limit_ = restart_limit;
}

void start_qprocess_repeat::restart(QProcess::ProcessError error)
{
    qDebug()<<"process ["<<process_->program()<<"] error == "<<error;
    if(restart_time_ < restart_limit_){
        if(error == QProcess::FailedToStart){
            process_->start(program_, arguments_);
            process_->waitForStarted();
        }
    }
    ++restart_time_;
}

