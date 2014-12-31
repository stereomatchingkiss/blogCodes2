#ifndef QPROCESS_GUARD_H
#define QPROCESS_GUARD_H

#include <QProcess>

#include <functional>

/**
 *@brief close the QProcess(blocking) when destroy
 *
 *
 * By default, this class will call kill() to close the\n
 * process and print out the error messages if the\n
 * process cannot be closed within 3 seconds
 */

class QProcess;

class qprocess_guard
{    
public:    
    explicit qprocess_guard(QProcess *process = nullptr);
    ~qprocess_guard();

    qprocess_guard(qprocess_guard const&) = delete;
    qprocess_guard& operator=(qprocess_guard const&) = delete;

    qprocess_guard(qprocess_guard &&) noexcept;
    qprocess_guard& operator=(qprocess_guard &&data) noexcept;

    void set_error_handle(std::function<void(QProcess&)> func);
    void set_finish_time(int msecs) noexcept;
    void set_qprocess(QProcess *process) noexcept;

private:
    std::function<void(QProcess&)> error_handle_;

    int finish_time_;

    QProcess *process_;

};

#endif // QPROCESS_GUARD_H
