#ifndef LABELS_UPDATE_HPP
#define LABELS_UPDATE_HPP

#include <QString>

class labels_update
{
public:
    explicit labels_update(QString const &file);

    void update_path(QString const &path);

private:
    QString file_;
};

#endif // LABELS_UPDATE_HPP
