#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <QStringList>

class QProcess;

QStringList get_running_process();

void kill_qprocess(QProcess &process) noexcept;

#endif // UTILITY_HPP

