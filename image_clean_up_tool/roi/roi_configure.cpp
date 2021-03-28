#include "roi_configure.hpp"
#include "ui_roi_configure.h"

#include "custom_graphics_view_pixmap.hpp"
#include "roi_configure_data_creator.hpp"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QJsonObject>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>

namespace{

QString const state_img_height("state_img_height");
QString const state_img_width("state_img_width");
QString const state_roi_creator("state_roi_creator");

}

roi_configure::roi_configure(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::roi_configure),
    pixmap_item_(new custom_graphics_view_pixmap),
    scene_(new QGraphicsScene(this))
{
    ui->setupUi(this);

    ui->graphicsView->setScene(scene_);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    scene_->addItem(pixmap_item_);

    roi_creator_ = new roi_configure_data_creator(*pixmap_item_, *scene_, this);
}

roi_configure::~roi_configure()
{
    delete ui;
}

QSize roi_configure::get_image_size() const noexcept
{
    return img_size_;
}

QPolygonF roi_configure::get_relative_polygon() const
{
    return roi_creator_->get_relative_polygon();
}

QRectF roi_configure::get_relative_roi() const
{
    return roi_creator_->get_relative_roi();
}

QJsonObject roi_configure::get_states() const
{
    QJsonObject obj;
    obj[state_img_height] = img_size_.height();
    obj[state_img_width] = img_size_.width();
    obj[state_roi_creator] = roi_creator_->get_states();

    return obj;
}

void roi_configure::set_qimage(QImage const &img)
{    
    if(!img.isNull()){
        auto const ratio = ui->graphicsView->width() / static_cast<double>(img.width());
        QPixmap pixmap(QPixmap::fromImage(img.scaled(ui->graphicsView->width(),
                                                     static_cast<int>(img.height() * ratio))));
        QSize const pixmap_size = pixmap.size();
        ui->graphicsView->setSceneRect(QRect(0, 0, pixmap_size.width(), pixmap_size.height()));
        ui->graphicsView->fitInView(ui->graphicsView->sceneRect());
        pixmap_item_->setPixmap(pixmap);
        create_rubber_band();
        restore_roi_on_image();
    }else{
        qDebug()<<__func__<<"image is null";
    }
}

void roi_configure::set_states(const QJsonObject &val)
{
    if(val.contains(state_img_height)){
        img_size_.setHeight(val[state_img_height].toInt());
    }
    if(val.contains(state_img_width)){
        img_size_.setWidth(val[state_img_width].toInt());
    }
}

void roi_configure::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        ui->retranslateUi(this);
    }else{
        QWidget::changeEvent(event);
    }
}

void roi_configure::create_rubber_band()
{
    if(roi_creator_->get_roi_size() == 0){
        auto const height_of_rband =
                100 > ui->graphicsView->height() / 4 ? 100 : ui->graphicsView->height() / 4;
        roi_creator_->create_rubber_band_rect(QRectF(0, 0, ui->graphicsView->width(), height_of_rband));
    }
}

void roi_configure::restore_roi_on_image()
{
    //when you restore roi from json, the image of the custom_graphics_view_pixmap may not
    //resize to appropriate size yet, so I use 200 pixels as the threshold to detect this case
    if(!pixmap_item_->pixmap().isNull() && pixmap_item_->pixmap().width() > 200){
        //restore_rect_or_polygon_image(pixmap_item_->pixmap().size());
        roi_creator_->rescaled_roi();
    }
}
