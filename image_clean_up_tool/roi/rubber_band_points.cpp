#include "rubber_band_points.hpp"

#include <QCursor>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QPainter>

namespace{

inline
int constexpr pen_width()
{
    return 4;
}

}

namespace eroi{

rubber_band_points::rubber_band_points(QGraphicsItem *parent) :
    rubber_band_points({}, parent)
{
}

rubber_band_points::rubber_band_points(QPolygonF polygon, QGraphicsItem *parent) :
    rubber_band_base(parent),
    fill_color_(QColor(0,0,0,0)),
    polygon_color_(QColor(204,102,0)),
    scale_(1.0)
{
    brect_pen_.setWidth(pen_width());
    brect_pen_.setStyle(Qt::DashDotDotLine);
    brect_pen_.setColor(QColor(255, 255, 0));

    polygon_pen_.setWidth(pen_width());
    polygon_pen_.setStyle(Qt::SolidLine);
    polygon_pen_.setColor(polygon_color_);

    polygon_.swap(polygon);
}

void rubber_band_points::add_point(const QPointF &pt)
{
    QRectF const brect = parentItem()->boundingRect();
    if(pt.x() >= 0 && pt.x() < brect.width() && pt.y() >= 0 && pt.y() < brect.height()){
        polygon_.append(pt);
    }
}

QRectF rubber_band_points::boundingRect() const
{
    return polygon_.boundingRect();
}

void rubber_band_points::clear_points()
{
    polygon_.clear();
    update();
}

void rubber_band_points::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(polygon_pen_);
    painter->setBrush(fill_color_);
    for(int i = 0; i != polygon_.size(); ++i){
        painter->drawPoint(polygon_[i]);
    }
}

QPolygonF rubber_band_points::get_polygon() const
{
    QPolygonF result;
    for(auto &pt : polygon_){
        QPointF const adjust_corner((pt.x() + x()) * scale_,
                                    (pt.y() + y()) * scale_);
        result.append(adjust_corner);
    }

    return result;
}

QRectF rubber_band_points::get_roi() const noexcept
{
    return polygon_.boundingRect();
}

bool rubber_band_points::is_select() const noexcept
{
    return fill_color_.color() == QColor(0,255,0,125);
}

void rubber_band_points::pop_point()
{
    polygon_.pop_back();
}

void rubber_band_points::scale_down()
{
    if(scale_ > 0.25){
        scale_ -= 0.125;
        setScale(scale_);
    }
    back_to_parent_region(boundingRect().topLeft());
}

void rubber_band_points::scale_to_normal()
{
    scale_ = 1.0;
    setScale(scale_);
    if(should_be_clipped()){
        scale_ -= 0.125;
        setScale(scale_);
    }
    back_to_parent_region(boundingRect().topLeft());
}

void rubber_band_points::scale_up()
{
    scale_ += 0.125;
    setScale(scale_);
    if(should_be_clipped()){
        scale_ -= 0.125;
        setScale(scale_);
    }
    back_to_parent_region(boundingRect().topLeft());
}

void rubber_band_points::select()
{
    fill_color_.setColor(QColor(0,255,0,125));
}

int rubber_band_points::type() const
{
    return rubber_band_points::Type;
}

void rubber_band_points::unselect()
{
    fill_color_.setColor(QColor(0,0,0,0));
}

bool rubber_band_points::back_to_parent_region(const QPointF &item_corner)
{
    //item_corner is the position of the mouse on bounding rect coordinate
    //parent_pos is the position of the mouse on image coordinate
    /**
      * ToDo : Make this function shorter
      */
    QPointF const parent_pos = mapToParent(item_corner);
    QRectF const parent_brect = parentItem()->boundingRect();
    QSizeF const brect_size = boundingRect().size() * scale_;
    QPointF const brect_tl = boundingRect().topLeft() * scale_;
    QPointF const adjust_corner(item_corner.x() * scale_ - brect_tl.x(), item_corner.y() * scale_ - brect_tl.y());

    qDebug()<<__func__<<": adjust_corder vs parent_pos == "<<adjust_corner<<","<<parent_pos<<",scale =="<<scale()
              <<", brect  == "<<boundingRect()<<", item_corner == "<<item_corner;

    bool const bigger_than_scene_width = (parent_pos.x() + (brect_size.width() - adjust_corner.x())) >
            parent_brect.width();
    bool const bigger_than_scene_height = (parent_pos.y() + (brect_size.height() - adjust_corner.y())) >
            parent_brect.height();
    bool const is_negative_x = ((parent_pos.x() - adjust_corner.x()) < 0) || parent_pos.x() < 0;
    bool const is_negative_y = ((parent_pos.y() - adjust_corner.y()) < 0) || parent_pos.y() < 0;

    bool const top_left_corner = is_negative_x && is_negative_y;
    bool const top_right_corner = is_negative_y && bigger_than_scene_width;
    bool const bottom_left_corner = is_negative_x && bigger_than_scene_height;
    bool const bottom_right_corner = bigger_than_scene_height && bigger_than_scene_width;

    if(top_left_corner){
        qDebug()<<__func__<<":top_left_corner_case";
        setPos(pen_width() - brect_tl.x(), pen_width() - brect_tl.y());
        return true;
    }else if(bottom_left_corner){
        qDebug()<<__func__<<":bottom_left_corner_case";
        setPos(pen_width() - brect_tl.x(),
               parent_brect.height() - brect_size.height() - pen_width() - brect_tl.y());
        return true;
    }else if(bottom_right_corner){
        qDebug()<<__func__<<":bottom_right_corner_case";
        setPos(parent_brect.width() - brect_size.width() - pen_width() - brect_tl.x(),
               parent_brect.height() - brect_size.height() - pen_width() - brect_tl.y());
        return true;
    }else if(top_right_corner){
        qDebug()<<__func__<<":top_right_corner_case";
        setPos(parent_brect.width() - brect_size.width() - pen_width() - brect_tl.x(),
               pen_width() - brect_tl.y());
        return true;
    }else if(is_negative_x){
        qDebug()<<__func__<<":is_negative_x";
        setPos(pen_width() - brect_tl.x(),
               parent_pos.y() - adjust_corner.y() - brect_tl.y());
        return true;
    }else if(is_negative_y){
        qDebug()<<__func__<<":is_negative_y";
        setPos(parent_pos.x() - adjust_corner.x() - brect_tl.x(),
               pen_width() - brect_tl.y());
        return true;
    }else if(bigger_than_scene_height){
        qDebug()<<__func__<<":bigger_than_scene_height";
        setPos(parent_pos.x() - adjust_corner.x() - brect_tl.x(),
               parent_brect.height() - brect_size.height() - pen_width() - brect_tl.y());
        return true;
    }else if(bigger_than_scene_width){
        qDebug()<<__func__<<":bigger_than_scene_width";
        setPos(parent_brect.width() - brect_size.width() - pen_width() - brect_tl.x(),
               parent_pos.y() - adjust_corner.y() - brect_tl.y());
        return true;
    }

    return false;
}

void rubber_band_points::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    qDebug()<<__func__<<": delete roi menu";
    QMenu menu;
    QAction *delete_rubber_band = menu.addAction(QIcon(QStringLiteral(":/assets/icons/rubber_band_delete.png")),
                                                 tr("Reselect points"));
    connect(delete_rubber_band, &QAction::triggered, this, &rubber_band_base::delete_rubber_band);
    menu.exec(event->screenPos());
}

bool rubber_band_points::should_be_clipped() noexcept
{
    QRectF const parent_brect = parentItem()->boundingRect();
    qDebug()<<__func__<<"boundingRect().width() * scale_ == "<<boundingRect().width() * scale_;
    qDebug()<<__func__<<"boundingRect().height() * scale_ == "<<boundingRect().height() * scale_;
    if((boundingRect().width() * scale_ + pen_width()) > parent_brect.width()){
        return true;
    }
    if((boundingRect().height() * scale_ + pen_width()) > parent_brect.height()){
        return true;
    }

    return false;
}

void rubber_band_points::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{    
    back_to_parent_region(event->pos());
    emit mouse_release(this);
    QGraphicsObject::mouseReleaseEvent(event);
}

}
