#include "kill_qprocess.hpp"

#include <QDebug>
#include <QProcess>

void kill_qprocess(QProcess &process) noexcept
{
    qDebug()<<"process id == "<<process.processId();
    qDebug()<<"process error == "<<process.error();
    process.kill();
}

