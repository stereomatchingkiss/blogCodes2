#ifndef DOWNLOAD_MODEL_H
#define DOWNLOAD_MODEL_H

#include "download_item.hpp"

#include <QAbstractTableModel>

namespace dm{

namespace net{

class download_manager;

}

namespace model{

class download_model : QAbstractTableModel
{
    Q_OBJECT
public:
    explicit download_model(QObject *parent = nullptr);

    bool append(QUrl const &value, QString const &save_at,
                QString const &save_as);

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

private slots:
    void download_size_changed(size_t value);
    void download_finished(int_fast64_t uuid);
    void download_progress(int_fast64_t uuid,
                           qint64 bytes_received,
                           qint64 bytes_total);
    void download_ready_read(int_fast64_t uuid);

private:
    template<typename Iter>
    QModelIndex const get_index(Iter it,
                                download_item::tag col) const
    {
        return index(get_row(it),
                     static_cast<int>(col));
    }

    QModelIndex const get_index(int_fast64_t uuid,
                                download_item::tag col) const;

    template<typename Iter>
    int get_row(Iter it) const
    {
        auto r_it = data_.project<random>(it);
        auto &ran = data_.get<random>();
        return r_it - std::begin(ran);
    }

    int get_row(int_fast64_t uuid) const
    {
        auto const &id_set = data_.get<uid>();
        auto id_it = id_set.find(uuid);
        if(id_it != std::end(id_set)){
            return data_.project<random>(id_it) -
                    std::begin(data_.get<random>());
        }

        return -1;
    }

    bool insertRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;

    model::download_index data_;
    net::download_manager *manager_;
};

}

}

#endif // DOWNLOAD_MODEL_H
