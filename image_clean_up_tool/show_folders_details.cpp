#include "show_folders_details.hpp"
#include "ui_show_folders_details.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageReader>
#include <QMessageBox>
#include <QTableWidget>

namespace{

class number_widget_item : public QTableWidgetItem {
public:
    bool operator <(const QTableWidgetItem &other) const
    {
        return text().toInt() < other.text().toInt();
    }
};

void check_has_unicode_chars(QFileInfoList const &im_info_list)
{
    for(auto const &im_info : im_info_list){
        auto const bname = im_info.completeBaseName();
        for(QChar charecter : bname){
            if(charecter > 127){
                qDebug()<<im_info.absoluteFilePath();
                continue;
            }
        }
    }
}

void save_as_jpg(QFileInfoList const &im_info_list)
{
    for(auto const &im_info : im_info_list){
        auto img_reader = std::make_unique<QImageReader>(im_info.absoluteFilePath());
        if(img_reader->format() == "webp"){
            qDebug()<<__func__<<im_info.absoluteFilePath()<<", "<<img_reader->canRead()<<", "<<img_reader->format();
            bool can_save_new_image = false;
            if(img_reader->canRead()){
                img_reader = nullptr;
                QImage img(im_info.absoluteFilePath());
                can_save_new_image = img.save(QString("%1/%2.jpg").arg(im_info.absolutePath(), im_info.completeBaseName()));
            }
            if(can_save_new_image && im_info.suffix() == "webp"){
                QFile::remove(im_info.absoluteFilePath());
            }
        }
    }
}

void move_dir_if_files_num_small(int im_size, QFileInfo const &finfo, QString const &move_to)
{
    if(im_size < 9){
        QDir dir(finfo.absoluteFilePath());
        dir.rename(finfo.absoluteFilePath(), move_to);
    }

}

}

show_folders_details::show_folders_details(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::show_folders_details)
{
    ui->setupUi(this);
}

show_folders_details::~show_folders_details()
{
    delete ui;
}

void show_folders_details::on_pushButtonOk_clicked()
{
    auto const finfo_list = QDir(ui->lineEditFolder->text()).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    ui->tableWidget->setRowCount(finfo_list.size());
    auto const image_types = QStringList()<<"*.png"<<"*.jpg"<<"*.jpeg"<<"*.bmp"<<"*.webp";
    for(int i = 0; i != finfo_list.size(); ++i){
        auto const &finfo = finfo_list[i];
        if(finfo_list[i].isDir()){
            auto dir_item = new QTableWidgetItem(finfo.baseName());
            dir_item->setFlags(dir_item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(i, 0, dir_item);

            auto const im_info_list = QDir(finfo.absoluteFilePath()).entryInfoList(image_types, QDir::Files | QDir::NoDotAndDotDot);
            //check_has_unicode_chars(im_info_list);
            //save_as_jpg(im_info_list);
            auto const im_size = im_info_list.size();
            //move_dir_if_files_num_small(im_size, finfo, "E:/computer_vision_dataset/coins/world_coins/coins/data/train/splitter/" + finfo.baseName());
            auto im_size_item = new number_widget_item;
            im_size_item->setText(QString::number(im_size));
            im_size_item->setFlags(im_size_item->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(i, 1, im_size_item);
        }
    }
    ui->tableWidget->sortItems(1);
}

