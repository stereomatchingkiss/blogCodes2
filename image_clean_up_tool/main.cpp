#include "mainwindow.hpp"

#include <QApplication>

#include <QDir>
#include <QFile>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("ThamSoft");
    QCoreApplication::setApplicationName("image_clean_up_tool");

    /*QString const root_folder("D:/programming/computer_vision_data/fiftyone/");
    QString const phase = "train";
    auto const finfo_list = QDir(QString("%1/license_plate/%2/labels").arg(root_folder, phase)).entryInfoList();
    QString const copy_to_dir(QString("%1/license_plate/%2/images").arg(root_folder, phase));
    QDir().mkpath(copy_to_dir);
    for(auto const &finfo : finfo_list){
        auto const bname = finfo.baseName();
        QString const source = QString("%1/open-images-v7/%2/data/%3.jpg").arg(root_folder, phase, bname);
        if(!QFile::exists(source)){
            qDebug()<<source;
        }
        QFile::copy(source,
                    QString("%1/%2.jpg").arg(copy_to_dir, bname));
    }//*/    

    MainWindow w;
    w.show();
    return a.exec();
}
