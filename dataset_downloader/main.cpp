#include <QApplication>

#include "data_downloader.hpp"

#include <kaggle_face_detection_parser.hpp>

#include <QDebug>
#include <QDir>
#include <QImage>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(argc != 3){
        qDebug()<<"must enter location of json and location to save the download images";
        return -1;
    }

    QDir().mkpath(argv[2]);
    kaggle_face_detection_parser parser;
    data_downloader downloader;
    QObject::connect(&downloader, &data_downloader::download_finished, [=](QByteArray const &array, QUrl const &url)
    {
        QImage const img = QImage::fromData(array);
        if(!img.isNull()){
            auto const splits = url.toString().split("/");
            if(!splits.isEmpty()){
                img.save(QString(argv[2]) + "/" + splits[splits.size() - 1]);
            }else{
                qDebug()<<"url "<<url<<" cannot split";
            }
        }else{
            qDebug()<<"cannot download image:"<<url;
        }
    });
    auto const parse_results = parser.parse(argv[1]);
    for(auto const &block : parse_results.blocks_){
        downloader.download_data(block.content_);
    }

    return a.exec();
}
