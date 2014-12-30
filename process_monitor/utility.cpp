#include "utility.hpp"

#include <QDebug>
#include <QProcess>

QString get_running_process()
{
    QProcess tasklist;
    tasklist.start(
                "PowerShell",
                QStringList() << "get-process | get-item -erroraction silentlycontinue | format-table name, directory"
                );
    tasklist.waitForFinished();
    QString output = tasklist.readAllStandardOutput();    ;
    qDebug()<<output;

    return output;
}
