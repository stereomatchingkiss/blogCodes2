#include "image_mover_mult.hpp"
#include "ui_image_mover_mult.h"

#include "utils/iterator_create.hpp"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QTableWidget>

#include <algorithm>

namespace{

QString const state_load_image_path("state_load_image_path");
QString const state_move_folder_paths("state_move_folder_paths");
int constexpr max_item_per_page(50);

}

image_mover_mult::image_mover_mult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::image_mover_mult),
    move_folder_widget_(new QTableWidget)
{
    ui->setupUi(this);

    QSettings settings("image_clean_up_tool", "image_mover_multi");
    if(settings.contains(state_load_image_path)){
        qDebug()<<"settings contains : "<<state_load_image_path;
        ui->lineEditImageFolder->setText(settings.value(state_load_image_path).toString());
    }

    QStringList paths;
    if(settings.contains(state_move_folder_paths)){
        paths = settings.value(state_move_folder_paths).toStringList();
    }

    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();

    move_folder_widget_->setColumnCount(2);
    move_folder_widget_->setRowCount(10);
    move_folder_widget_->setHorizontalHeaderLabels(QStringList()<<"folder path"<<"number");
    for(int i = 0; i != move_folder_widget_->rowCount(); ++i){
        auto item = new QTableWidgetItem(QString::number(i));
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        move_folder_widget_->setItem(i, 1, item);
        if(i < paths.size()){
            auto item_path = new QTableWidgetItem(paths[i]);
            move_folder_widget_->setItem(i, 0, item_path);
        }
    }
}

image_mover_mult::~image_mover_mult()
{
    QSettings settings("image_clean_up_tool", "image_mover_multi");
    settings.setValue(state_load_image_path, ui->lineEditImageFolder->text());

    delete ui;
}

void image_mover_mult::on_pushButtonLoadImages_clicked()
{
    if(!QDir(ui->lineEditImageFolder->text()).exists()){
        QMessageBox::warning(this, tr("image_clean_up_tool"), tr("Image folder do not exist"));
        return;
    }

    auto const iterate_flag = ui->checkBoxLoadImageRecursive->isChecked() ?
                QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    auto dir_it = create_image_iterator(ui->lineEditImageFolder->text(), iterate_flag);
    images_urls_.clear();
    while(dir_it.hasNext()){
        images_urls_.push_back(QFileInfo(dir_it.next()).absoluteFilePath());
    }
    qDebug()<<__func__<<": image size = "<<images_urls_.size();
    images_url_move_target_.clear();
    images_url_move_target_.resize(images_urls_.size());
    images_url_move_target_.fill(-1);

    if(images_urls_.size() >= max_item_per_page){
        ui->spinBoxSelectPage->setRange(1, (images_urls_.size() / max_item_per_page));
        ui->spinBoxSelectPage->setValue(1);
    }
    show_page(ui->spinBoxSelectPage->value());
}


void image_mover_mult::on_pushButtonSelectImageFolder_clicked()
{
    auto *editor = ui->lineEditImageFolder;
    auto const dir =
            QFileDialog::getExistingDirectory(this, tr("Select folder to move the file"), editor->text());
    editor->setText(dir);
}

void image_mover_mult::show_page(int index)
{
    auto const max_size = std::min(index * max_item_per_page, static_cast<int>(images_urls_.size()));
    qDebug()<<__func__<<max_size;
    ui->tableWidget->setRowCount(max_size);
    for(int i = 0; i != max_size; ++i){
        if(QImage img(images_urls_[i]); !img.isNull()){
            img = img.scaledToWidth(100);
            QTableWidgetItem *thumbnail = new QTableWidgetItem;
            thumbnail->setData(Qt::DecorationRole, QPixmap::fromImage(std::move(img)));
            thumbnail->setFlags(thumbnail->flags() ^ Qt::ItemIsEditable);
            ui->tableWidget->setItem(i, 0, thumbnail);
        }
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
}


void image_mover_mult::on_pushButtonSetFolderMoveTo_clicked()
{
    move_folder_widget_->show();
}


void image_mover_mult::on_pushButtonMoveImages_clicked()
{

}

