#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <QProcess>
#include <QStringList>

QStringList get_running_process();

void kill_qprocess(QProcess &process) noexcept;

void restart_qprocess(QProcess &process, QProcess::ProcessError error) noexcept;

#endif // UTILITY_HPP

