#ifndef START_QPROCESS_REPEAT_H
#define START_QPROCESS_REPEAT_H

#include <QObject>
#include <QProcess>
#include <QStringList>

/**
 * @brief Start the process repeatly by QProcess
 */
class start_qprocess_repeat : public QObject
{
    Q_OBJECT

public:
    start_qprocess_repeat(QProcess &process,
                          const QString &program,
                          const QStringList &arguments);
    ~start_qprocess_repeat();

private slots:
    void restart(QProcess::ProcessError error);

private:
    QStringList arguments_;

    QProcess *process_;
    QString program_;
};

#endif // START_QPROCESS_REPEAT_H
