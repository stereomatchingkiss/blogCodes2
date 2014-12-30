#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

/**
 * @brief open the process of list in file and reopen\n
 * those processes if they are down because some errors happens
 */

#include "qprocess_guard.hpp"

#include <QProcess>
#include <QStringList>

#include <map>
#include <memory>
#include <utility>

class process_manager
{
public:
    explicit process_manager(QString const &file);
    ~process_manager();

private:
    void create_process(QString const &data);

    bool get_argument(QString const &data) const;

private:
    QStringList arguments_;

    std::map<QString, std::unique_ptr<qprocess_guard> > guard_process_;
    std::map<QString, std::unique_ptr<QProcess> > process_;
    QString program_;
};

#endif // PROCESS_MANAGER_H
