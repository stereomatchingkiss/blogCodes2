#ifndef KILL_QPROCESS_H
#define KILL_QPROCESS_H

/**
 * @brief kill the QProcess and print out\n
 * the error message
 */

class QProcess;

void kill_qprocess(QProcess &process) noexcept;

#endif // KILL_QPROCESS_H
