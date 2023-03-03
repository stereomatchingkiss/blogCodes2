#include "simple_downloader.hpp"
#include "ui_simple_downloader.h"

#include <QDebug>
#include <QFileInfo>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>

namespace{

enum table_column{
    Link,
    State,
    SaveAt
};

}

simple_downloader::simple_downloader(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::simple_downloader),
    current_download_row_(0),
    manager_(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    QSslConfiguration configure = QSslConfiguration::defaultConfiguration();
    configure.setPeerVerifyMode(QSslSocket::VerifyNone);
    QSslConfiguration::setDefaultConfiguration(configure);

    QSettings settings("tham soft", "link generator");
    if(settings.contains("simple_downloader/geometry")){
        restoreGeometry(settings.value("simple_downloader/geometry").toByteArray());
    }
}

simple_downloader::~simple_downloader()
{
    QSettings settings("tham soft", "link generator");
    settings.setValue("simple_downloader/geometry", saveGeometry());

    delete ui;
}

void simple_downloader::add_files(QStringList links, QString const &save_at)
{
    links.removeDuplicates();
    qDebug()<<"table widget row count:"<<ui->tableWidgetDownloadInfo->rowCount();
    int origin_row_count = ui->tableWidgetDownloadInfo->rowCount();
    for(auto const &link : links){
        qDebug()<<"add link:"<<link;
        ui->tableWidgetDownloadInfo->insertRow(origin_row_count);
        ui->tableWidgetDownloadInfo->setItem(origin_row_count, Link, new QTableWidgetItem(link));
        ui->tableWidgetDownloadInfo->setItem(origin_row_count, State, new QTableWidgetItem("new link"));
        ui->tableWidgetDownloadInfo->setItem(origin_row_count, SaveAt, new QTableWidgetItem(save_at));
        ++origin_row_count;
    }
}

void simple_downloader::download()
{    
    auto link = ui->tableWidgetDownloadInfo->item(current_download_row_, Link);
    if(link){
        qDebug()<<"link exist:"<<link->data(Qt::DisplayRole).toString();
        auto state = ui->tableWidgetDownloadInfo->item(current_download_row_, State);
        if(state){
            qDebug()<<"state:"<<state->data(Qt::DisplayRole).toString();
            auto const download_link = link->data(Qt::DisplayRole).toString();
            //{'User-Agent' : 'Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36'}
            //Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36
            auto const url = QUrl(download_link);
            QNetworkRequest request(url);
            //GET / HTTP/1.1
            //Accept-Encoding: identity
            //Host: www.google.com
            //Connection: close
            //GET /media/photos/11916/00032.jpg HTTP/1.1
            //Host: 18comic.org
            //cache-control: no-cache
            request.setRawHeader("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36");
            //request.setRawHeader("Host", "18comic.org");
            //request.setRawHeader("cache-control", "no-cache");
            //request.setRawHeader("GET", "/media/photos/11916/00032.jpg HTTP/1.1");
            //request.setRawHeader("Postman-Token", "6a22b0a2-2f7b-4b99-aef6-76116bf82376");
            //request.setRawHeader("Postman-Token", "6a22b0a2-2f7b-4b99-aef6-76116bf82376");
            //qDebug()<<__func__<<":"<<request.rawHeader();
            auto reply = manager_->get(request);
            //QList<QSslError> expectedSslErrors;
            //QSslError ignoreNOErrors(QSslError::NoError);
            //expectedSslErrors.append(ignoreNOErrors);
            //reply->ignoreSslErrors(expectedSslErrors);
            connect(reply, &QNetworkReply::errorOccurred, this, &simple_downloader::network_error);
            connect(reply, &QNetworkReply::downloadProgress, this, &simple_downloader::download_progress);
            connect(reply, &QNetworkReply::finished, this, &simple_downloader::download_finished);
        }else{
            return;
        }
    }else{
        setEnabled(true);
    }
}

void simple_downloader::download_finished()
{    
    if(auto *reply = qobject_cast<QNetworkReply*>(sender()); reply){
        if(reply->error() == QNetworkReply::NoError){
            qDebug()<<"download finish no error";
            QString const save_at =
                    ui->tableWidgetDownloadInfo->item(current_download_row_, SaveAt)->data(Qt::DisplayRole).toString();
            QString file_name = ui->tableWidgetDownloadInfo->item(current_download_row_, Link)->
                    data(Qt::DisplayRole).toString();
            ui->tableWidgetDownloadInfo->removeRow(current_download_row_);
            file_name = QFileInfo(file_name).fileName();
            if(QFile file(save_at + "/" + QFileInfo(save_at).baseName() + "_" + file_name);
                    file.open(QIODevice::WriteOnly)){
                file.write(reply->readAll());
            }
            reply->deleteLater();
            reply = nullptr;
            download();
        }else{
            qDebug()<<"download finish with error:"<<reply->errorString();
            auto *item = ui->tableWidgetDownloadInfo->item(current_download_row_, State);
            if(item->data(Qt::DisplayRole).toString() == "new link"){
                download_with_different_suffix();
            }else{
                ++current_download_row_;
                download();
            }
        }
    }
}

void simple_downloader::download_with_different_suffix()
{
    auto *item = ui->tableWidgetDownloadInfo->item(current_download_row_, Link);
    QString img_link(item->data(Qt::DisplayRole).toString());
    QFileInfo finfo(item->data(Qt::DisplayRole).toString());
    qDebug()<<__func__<<"full link:"<<item->data(Qt::DisplayRole).toString();
    qDebug()<<__func__<<finfo.fileName()<<","<<finfo.completeBaseName();
    if(finfo.suffix().toLower() != "png"){
        auto const suffix = finfo.suffix().toLower();
        if(suffix == "jpg"){
            item->setData(Qt::DisplayRole, img_link.replace(".jpg", ".png"));
        }else{
            item->setData(Qt::DisplayRole, img_link.replace(".jpeg", ".png"));
        }
    }else{
        item->setData(Qt::DisplayRole, img_link.replace(".png", ".jpg"));
    }

    auto *state_item = ui->tableWidgetDownloadInfo->item(current_download_row_, State);
    state_item->setData(Qt::DisplayRole, "Change suffix, download again");

    download();
}

void simple_downloader::network_error(QNetworkReply::NetworkError)
{
    auto *reply = qobject_cast<QNetworkReply*>(sender());
    qDebug()<<__func__<<reply->errorString();
}

void simple_downloader::on_pushButtonClearAll_clicked()
{    
    current_download_row_ = 0;
    ui->tableWidgetDownloadInfo->setRowCount(0);
}

void simple_downloader::on_pushButtonDownload_clicked()
{ 
    download();
}

void simple_downloader::download_progress(qint64 bytes_received, qint64 bytes_total)
{
    qDebug()<<__func__<<": progress = "<<bytes_received<<"/"<<bytes_total;
}
