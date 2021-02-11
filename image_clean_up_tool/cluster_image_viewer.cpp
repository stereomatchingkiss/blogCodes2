#include "cluster_image_viewer.hpp"
#include "ui_cluster_image_viewer.h"

#include <QImage>

cluster_image_viewer::cluster_image_viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cluster_image_viewer)
{
    ui->setupUi(this);
}

cluster_image_viewer::~cluster_image_viewer()
{
    delete ui;
}

void cluster_image_viewer::set_clustered_url(std::vector<std::vector<QString> > clustered_url)
{
    clustered_url_ = std::move(clustered_url);
    ui->spinBoxImageCluster->setRange(0, static_cast<int>(clustered_url_.size()) - 1);
    ui->spinBoxImageIndex->setRange(0, static_cast<int>(clustered_url_[0].size()) - 1);
    ui->spinBoxImageCluster->setValue(0);
    ui->spinBoxImageIndex->setValue(0);

    update_image();
}

void cluster_image_viewer::on_spinBoxImageIndex_valueChanged(int)
{
    update_image();
}

void cluster_image_viewer::on_spinBoxImageCluster_valueChanged(int arg1)
{
    auto const maximum = std::max(1, static_cast<int>(clustered_url_[static_cast<size_t>(arg1)].size() - 1));
    ui->spinBoxImageIndex->setRange(0, std::max(1, maximum));
    update_image();
}

void cluster_image_viewer::update_image()
{
    auto const cluster_id = static_cast<size_t>(ui->spinBoxImageCluster->value());
    QImage img(clustered_url_[cluster_id][static_cast<size_t>(ui->spinBoxImageIndex->value())]);
    if(!img.isNull()){
        if(img.width() > img.height()){
            if(img.width() > 640){
                img = img.scaledToWidth(640);
            }
        }else{
            if(img.height() > 480){
                img = img.scaledToHeight(480);
            }
        }
        ui->labelImage->setPixmap(QPixmap::fromImage(img));
    }
}
