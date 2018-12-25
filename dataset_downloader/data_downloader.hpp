#ifndef DATA_DOWNLOADER_HPP
#define DATA_DOWNLOADER_HPP

#include <QNetworkReply>
#include <QObject>
#include <QSslError>

class QNetworkAccessManager;

class data_downloader : public QObject
{
    Q_OBJECT
public:
    explicit data_downloader(QObject *parent = nullptr);

    void download_data(QUrl const &url);
    void download_data(QList<QUrl> const &urls);
    void download_data(QString const &url);
    void download_data(QStringList const &urls);

signals:
    void download_finished(QByteArray const &content, QUrl const &url);
    void network_error(QNetworkReply::NetworkError code);
    void ssl_error(QList<QSslError> const &error);

private:
    void create_connection(QNetworkReply *reply);
    void download_progress(qint64 bytesReceived, qint64 bytesTotal);
    void process_download_finished();

    QNetworkAccessManager *manager_;
    QList<QUrl> urls_;
};

#endif // DATA_DOWNLOADER_HPP
