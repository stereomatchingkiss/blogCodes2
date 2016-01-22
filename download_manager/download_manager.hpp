#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include "download_info.hpp"

#include <QObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QStringList>
#include <QUrl>

#include <map>
#include <queue>
#include <set>

class QNetworkAccessManager;

namespace dm{

namespace net{

/**
 * Manage multiple download files
 */
class download_manager : public QObject
{
    Q_OBJECT
public:
    explicit download_manager(QObject *obj = nullptr);

    int_fast64_t append(QUrl const &value,
                        QString const &save_at,
                        QString const &save_as);

    size_t get_max_download_size() const;
    size_t get_total_download_file() const;

    void set_max_download_size(size_t value);
    bool start_download(int_fast64_t uuid);

signals:
    void download_error(int_fast64_t uuid, QString error);
    void download_finished(int_fast64_t uuid, QString error);
    void download_progress(int_fast64_t uuid,
                           qint64 bytes_received,
                           qint64 bytes_total);
    void download_ready_read(int_fast64_t uuid);
    void downloading_size_decrease(size_t value);

private slots:        
    void download_finished();
    void download_progress(qint64 bytes_received,
                           qint64 bytes_total);
    void download_ready_read();

    void error(QNetworkReply::NetworkError code);

private:
    void connect_network_reply(QNetworkReply *reply);

    template<typename Index, typename Pair>
    bool create_dir(QString const &save_at, Index &index,
                    Pair const &pair)
    {
        QDir dir(save_at);
        if(!dir.exists()){
            if(!QDir().mkpath(save_at)){
                QMessageBox::warning(0, tr("Warning"),
                                     tr("Can not create directory"));
                index.erase(pair.first);
                return false;
            }
        }

        return true;
    }

    template<typename Index, typename Pair>
    bool create_file(QString const &save_at, QString const &save_as,
                     Index &index, Pair const &pair)
    {
        bool success = true;
        index.modify(pair.first, [&](auto &v)
        {
            v.file_ = std::make_shared<QFile>(save_at + "/" + save_as);
            if(!v.file_->open(QIODevice::WriteOnly)){
                qDebug()<<__func__<<" cannot open file";
                QMessageBox::warning(0, tr("Warning"),
                                     tr("Can not save download file"));
                success = false;
            }
        });

        return success;
    }

    int_fast64_t start_download_impl(QUrl const &value,
                                     QString const &save_at,
                                     QString const &save_as);

    download_info_index download_info_;
    QNetworkAccessManager *manager_;
    size_t max_download_size_;
    size_t total_download_files_;
    int_fast64_t uuid_;
};

}

}

#endif // DOWNLOAD_MANAGER_H
