#ifndef DOWNLOAD_MODEL_H
#define DOWNLOAD_MODEL_H

#include "download_item.hpp"

#include <QAbstractTableModel>

namespace dm{

namespace net{

class download_manager;

}

namespace model{

/**
 * Model manage download data, the api inherited from
 * QAbstractTableModel do the same job as the parent,
 * please check the Qt doc to find out what are those
 * api doing if needed
 */
class download_model : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit download_model(QObject *parent = nullptr);

    /**
     * Append new data to the download list, if the
     * data append to the downlad manager more than
     * maximum download size, it would not start the
     * download progress but save the information need
     * to start the download, you should call the api
     * "start_download" to start the appended download
     * request but not call the append api again, because
     * append api will add new download data into the download
     * list rather than start the download progress of
     * appended request
     * @param url url of the data want to download
     * @param save_at the location you want to save the file at
     * @param save_as the file name of the download target
     * @return unique id for each download request, value >= 0
     * indicate append operation success and vice versa
     */
    bool append(QUrl const &value, QString const &save_at,
                QString const &save_as);

    int columnCount(const QModelIndex &parent = {}) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * Erase the download item
     * @param row the row want to erase
     * @return true if the row can be erased and vice versa
     */
    bool erase(int row);

    /**
     * Get the maximum download size of download manager,
     * this value determine how many items could be downloaded
     * at the same time
     * @return maximum download size
     */
    size_t get_max_download_size() const;

    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index,
                 const QVariant &value, int role) override;

    /**
     * Set maximum download size, this value determine how
     * many items could be downloaded at the same time
     * @param value value of maximum download size
     */
    void set_max_download_size(size_t value);

    /**
     * start the download in the download list, every
     * download has it associated unique id
     * @param uuid the unique id of the item want to download
     * @return true if the download can begin and vice versa
     */
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
