#include "image_cropper.hpp"
#include "ui_image_cropper.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QMessageBox>

image_cropper::image_cropper(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::image_cropper)
{
    ui->setupUi(this);
}

image_cropper::~image_cropper()
{
    delete ui;
}

void image_cropper::on_pushButtonSelectImage_clicked()
{
    auto const im_name =
            QFileDialog::getOpenFileName(this, tr("Select image want to split"), ui->lineEditImageUrl->text());
    ui->lineEditImageUrl->setText(im_name);
}

void image_cropper::on_pushButtonSplit_clicked()
{
    if(!QFile(ui->lineEditImageUrl->text()).exists()){
        QMessageBox::warning(this, tr("image_clean_up_tool"), tr("Image do not exist"));
        return;
    }

    if(!split_image(ui->lineEditImageUrl->text())){
        QMessageBox::warning(this, tr("image_clean_up_tool"), tr("Cannot open image"));
    }
}


void image_cropper::on_pushButtonSeectFolder_clicked()
{
    auto const dir_name =
            QFileDialog::getExistingDirectory(this, tr("Select folder with images want to split"), ui->lineEditFolderUrl->text());
    ui->lineEditFolderUrl->setText(dir_name);
}


void image_cropper::on_pushButtonSplitImages_clicked()
{
    auto const finfo_list = QDir(ui->lineEditFolderUrl->text()).entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for(auto const &finfo : finfo_list){
        split_image(finfo.absoluteFilePath());
    }
}

bool image_cropper::split_image(const QString &img_url) const
{
    if(QImage img(img_url); !img.isNull()){
        auto const split_width = img.width() * (ui->spinBoxSplitLeftRightRatioWidth->value() / 100.0);
        auto const split_height = img.height() * (ui->spinBoxSplitLeftRightRatioHeight->value() / 100.0);
        QFileInfo const info(img_url);
        auto const im_folder = info.absoluteDir().path();
        auto const im_base_name = info.baseName();
        qDebug()<<QString("%1/%2_0.jpg").arg(im_folder, im_base_name);
        img.copy(QRect(0, 0, split_width, split_height)).save(QString("%1/%2_0.jpg").arg(im_folder, im_base_name));
        img.copy(QRect(split_width, 0, img.width() - split_width, split_height)).
                 save(QString("%1/%2_1.jpg").arg(im_folder, im_base_name));
        return true;
    }

    qDebug()<<__func__<<" cannot open image : "<<img_url;

    return false;
}

