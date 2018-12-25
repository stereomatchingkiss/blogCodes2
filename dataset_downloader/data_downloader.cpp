#include "data_downloader.hpp"

#include <QDebug>
#include <QNetworkAccessManager>

data_downloader::data_downloader(QObject *parent) :
    QObject(parent),
    manager_(new QNetworkAccessManager(this))
{

}

void data_downloader::download_data(const QUrl &url)
{
    create_connection(manager_->get(QNetworkRequest(url)));
}

void data_downloader::download_data(const QList<QUrl> &urls)
{
    urls_.append(urls);
    if(!urls_.empty()){
        download_data(urls_[0]);
        urls_.pop_front();
    }
}

void data_downloader::download_data(const QString &url)
{
    download_data(QUrl(url));
}

void data_downloader::download_data(const QStringList &urls)
{
    QList<QUrl> temp_urls;
    std::copy(std::begin(urls), std::end(urls), std::back_inserter(temp_urls));
    download_data(temp_urls);
}

void data_downloader::create_connection(QNetworkReply *reply)
{
    connect(reply, &QNetworkReply::finished, this, &data_downloader::process_download_finished);
    connect(reply, &QNetworkReply::downloadProgress, this, &data_downloader::download_progress);
    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            [=](QNetworkReply::NetworkError code){ emit network_error(code); });
    connect(reply, &QNetworkReply::sslErrors, this, &data_downloader::ssl_error);
}

void data_downloader::download_progress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(auto reply = qobject_cast<QNetworkReply*>(sender())){
        qDebug()<<__func__<<reply->url()<<": bytesReceived = "<<bytesReceived<<", bytesTotal = "<<bytesTotal;
    }else{
        qDebug()<<__func__<<"bytesReceived = "<<bytesReceived<<", bytesTotal = "<<bytesTotal;
    }
}

void data_downloader::process_download_finished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if(reply){
       qDebug()<<__func__<<"download finished:"<<reply->url();
       download_finished(reply->readAll(), reply->url());
       reply->deleteLater();
       if(!urls_.empty()){
           download_data(urls_[0]);
           urls_.pop_front();
       }
    }else{
       qDebug()<<__func__<<"cannot cast to QNetworkReply";
    }
}
