#include "widget_open_image_viewer.hpp"
#include "ui_widget_open_image_viewer.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <QPainter>
#include <QPen>

widget_open_image_viewer::widget_open_image_viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget_open_image_viewer)
{
    ui->setupUi(this);

    ui->lineEditFolder->setText("C:/Users/pc/fiftyone/open-images-v7/validation");
    ui->spinBoxSelectImage->setMaximum(0);
}

widget_open_image_viewer::~widget_open_image_viewer()
{
    delete ui;
}

void widget_open_image_viewer::on_pushButtonLoad_clicked()
{
    load_key_to_labels();
    load_images_path();
    if(QFile file(ui->lineEditFolder->text() + "/labels/detections.csv"); file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);
        QString line;
        line = stream.readLine();
        while(!stream.atEnd()){
            line = stream.readLine();
            auto const sline = line.split(",");
            if(images_base_name_.contains(sline[0])){
                if(auto it = detector_info_list_.find(sline[0]); it != std::end(detector_info_list_)){
                    it->second.emplace_back(strs_to_detection_info(sline));
                }else{
                    std::vector<detection_info> div;
                    div.emplace_back(strs_to_detection_info(sline));
                    detector_info_list_.emplace(sline[0], std::move(div));
                }
            }
        }
    }
    qDebug()<<"detector_info_list_ size = "<<detector_info_list_.size();

    if(!detector_info_list_.empty()){
        ui->spinBoxSelectImage->setMaximum(static_cast<int>(detector_info_list_.size() - 1));
        on_spinBoxSelectImage_valueChanged(0);
    }
}

void widget_open_image_viewer::load_images_path()
{
    images_base_name_.clear();
    if(QDir dir(ui->lineEditFolder->text() + "/data"); dir.exists()){
        auto im_paths = dir.entryInfoList(QStringList()<<"*.jpg"<<"*.jpeg"<<"*.png"<<"*.bmp", QDir::NoDotAndDotDot | QDir::Files);
        for(auto const &val : im_paths){
            images_base_name_.emplace(val.baseName());
        }
        qDebug()<<"image size = "<<images_base_name_.size();
    }
}

void widget_open_image_viewer::load_key_to_labels()
{
    if(QFile file(ui->lineEditFolder->text() + "/metadata/classes.csv"); file.open(QIODevice::ReadOnly)){
        key_to_labels_.clear();
        QTextStream stream(&file);
        QString line;
        while(!stream.atEnd()){
            line = stream.readLine();
            auto const sline = line.split(",");
            if(sline.size() >= 2){
                key_to_labels_.emplace(sline[0], sline[1]);
                qDebug()<<sline;
            }
        }
    }
}

widget_open_image_viewer::detection_info widget_open_image_viewer::strs_to_detection_info(QStringList const &input) const
{
    detection_info di;
    di.label_ = input[2];
    di.xmin_ = input[4].toFloat();
    di.xmax_ = input[5].toFloat();
    di.ymin_ = input[6].toFloat();
    di.ymax_ = input[7].toFloat();

    return di;
}


void widget_open_image_viewer::on_spinBoxSelectImage_valueChanged(int arg1)
{
    auto begin = images_base_name_.begin();
    std::advance(begin, arg1);

    auto const im_path = ui->lineEditFolder->text() + "/data/" + *begin + ".jpg";
    if(QImage img(im_path); !img.isNull()){
        draw_detection_info(img, im_path);
        ui->labelImage->setPixmap(QPixmap::fromImage(img.scaledToWidth(800)));
    }
}

void widget_open_image_viewer::draw_detection_info(QImage &img, QString const &im_path) const
{
    QPainter painter(&img);
    QPen rect_pen;
    rect_pen.setWidth(3);
    rect_pen.setColor(Qt::red);

    QPen text_pen;
    text_pen.setWidth(12);
    text_pen.setColor(Qt::blue);
    auto const bname = QFileInfo(im_path).baseName();
    if(auto it = detector_info_list_.find(bname); it != std::end(detector_info_list_)){
        auto const &info_vec = it->second;
        for(auto const &val : info_vec){
            QRectF rect;
            rect.setX(val.xmin_ * img.width());
            rect.setY(val.ymin_ * img.height());
            rect.setBottomRight(QPointF(val.xmax_ * img.width(), val.ymax_ * img.height()));
            painter.setPen(rect_pen);
            painter.drawRect(rect);

            painter.setPen(text_pen);
            if(auto kit = key_to_labels_.find(val.label_); kit != std::end(key_to_labels_)){
                painter.drawText(rect.topLeft(), kit->second);
            }
        }
    }
}

