#ifndef QPROCESS_GUARD_H
#define QPROCESS_GUARD_H

#include <QObject>
#include <QProcess>

#include <functional>

/**
 *@brief 1 : close the QProcess(blocking) when destroy\n
 * 2 : restart the process if the process down
 *
 *
 * By default, this class will call kill() to close the\n
 * process and print out the error messages if the\n
 * process cannot be closed within 3 seconds
 */

class qprocess_guard : public QObject
{    
    Q_OBJECT

public:    
    explicit qprocess_guard(QProcess *process,
                            QString const &program,
                            QStringList const &arguments,
                            QObject *parent = nullptr);
    ~qprocess_guard();

    qprocess_guard(qprocess_guard const&) = delete;
    qprocess_guard& operator=(qprocess_guard const&) = delete;

    void enable_restart(bool value) noexcept;

    void set_close_handle(std::function<void(QProcess&)> func);
    void set_finish_time(int msecs) noexcept;
    void set_restart_handle(std::function<void(QProcess&, QProcess::ProcessError)> func);

private slots:
    void restart(QProcess::ProcessError error) noexcept;

private:
    std::function<void(QProcess&)> close_handle_;

    bool enable_restart_;    

    int finish_time_;

    QProcess *process_;

    std::function<void(QProcess&, QProcess::ProcessError)> restart_handle_;
};

#endif // QPROCESS_GUARD_H
