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

    if(current_download_row_ >= ui->tableWidgetDownloadInfo->rowCount()){
        current_download_row_ = 0;
    }
}

void simple_downloader::add_files(QStringList const &links, QStringList const &save_at)
{
    qDebug()<<"table widget row count:"<<ui->tableWidgetDownloadInfo->rowCount();
    int origin_row_count = ui->tableWidgetDownloadInfo->rowCount();
    for(size_t i = 0; i != links.size(); ++i){
        qDebug()<<"add link:"<<links[i];
        ui->tableWidgetDownloadInfo->insertRow(origin_row_count);
        ui->tableWidgetDownloadInfo->setItem(origin_row_count, Link, new QTableWidgetItem(links[i]));
        ui->tableWidgetDownloadInfo->setItem(origin_row_count, State, new QTableWidgetItem("new link"));
        ui->tableWidgetDownloadInfo->setItem(origin_row_count, SaveAt, new QTableWidgetItem(save_at[i]));
        ++origin_row_count;
    }

    if(current_download_row_ >= ui->tableWidgetDownloadInfo->rowCount()){
        current_download_row_ = 0;
    }
}

std::pair<QStringList, QStringList> simple_downloader::get_files_url() const
{
    QStringList files, urls;
    for(int i = 0; i != ui->tableWidgetDownloadInfo->rowCount(); ++i){
        files.emplaceBack(ui->tableWidgetDownloadInfo->item(i, Link)->text());
        urls.emplaceBack(ui->tableWidgetDownloadInfo->item(i, SaveAt)->text());
    }

    return {files, urls};
}

void simple_downloader::download()
{
    if(auto const link = ui->tableWidgetDownloadInfo->item(current_download_row_, Link)){
        qDebug()<<"link exist:"<<link->data(Qt::DisplayRole).toString();        
        if(auto const state = ui->tableWidgetDownloadInfo->item(current_download_row_, State); state){
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
            request.setRawHeader("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) "
                                               "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.47 Safari/537.36");
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
            ui->tableWidgetDownloadInfo->item(current_download_row_, State)->setText("Fail");
            ++current_download_row_;
            download();
        }
    }    
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
    setEnabled(false);
    current_download_row_ = 0;
    download();
}

void simple_downloader::download_progress(qint64 bytes_received, qint64 bytes_total)
{
    qDebug()<<__func__<<": progress = "<<bytes_received<<"/"<<bytes_total;
}

void simple_downloader::on_pushButtonRedownloadFailure_clicked()
{
    for(int i = 0; i != ui->tableWidgetDownloadInfo->rowCount(); ++i){
        ui->tableWidgetDownloadInfo->item(i, State)->setText("new link");
    }

    download();
}

void simple_downloader::on_pushButtonChangeSuffix_clicked()
{
    for(int i = 0; i != ui->tableWidgetDownloadInfo->rowCount(); ++i){
        if(auto *item = ui->tableWidgetDownloadInfo->item(i, Link)){
            QString img_link(item->data(Qt::DisplayRole).toString());
            // Check if a dot was found and is part of a suffix
            if (int dot_index = img_link.lastIndexOf('.'); dot_index != -1) {
                // Replace the suffix
                img_link = img_link.left(dot_index) + "." + ui->lineEditChangeSuffix->text();
                item->setData(Qt::DisplayRole, img_link);
            }
        }

        ui->tableWidgetDownloadInfo->item(i, State)->setText("new link");
    }

    current_download_row_ = 0;
}

