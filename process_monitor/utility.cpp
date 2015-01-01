#include "utility.hpp"

#include <QDebug>
#include <QProcess>
#include <QRegularExpression>

#include <algorithm>
#include <iterator>

/**
 * @brief get the running process paths/names
 *
 *
 * By now this function only support windows platform\n
 * which support power shell
 * @return running process paths/names
 */
QStringList get_running_process()
{
    QProcess tasklist;
    tasklist.start(
                "PowerShell",
                QStringList() << "Get-Process | Format-List Path"
                );
    tasklist.waitForFinished();
    QString const Output = tasklist.readAllStandardOutput();
    auto result = Output.split(QRegularExpression("\r\n|\n\r|\r|\n"));
    result.removeDuplicates();
    for(auto &data : result){
        data.replace("Path : ", "");
    }

    auto it = std::remove_if(std::begin(result), std::end(result), [](QString const &a)
    {
        return a.isEmpty();
    });
    result.erase(it, std::end(result));

    return result;
}

void kill_qprocess(QProcess &process) noexcept
{
    qDebug()<<"process id == "<<process.processId();
    qDebug()<<"process error == "<<process.error();
    process.kill();
}


void restart_qprocess(QProcess &process, QProcess::ProcessError error) noexcept
{
    if(error == QProcess::Crashed){
        process.start(process.program(), process.arguments());
    }else if(error == QProcess::FailedToStart){
        qDebug()<<process.program()<<" failed to start";
    }
}
