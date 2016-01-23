#ifndef DOWNLOAD_MODEL_H
#define DOWNLOAD_MODEL_H

#include "download_item.hpp"

#include <QAbstractTableModel>

namespace dm{

namespace net{

class download_manager;

}

namespace model{

class download_model : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit download_model(QObject *parent = nullptr);

    bool append(QUrl const &value, QString const &save_at,
                QString const &save_as);

    int columnCount(const QModelIndex &parent = {}) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    bool erase(int row);

    size_t get_max_download_size() const;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index,
                 const QVariant &value, int role) override;
    void set_max_download_size(size_t value);
    void start_download(int row);

private slots:    
    void download_error(int_fast64_t uuid, QString error);
    void download_finished(int_fast64_t uuid, QString error);
    void download_progress(int_fast64_t uuid,
                           qint64 bytes_received,
                           qint64 bytes_total);
    void download_ready_read(int_fast64_t uuid);
    void download_size_changed(size_t value);

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

    int get_row(int_fast64_t uuid) const;

    bool insertRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;

    bool removeRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;

    model::download_index data_;
    net::download_manager *manager_;
    size_t max_download_size_;
};

}

}

#endif // DOWNLOAD_MODEL_H
