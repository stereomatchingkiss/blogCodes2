#include "download_manager.hpp"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QNetworkAccessManager>

namespace dm{

namespace net{

download_manager::download_manager(QObject *obj) :
    QObject(obj),
    manager_{new QNetworkAccessManager(obj)},
    max_download_size_{4},
    total_download_files_{0},
    uuid_{0}
{

}

int_fast64_t download_manager::
append(QUrl const &value,
       QString const &save_at,
       QString const &save_as)
{
    return start_download_impl(value, save_at, save_as);
}

size_t download_manager::get_max_download_size() const
{
    return max_download_size_;
}

size_t download_manager::get_total_download_file() const
{
    return total_download_files_;
}

void download_manager::set_max_download_size(size_t value)
{
    max_download_size_ = value;
}

int_fast64_t download_manager::start_download_impl(QUrl const &value,
                                                   QString const &save_at,
                                                   QString const &save_as)
{     
    if(total_download_files_ < max_download_size_){
        QNetworkRequest request(value);

        auto &uid_index = download_info_.get<uid>();

        auto *reply = manager_->get(request);
        auto pair = uid_index.insert({uuid_, reply,
                                      save_at, save_as});
        if(pair.second)
        {
            QDir dir(save_at);
            if(!dir.exists()){
                if(!QDir().mkpath(save_at)){
                    QMessageBox::warning(0, tr("Warning"),
                                         tr("Can not create directory"));
                    uid_index.erase(pair.first);
                    return -1;
                }
            }
            uid_index.modify(pair.first, [&](auto &v)
            {
                v.file_ = std::make_shared<QFile>(save_at + "/" + save_as);
                if(!v.file_->open(QIODevice::WriteOnly)){
                    qDebug()<<__func__<<" cannot open file";
                    QMessageBox::warning(0, tr("Warning"),
                                         tr("Can not save download file"));
                    return;
                }
            });
            connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                    SLOT(download_progress(qint64,qint64)));
            connect(reply, SIGNAL(finished()),
                    SLOT(download_finished()));
            connect(reply, SIGNAL(readyRead()),
                    SLOT(download_ready_read()));
            ++total_download_files_;

            return uuid_++;
        }
    }else{
        qDebug()<<this<<" max_download_size_ >= total_download_files_";
    }

    return -1;
}

void download_manager::download_finished()
{
    class recycle
    {
    public:
        explicit recycle(QNetworkReply *value) :
            value_(value) {}
        ~recycle(){ value_->deleteLater(); }

    private:
        QNetworkReply *value_;
    };

    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply){
        recycle rc(reply);

        auto &net_index = download_info_.get<net_reply>();
        auto it = net_index.find(reply);
        if(it != std::end(net_index)){
            auto const uuid = it->uuid_;
            net_index.erase(it);
            emit download_finished(uuid);
            emit downloading_size_decrease(--total_download_files_);
        }
    }else{
        qDebug()<<__func__<<" : do not exist";
    }
}

void download_manager::
download_progress(qint64 bytes_received,
                  qint64 bytes_total)
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply){
        auto &net_index = download_info_.get<net_reply>();
        auto it = net_index.find(reply);
        qDebug()<<__func__<< " receive "<<bytes_received;
        qDebug()<<__func__<< " total "<<bytes_total;
        if(it != std::end(net_index)){
            emit download_progress(it->uuid_, bytes_received,
                                   bytes_total);
        }
    }
}

void download_manager::download_ready_read()
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply){
        qDebug()<<__func__<<" ready read";
        auto &net_index = download_info_.get<net_reply>();
        auto it = net_index.find(reply);
        if(it != std::end(net_index)){
            QByteArray data(reply->bytesAvailable(), Qt::Uninitialized);
            reply->read(data.data(), data.size());
            it->file_->write(data);

            emit download_ready_read(it->uuid_);
        }
    }else{
        qDebug()<<__func__<< " cannot cast sender to reply";
    }
}

}

}
