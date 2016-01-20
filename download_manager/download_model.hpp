#ifndef DOWNLOAD_MODEL_H
#define DOWNLOAD_MODEL_H

#include "download_item.hpp"

#include <QAbstractTableModel>

namespace dm{

namespace model{

class download_model : QAbstractTableModel
{
    Q_OBJECT
public:
    explicit download_model(QObject *parent = nullptr);

    bool append(QUrl const &value);

    int columnCount(const QModelIndex &parent = {}) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;

    bool removeRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index,
                 const QVariant &value, int role) override;

private:
    bool insertRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;

    dm::model::download_index data_;
};

}

}

#endif // DOWNLOAD_MODEL_H
