#include "download_manager.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QTextStream>

namespace dm{

namespace net{

download_manager::download_manager(QObject *obj) :
    QObject(obj),
    manager_{new QNetworkAccessManager(obj)},
    max_download_size_{4},
    total_download_files_{0}
{

}

bool download_manager::append(QUrl const &value,
                              QString const &save_at,
                              QString const &save_as)
{
    auto *reply = start_download(value);
    if(reply){
        return download_info_.insert({reply,
                                      {save_at, save_as}}).second;
    }

    return false;
}

size_t download_manager::get_max_download_size() const
{
    return max_download_size_;
}

void download_manager::set_max_download_size(size_t value)
{
    max_download_size_ = value;
}

QNetworkReply* download_manager::start_download(QUrl const &value)
{     
    if(max_download_size_ < total_download_files_){
        QNetworkRequest request(value);
        auto *current_download = manager_->get(request);
        connect(current_download, SIGNAL(downloadProgress(qint64,qint64)),
                SIGNAL(download_progress(qint64,qint64)));
        connect(current_download, SIGNAL(finished()),
                SIGNAL(download_finished()));
        connect(current_download, SIGNAL(readyRead()),
                SIGNAL(download_ready_read()));
        ++total_download_files_;

        return current_download;
    }

    return nullptr;
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
    recycle rc(reply);

    auto it = download_info_.find(reply);
    if(it != std::end(download_info_)){
        auto const data = reply->readAll();
        save_data(it->second, data);

        download_info_.erase(it);
        --total_download_files_;
    }

    emit download_finished(reply->url());
}

void download_manager::
save_data(download_manager::file_info const &info,
          QByteArray const &data)
{
    QDir dir(info.save_at_);
    if(!dir.exists() && QFileInfo(info.save_at_).isDir()){
        dir.mkpath(info.save_at_);
    }
    QFile file(info.save_at_ + "/" + info.save_as_);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);
    out<<data;
}

download_manager::file_info::
file_info(QString const &save_at,
          QString const &save_as) :
    save_at_(save_at),
    save_as_(save_as)
{

}

}

}
