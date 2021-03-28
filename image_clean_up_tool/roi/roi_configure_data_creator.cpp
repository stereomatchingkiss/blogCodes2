#include "roi_configure_data_creator.hpp"

#include "custom_graphics_view_pixmap.hpp"
#include "rubber_band_points.hpp"
#include "rubber_band_rect.hpp"

#include "config/config_utils.hpp"

#include <QGraphicsScene>

#include <QJsonArray>
#include <QJsonObject>

using namespace eroi;

namespace{

QString const state_polygon("state_polygon");
QString const state_rect("state_rect");
QString const state_roi_type("state_roi_type");

}

roi_configure_data_creator::roi_configure_data_creator(custom_graphics_view_pixmap &pixmap,
                                                       QGraphicsScene &scene,
                                                       QObject *parent) :
    QObject(parent),
    pixmap_item_(pixmap),
    scene_(scene)
{

}

roi_configure_data_creator::~roi_configure_data_creator()
{

}

void roi_configure_data_creator::add_point_to_polygon(const QPointF &pt)
{
    qDebug()<<__func__;
    if(rubber_band_ == nullptr || qobject_cast<rubber_band_rect*>(rubber_band_) != nullptr){
        qDebug()<<__func__;
        release();
        qDebug()<<__func__;
        rubber_band_ = new rubber_band_points;
        rubber_band_->setParentItem(&pixmap_item_);
        connect(rubber_band_, &rubber_band_base::delete_rubber_band, this, &roi_configure_data_creator::release);
    }

    qDebug()<<__func__;
    auto *rband = qobject_cast<rubber_band_points*>(rubber_band_);
    qDebug()<<__func__;
    rband->add_point(pt);
}

QRectF roi_configure_data_creator::get_relative_roi() const
{
    if(!rubber_band_){
        return {};
    }

    QRectF const roi = rubber_band_->get_roi();
    auto const img_size = pixmap_item_.pixmap().size();
    auto const rroi = scaled_roi(roi, img_size);
    qDebug()<<__func__<<": origin roi = "<<roi<<", img size = "<<img_size<<", relative roi = "<<rroi;

    return rroi;
}

int roi_configure_data_creator::get_roi_size() const noexcept
{
    return rubber_band_ != nullptr ? 1 : 0;
}

QJsonObject roi_configure_data_creator::get_states() const
{
    QJsonObject obj;
    if(rubber_band_){
        auto const rtype = qobject_cast<rubber_band_points*>(rubber_band_) != nullptr ?
                    roi_type::polygon : roi_type::rect;
        obj[state_roi_type] = static_cast<int>(rtype);
        if(rtype == roi_type::rect){
            obj[state_rect] = qrectf_to_qjson_array(get_relative_roi());
        }else{
            if(auto *rband = qobject_cast<rubber_band_points*>(rubber_band_)){
                obj[state_polygon] = qpolygon_to_qjson_array(get_relative_polygon());
            }
        }
    }

    return obj;
}

void roi_configure_data_creator::set_enable_context_menu(bool value)
{
    if(rubber_band_){
        rubber_band_->set_enable_context_menu(value);
    }
}

void roi_configure_data_creator::set_states(const QJsonObject &val)
{
    if(val.contains(state_roi_type)){
        auto const rtype = static_cast<roi_type>(val[state_roi_type].toInt());
        if(rtype == roi_type::rect && val.contains(state_rect)){
            first_init_roi_ = qjson_array_to_qrectf(val[state_rect].toArray());
            create_rubber_band_rect(scaled_roi(first_init_roi_,
                                               pixmap_item_.pixmap().size(), scaled_mode::multiply));
        }else if(val.contains(state_polygon)){
            first_init_polygon_ = qjson_array_to_qpolygonf(val[state_polygon].toArray());
            auto const im_size = pixmap_item_.pixmap().size();
            for(auto const &val : first_init_polygon_){
                QPointF const pt(val.x() * im_size.width(), val.y() * im_size.height());
                add_point_to_polygon(pt);
            }
        }
    }
}

void roi_configure_data_creator::create_rubber_band_rect(const QRectF &rect)
{
    release();
    qDebug()<<__func__<<": rect = "<<rect;
    rubber_band_ = create_new_rubber_band(rect, roi_type::rect);
    set_enable_context_menu(true);
}

QPolygonF roi_configure_data_creator::get_relative_polygon() const
{
    if(auto *rband = qobject_cast<rubber_band_points*>(rubber_band_)){
        auto results = rband->get_polygon();
        auto const im_size = pixmap_item_.pixmap().size();
        for(auto &pt : results){
            pt = QPointF(pt.x() / im_size.width(), pt.y() / im_size.height());
        }
        return results;
    }

    return {};
}

void roi_configure_data_creator::release()
{
    if(rubber_band_){
        scene_.removeItem(rubber_band_);
        rubber_band_ = nullptr;
        emit rubber_band_released();
    }
}

void roi_configure_data_creator::rescaled_roi()
{
    if(!first_init_roi_.isNull()){
        if(auto *rband = qobject_cast<rubber_band_rect*>(rubber_band_)){
            rband->set_bounding_rect(scaled_roi(first_init_roi_, pixmap_item_.pixmap().size(), scaled_mode::multiply));
            first_init_roi_ = QRectF();
        }
    }
    if(!first_init_polygon_.isEmpty()){
        if(auto *rband = qobject_cast<rubber_band_points*>(rubber_band_)){
            rband->clear_points();
             auto const img_size = pixmap_item_.pixmap().size();
             for(auto &pt : first_init_polygon_){
                 pt.setX(pt.x() * img_size.width());
                 pt.setY(pt.y() * img_size.height());
                 add_point_to_polygon(pt);
             }
             first_init_polygon_.clear();
        }
    }
}

rubber_band_base *roi_configure_data_creator::create_new_rubber_band(const QRectF &rect, roi_type shape)
{
    rubber_band_rect *rband = nullptr;
    if(shape == roi_type::rect){
        rband = new eroi::rubber_band_rect(eroi::rect_shape::rect, rect);
        rband->setParentItem(&pixmap_item_);
        connect(rband, &rubber_band_base::delete_rubber_band, this, &roi_configure_data_creator::release);
        connect(rband, static_cast<void(rubber_band_base::*)()>(&rubber_band_base::mouse_press),
                this, &roi_configure_data_creator::rubber_band_rect_mouse_press);
        connect(rband, &rubber_band_base::mouse_release,
                this, &roi_configure_data_creator::rubber_band_rect_mouse_release);
    }

    return rband;
}

QRectF roi_configure_data_creator::scaled_roi(const QRectF &roi,
                                              const QSize &img_size,
                                              scaled_mode mode) const noexcept
{
    if(mode == scaled_mode::divide){
        return QRectF(roi.x() / img_size.width(),
                      roi.y() / img_size.height(),
                      roi.width() / img_size.width(),
                      roi.height() / img_size.height());
    }else{
        return QRectF(roi.x() * img_size.width(),
                      roi.y() * img_size.height(),
                      roi.width() * img_size.width(),
                      roi.height() * img_size.height());
    }
}
