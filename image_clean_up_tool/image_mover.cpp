#include "image_mover.hpp"
#include "ui_image_mover.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageReader>
#include <QKeyEvent>
#include <QImage>
#include <QMessageBox>
#include <QSettings>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <execution>

namespace{

QString const state_button_1("state_button_1");
QString const state_button_2("state_button_2");
QString const state_button_3("state_button_3");
QString const state_button_4("state_button_4");
QString const state_button_5("state_button_5");
QString const state_image_folder("state_image_folder");

}

image_mover::image_mover(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::image_mover)
{
    ui->setupUi(this);

    setFocusPolicy(Qt::StrongFocus);

    QSettings settings("image_clean_up_tool", "image_mover");
    if(settings.contains(state_button_1)){
        ui->lineEdit_1->setText(settings.value(state_button_1).toString());
    }
    if(settings.contains(state_button_2)){
        ui->lineEdit_2->setText(settings.value(state_button_2).toString());
    }
    if(settings.contains(state_button_3)){
        ui->lineEdit_3->setText(settings.value(state_button_3).toString());
    }
    if(settings.contains(state_button_4)){
        ui->lineEdit_4->setText(settings.value(state_button_4).toString());
    }
    if(settings.contains(state_button_5)){
        ui->lineEdit_5->setText(settings.value(state_button_5).toString());
    }
    if(settings.contains(state_image_folder)){
        ui->lineEditImageFolder->setText(settings.value(state_image_folder).toString());
    }
}

image_mover::~image_mover()
{
    QSettings settings("image_clean_up_tool", "image_mover");
    settings.setValue(state_button_1, ui->lineEdit_1->text());
    settings.setValue(state_button_2, ui->lineEdit_2->text());
    settings.setValue(state_button_3, ui->lineEdit_3->text());
    settings.setValue(state_button_4, ui->lineEdit_4->text());
    settings.setValue(state_button_5, ui->lineEdit_5->text());
    settings.setValue(state_image_folder, ui->lineEditImageFolder->text());

    delete ui;
}

void image_mover::on_pushButtonSelectFolder_1_clicked()
{
    select_folder(ui->lineEdit_1);
}

void image_mover::on_pushButtonSelectFolder_2_clicked()
{
    select_folder(ui->lineEdit_2);
}

void image_mover::on_pushButtonSelectFolder_3_clicked()
{
    select_folder(ui->lineEdit_3);
}

void image_mover::on_pushButtonSelectFolder_4_clicked()
{
    select_folder(ui->lineEdit_4);
}

void image_mover::on_pushButtonSelectFolder_5_clicked()
{
    select_folder(ui->lineEdit_5);
}

void image_mover::on_pushButtonSelectImageFolder_clicked()
{
    select_folder(ui->lineEditImageFolder);
}

void image_mover::set_number()
{
    ui->labelImageNumber->setText(QString("%1/%2").arg(image_index_ + 1).arg(images_urls_.size()));
}

void image_mover::on_pushButtonLoadImages_clicked()
{
    if(!QDir(ui->lineEditImageFolder->text()).exists()){
        QMessageBox::warning(this, tr("image_clean_up_tool"), tr("Image folder do not exist"));
        return;
    }

    auto const iterate_flag = ui->checkBoxRecursive->isChecked() ?
                QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    QDirIterator dir_it(ui->lineEditImageFolder->text(), QStringList()<<"*.jpg"<<"*.png"<<"*.bmp"<<"*.tiff",
                        QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs, iterate_flag);

    images_urls_.clear();
    while(dir_it.hasNext()){
        images_urls_.emplace_back(QFileInfo(dir_it.next()).absoluteFilePath());
    }
    qDebug()<<__func__<<": image size = "<<images_urls_.size();
    image_index_ = 0;
    set_number();
    show_image();
}

void image_mover::show_image()
{
    if((image_index_ + 1) <= images_urls_.size()){
        auto const url = images_urls_[image_index_];
        QFile file(url);
        if(file.open(QIODevice::ReadOnly)){
            auto buffer = file.readAll();
            auto cimg = cv::imdecode(cv::Mat(1, buffer.size(), CV_8U, buffer.data()), cv::IMREAD_COLOR);
            if(!cimg.empty()){
                if(cimg.cols > 640){
                    cv::resize(cimg, cimg, cv::Size(640, static_cast<int>(640.0/cimg.cols * cimg.rows)));
                }
                cv::cvtColor(cimg, cimg, cv::COLOR_BGR2RGB);
                ui->labelImage->setPixmap(QPixmap::fromImage(QImage(cimg.data, cimg.cols, cimg.rows,
                                                                    static_cast<int>(cimg.step[0]),
                                                             QImage::Format_RGB888).copy()));
            }else{
                QMessageBox::warning(this, tr("image_clean_up_tool"), tr("Cannot read image %1").arg(url));
            }
        }else{
            QMessageBox::warning(this, tr("image_clean_up_tool"), tr("Cannot read image %1").arg(url));
        }
    }
    set_number();
}

void image_mover::on_pushButtonPrev_clicked()
{
    if(image_index_ > 0){
        --image_index_;
        show_image();
    }
}

void image_mover::on_pushButtonNext_clicked()
{
    if(image_index_ < images_urls_.size()){
        ++image_index_;
        show_image();
    }
}

void image_mover::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<__func__<<event->key();
    if(event->key() == Qt::Key_1){
        move_file(ui->lineEdit_1->text());
    }else if(event->key() == Qt::Key_2){
        move_file(ui->lineEdit_2->text());
    }else if(event->key() == Qt::Key_3){
        move_file(ui->lineEdit_3->text());
    }else if(event->key() == Qt::Key_4){
        move_file(ui->lineEdit_4->text());
    }else if(event->key() == Qt::Key_5){
        move_file(ui->lineEdit_5->text());
    }else if(event->key() == Qt::Key_Left){
        on_pushButtonPrev_clicked();
    }else if(event->key() == Qt::Key_Right){
        on_pushButtonNext_clicked();
    }else{
        QWidget::keyPressEvent(event);
    }
}

void image_mover::move_file(const QString &target_dir)
{
    QDir dir;
    auto const success = dir.rename(images_urls_[image_index_],
                                    target_dir + "/" + QFileInfo(images_urls_[image_index_]).fileName());
    if(success){
        images_urls_.erase(images_urls_.begin() + static_cast<int>(image_index_));
        show_image();
    }else{
        QMessageBox::warning(this, tr("image_clean_up_tool"),
                             tr("Cannot show image %1").arg(images_urls_[image_index_]));
    }
}

void image_mover::select_folder(QLineEdit *editor)
{
    auto const dir =
            QFileDialog::getExistingDirectory(this, tr("Select folder to move the file"), editor->text());
    editor->setText(dir);
}
