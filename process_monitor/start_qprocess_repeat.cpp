#include "start_qprocess_repeat.hpp"

#include <QDebug>
#include <QProcess>

start_qprocess_repeat::start_qprocess_repeat(QProcess &process,
                                             const QString &program,
                                             const QStringList &arguments)
    : QObject(nullptr),
      arguments_(arguments),
      process_(&process),
      program_(program)
{
    process.start(program, arguments);
    connect(&process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(restart(QProcess::ProcessError error)));
    process.waitForStarted();
}

start_qprocess_repeat::~start_qprocess_repeat()
{

}

void start_qprocess_repeat::restart(QProcess::ProcessError error)
{
    qDebug()<<"process ["<<process_->program()<<"] error == "<<error;
    if(error == QProcess::FailedToStart){
        process_->start(program_, arguments_);
        process_->waitForStarted();
    }
}

