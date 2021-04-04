#include "mask_adjustment_widget.hpp"
#include "ui_mask_adjustment_widget.h"

#include "roi/custom_graphics_view_pixmap.hpp"

#include "utils/iterator_create.hpp"
#include "utils/utility.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QImageReader>
#include <QKeyEvent>
#include <QImage>
#include <QMessageBox>
#include <QSettings>
#include <QUuid>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <execution>

namespace{

QString const state_image_folder_img("state_image_folder_img");
QString const state_image_folder_mask("state_image_folder_mask");
QString const state_move_image_to("state_move_image_to");
QString const state_move_mask_to("state_move_mask_to");
QString const state_save_at("state_save_at");
QString const state_scene_path("state_scene_path");

}

mask_adjustment_widget::mask_adjustment_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mask_adjustment_widget)
{
    ui->setupUi(this);

    QSettings settings("image_clean_up_tool", "mask_adjustment");
    if(settings.contains(state_image_folder_img)){
        ui->lineEditImageFolder->setText(settings.value(state_image_folder_img).toString());
    }
    if(settings.contains(state_image_folder_mask)){
        ui->lineEditMaskFolder->setText(settings.value(state_image_folder_mask).toString());
    }
    if(settings.contains(state_move_image_to)){
        ui->lineEditMoveImageTo->setText(settings.value(state_move_image_to).toString());
    }
    if(settings.contains(state_move_mask_to)){
        ui->lineEditMoveMaskTo->setText(settings.value(state_move_mask_to).toString());
    }
    if(settings.contains(state_save_at)){
        ui->lineEditSaveAt->setText(settings.value(state_save_at).toString());
    }
    if(settings.contains(state_scene_path)){
        ui->lineEditScenePath->setText(settings.value(state_scene_path).toString());
    }
}

mask_adjustment_widget::~mask_adjustment_widget()
{
    QSettings settings("image_clean_up_tool", "mask_adjustment");
    settings.setValue(state_image_folder_img, ui->lineEditImageFolder->text());
    settings.setValue(state_image_folder_mask, ui->lineEditMaskFolder->text());
    settings.setValue(state_move_image_to, ui->lineEditMoveImageTo->text());
    settings.setValue(state_move_mask_to, ui->lineEditMoveMaskTo->text());
    settings.setValue(state_save_at, ui->lineEditSaveAt->text());
    settings.setValue(state_scene_path, ui->lineEditScenePath->text());

    delete ui;
}

void mask_adjustment_widget::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<__func__<<ui->widgetMask->size();
    qDebug()<<__func__<<": key == "<<event->key();
    if(event->key() == Qt::Key_D){
        on_pushButtonToZero_clicked();
    }else if(event->key() == Qt::Key_S){
        on_pushButtonSave_clicked();
    }else if(event->key() == Qt::Key_Space){
        on_pushButtonNext_clicked();
    }else if(event->key() == Qt::Key_A){
        on_pushButtonPrev_clicked();
    }else if(event->key() == Qt::Key_M){
        move_file();
    }else if(event->key() == Qt::Key_F){
        erosion();
        reload_image();
    }else if(event->key() == Qt::Key_G){
        dilation();
        reload_image();
    }else{
        QWidget::keyPressEvent(event);
    }
}

void mask_adjustment_widget::on_pushButtonLoadImages_clicked()
{
    load_images(0);
    show_image();
}

void mask_adjustment_widget::load_images(size_t image_index)
{
    image_index_ = image_index;
    images_urls_img_.clear();
    images_urls_mask_.clear();

    auto const iterate_flag = QDirIterator::Subdirectories;
    for(auto dir_it = create_image_iterator(ui->lineEditImageFolder->text(), iterate_flag);
        dir_it.hasNext();){
        images_urls_img_.emplace_back(QFileInfo(dir_it.next()).absoluteFilePath());
    }
    for(auto dir_two = create_image_iterator(ui->lineEditMaskFolder->text(), iterate_flag);
        dir_two.hasNext();){
        images_urls_mask_.emplace_back(QFileInfo(dir_two.next()).absoluteFilePath());
    }
    if(images_urls_img_.size() != images_urls_mask_.size()){
        QMessageBox::warning(this, tr("Mask adjustment"), tr("Sizes of mask and img must be equal"));
        return;
    }
    if(images_urls_img_.empty()){
        QMessageBox::warning(this, tr("Mask adjustment"), tr("Sizes of mask and img must not empty"));
        return;
    }

    qDebug()<<__func__<<": image size = "<<images_urls_img_.size()<<", mask size = "<<images_urls_mask_.size();
    ui->spinBoxToImage->setRange(0, static_cast<int>(images_urls_img_.size() - 1));
    qDebug()<<__func__<<" range of spinbox = "<<ui->spinBoxToImage->minimum()<<", "<<ui->spinBoxToImage->maximum();
}

void mask_adjustment_widget::move_file()
{
    QDir().rename(images_urls_img_[image_index_],
                  ui->lineEditMoveImageTo->text() + "/" + QFileInfo(images_urls_img_[image_index_]).fileName());
    QDir().rename(images_urls_mask_[image_index_],
                  ui->lineEditMoveMaskTo->text() + "/" + QFileInfo(images_urls_mask_[image_index_]).fileName());
    images_urls_img_.erase(std::begin(images_urls_img_) + static_cast<int>(image_index_));
    images_urls_mask_.erase(std::begin(images_urls_mask_) + static_cast<int>(image_index_));
    show_image();
}

void mask_adjustment_widget::reload_image()
{
    cscene_origin_.copyTo(cscene_);
    cscene_.setTo(0, cmask_ == 0);

    cv::addWeighted(cscene_, 0.0, cimg_origin_, 1.0, 0, cscene_);
    cscene_origin_.copyTo(cscene_, cmask_ == 0);
    cv::Mat cimg_copy;
    cv::cvtColor(cimg_origin_, cimg_copy, cv::COLOR_BGR2RGB);
    cv::cvtColor(cscene_, cscene_, cv::COLOR_BGR2RGB);

    cv::Mat mask_rgb;
    cv::cvtColor(cmask_, mask_rgb, cv::COLOR_GRAY2RGB);
    ui->widgetMask->set_qimage(QImage(cscene_.data, cscene_.cols, cscene_.rows,
                                      static_cast<int>(cscene_.step[0]),
                               QImage::Format_RGB888).copy());

    cimg_copy.setTo(0, cmask_ != 127);
    ui->labelImg->setPixmap(QPixmap::fromImage(QImage(cimg_copy.data, cimg_copy.cols, cimg_copy.rows,
                                                      static_cast<int>(cimg_copy.step[0]),
                                               QImage::Format_RGB888).copy()));
    ui->labelComposeImg->setPixmap(QPixmap::fromImage(QImage(mask_rgb.data, mask_rgb.cols, mask_rgb.rows,
                                                             static_cast<int>(mask_rgb.step[0]),
                                                      QImage::Format_RGB888).copy()));
    set_number();
}

void mask_adjustment_widget::set_number()
{
    ui->labelImageInfo->setText(QString("%1/%2").arg(image_index_).arg(images_urls_img_.size() - 1));
}

void mask_adjustment_widget::show_image()
{    
    auto const url = images_urls_img_[image_index_];
    cv::Size const size(256, 256);
    auto cimg = read_cv_img(images_urls_img_[image_index_], cv::INTER_NEAREST, size);
    auto cmask = read_cv_img(images_urls_mask_[image_index_], cv::INTER_NEAREST, size);
    cscene_ = read_cv_img(ui->lineEditScenePath->text(), cv::INTER_NEAREST, size);
    if(!cimg.empty() && !cmask.empty() && !cscene_.empty()){        
        cmask.setTo(127, cmask);
        cmask_ = cmask.clone();
        erosion();

        cscene_origin_ = cscene_.clone();
        qDebug()<<__LINE__<<","<<cscene_.cols<<", "<<cscene_.rows;
        cimg_origin_ = cimg.clone();
        ui->spinBoxToImage->setValue(static_cast<int>(image_index_));

        reload_image();
    }
}

void mask_adjustment_widget::on_pushButtonReloadImg_clicked()
{
    reload_image();
}

void mask_adjustment_widget::on_pushButtonToZero_clicked()
{
    auto const roi = ui->widgetMask->get_relative_roi();
    cv::Rect const croi(static_cast<int>(roi.x() * cmask_.cols),
                        static_cast<int>(roi.y() * cmask_.rows),
                        static_cast<int>(roi.width() * cmask_.cols),
                        static_cast<int>(roi.height() * cmask_.rows));
    cmask_(scale_coordinate(croi, cmask_.cols, cmask_.rows)).setTo(0);
    reload_image();
    qDebug()<<__func__<<roi;
}

void mask_adjustment_widget::on_pushButtonPrev_clicked()
{
    if(image_index_ > 0){
        --image_index_;
    }

    qDebug()<<__func__<<" im index = "<<image_index_;
    show_image();
}

void mask_adjustment_widget::on_pushButtonNext_clicked()
{
    if(image_index_ < (images_urls_img_.size() - 1)){
        ++image_index_;
    }
    qDebug()<<__func__<<" im index = "<<image_index_;
    show_image();
}

void mask_adjustment_widget::on_pushButtonSave_clicked()
{
    QFileInfo finfo(images_urls_img_[image_index_]);
    auto const url = (ui->lineEditSaveAt->text() + "/" + finfo.fileName());
    auto const saved = cv::imwrite((url).toStdString(), cmask_);
    qDebug()<<__func__<<":"<<url<<", saved success = "<<saved;
}

void mask_adjustment_widget::on_spinBoxToImage_valueChanged(int arg1)
{
    image_index_ = static_cast<size_t>(arg1);
    show_image();
}

void mask_adjustment_widget::dilation()
{
    int constexpr erosion_type = cv::MORPH_RECT;
    int constexpr erosion_size = 1;
    auto const element = cv::getStructuringElement(erosion_type,
                                                   cv::Size(2*erosion_size + 1, 2*erosion_size+1),
                                                   cv::Point(erosion_size, erosion_size));
    cv::dilate(cmask_, cmask_, element);
}

void mask_adjustment_widget::erosion()
{
    int constexpr erosion_type = cv::MORPH_RECT;
    int constexpr erosion_size = 1;
    auto const element = cv::getStructuringElement(erosion_type,
                                                   cv::Size(2*erosion_size + 1, 2*erosion_size+1),
                                                   cv::Point(erosion_size, erosion_size));
    cv::erode(cmask_, cmask_, element);
}
