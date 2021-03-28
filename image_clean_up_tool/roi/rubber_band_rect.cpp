#include "rubber_band_rect.hpp"

#include <QAction>
#include <QCursor>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QPointF>

namespace eroi{

namespace{

inline
bool fuzzy_equal(qreal a, qreal b)
{
    return std::abs(a - b) < 0.0001;
}

inline
int constexpr pen_width()
{
    return 4;
}

QRect to_qrect(QRectF const &input)
{
    return QRect(static_cast<int>(input.x()), static_cast<int>(input.y()),
                 static_cast<int>(input.width()), static_cast<int>(input.height()));
}

}

rubber_band_rect::rubber_band_rect(eroi::rect_shape shape,
                                   QRectF const &rect,
                                   QColor const &color,
                                   QGraphicsItem *parent) :
    rubber_band_base(parent),
    corner_place_(corner_place::bottom_right),
    fill_color_(QColor(0,0,0,0)),
    rband_color_(color),
    rect_(0,0,rect.width(),rect.height()),
    shape_(shape)
{
    rect_.setX(0);
    rect_.setY(0);
    rect_.setWidth(rect.width());
    rect_.setHeight(rect.height());
    setPos(rect.x(), rect.y());
    setCursor(QCursor(Qt::SizeFDiagCursor));
}

QRectF rubber_band_rect::boundingRect() const
{
    return rect_;
}

QRectF rubber_band_rect::get_roi() const noexcept
{
    QPointF const pos = mapToScene(rect_.topLeft());
    return {pos.x(), pos.y(), rect_.width(), rect_.height()};
}

rect_shape rubber_band_rect::get_shape() const noexcept
{
    return shape_;
}

bool rubber_band_rect::is_select() const noexcept
{
    return fill_color_.color() == QColor(0,255,0,125);
}

void rubber_band_rect::select()
{
    fill_color_.setColor(QColor(0,255,0,125));
}

void rubber_band_rect::set_bounding_rect(const QRectF &rect)
{
    rect_.setX(0);
    rect_.setY(0);
    rect_.setWidth(rect.width());
    rect_.setHeight(rect.height());
    setPos(QPointF(rect.x(), rect.y()));
    clipped_if_too_big();
}

void rubber_band_rect::set_enable_context_menu(bool val)
{
    enable_context_menu_ = val;
}

int rubber_band_rect::type() const
{
    return rubber_band_rect::Type;
}

void rubber_band_rect::unselect()
{
    fill_color_.setColor(QColor(0,0,0,0));
}

void rubber_band_rect::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(!is_select()){
        fill_color_.setColor(QColor(0,0,255,125));
    }
    corner_place_ = process_hover_event(event);
    update();
}

void rubber_band_rect::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!is_select()){
        fill_color_.setColor(QColor(0,0,0,0));
    }
    rubber_band_base::hoverLeaveEvent(event);
    update();
}

void rubber_band_rect::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!is_select()){
        fill_color_.setColor(QColor(0,0,255,125));
    }
    corner_place_ = process_hover_event(event);
    update();
}

bool rubber_band_rect::clipped_if_too_big() noexcept
{
    QRectF const parent_brect = parentItem()->boundingRect();
    bool clipped = false;
    if((rect_.width() + pen_width()) > parent_brect.width()){
        rect_.setWidth(parent_brect.width() - pen_width() * 2);
        clipped = true;
    }
    if((rect_.height() + pen_width()) > parent_brect.height()){
        rect_.setHeight(parent_brect.height() - pen_width() * 2);
        clipped = true;
    }
    if(clipped){
        prepareGeometryChange();
        update();
    }

    return clipped;
}

void rubber_band_rect::resize_brect(QPointF const &top_left, QPointF const &bottom_right)
{
    QRectF const temp = QRectF(top_left, bottom_right);
    if(temp.width() >= 25 && temp.height() >= 25){
        rect_ = QRectF(top_left, bottom_right);
        qDebug()<<__func__<<rect_;
        prepareGeometryChange();
        update();
    }
}

void rubber_band_rect::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(event && event->buttons() == Qt::LeftButton){
        QPointF const current_bottom_right = rect_.bottomRight();
        QPointF const current_top_left = rect_.topLeft();
        QPointF const event_pos = event->pos();
        if(corner_place_ == corner_place::bottom_right){
            resize_brect(current_top_left, event_pos);
        }else if(corner_place_ == corner_place::top_left){
            resize_brect(event_pos, current_bottom_right);
        }else if(corner_place_ == corner_place::top_right){
            resize_brect(QPointF(current_top_left.x(), event_pos.y()), QPointF(event_pos.x(), current_bottom_right.y()));
        }else if(corner_place_ == corner_place::bottom_left){
            resize_brect(QPointF(event_pos.x(), current_top_left.y()), QPointF(current_bottom_right.x(), event_pos.y()));
        }else{
            QGraphicsObject::mouseMoveEvent(event);
        }
    }else{
        QGraphicsObject::mouseMoveEvent(event);
    }
}

void rubber_band_rect::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    select();
    emit mouse_press();
    rubber_band_base::mousePressEvent(event);
}

void rubber_band_rect::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    clipped_if_too_big();
    back_to_parent_region(event->pos());
    emit mouse_release(this);
    corner_place_ = corner_place::other;
    QGraphicsObject::mouseReleaseEvent(event);
}

void rubber_band_rect::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPen pen;
    pen.setWidth(pen_width());
    pen.setStyle(Qt::DashDotDotLine);
    pen.setColor(rband_color_);
    painter->setPen(pen);
    painter->setBrush(QBrush(fill_color_));

    if(rect_.height() > 10 && rect_.width() > 10){
        painter->drawRect(top_left_corner());
        painter->drawRect(top_right_corner());
        painter->drawRect(bottom_right_corner());
        painter->drawRect(bottom_left_corner());
    }
    if(shape_ == eroi::rect_shape::rect){
        painter->drawRect(rect_);
    }else if(shape_ == eroi::rect_shape::horizontal_region){
        QRect const rect = to_qrect(rect_);
        painter->drawLine(QLine(rect.x(), rect.y(),
                                rect.x() + rect.width(), rect.y()));
        painter->drawLine(QLine(rect.x(), rect.y() + rect.height(),
                                rect.x() + rect.width(), rect.y() + rect.height()));
    }else if(shape_ == eroi::rect_shape::vertical_region){
        QRect const rect = to_qrect(rect_);
        painter->drawLine(QLine(rect.x(), rect.y(),
                                rect.x(), rect.y() + rect.height()));
        painter->drawLine(QLine(rect.x() + rect.width(), rect.y(),
                                rect.x() + rect.width(), rect.y() + rect.height()));
    }
}

bool rubber_band_rect::back_to_parent_region(QPointF const &event_pos)
{    
    //event pos is the position of the mouse on bounding rect coordinate
    //parent pos is the position of the mouse on image coordinate
    //todo : Make this function shorter
    QPointF const parent_pos = mapToParent(event_pos);
    qDebug()<<__func__<<": event pos vs parent pos == "<<event_pos<<","<<parent_pos;
    bool const is_negative_x = ((parent_pos.x() - event_pos.x() + rect_.x()) < 0) || parent_pos.x() < 0;
    bool const is_negative_y = ((parent_pos.y() - event_pos.y() + rect_.y()) < 0) || parent_pos.y() < 0;
    QRectF const parent_brect = parentItem()->boundingRect();
    bool const bigger_than_scene_width =
            (parent_pos.x() + rect_.x() + (rect_.width() - event_pos.x())) > parent_brect.width();
    bool const bigger_than_scene_height =
            (parent_pos.y() + rect_.y() + (rect_.height() - event_pos.y())) > parent_brect.height();

    bool const top_left_corner = is_negative_x && is_negative_y;
    bool const top_right_corner = is_negative_y && bigger_than_scene_width;
    bool const bottom_left_corner = is_negative_x && bigger_than_scene_height;
    bool const bottom_right_corner = bigger_than_scene_height && bigger_than_scene_width;

    if(top_left_corner){
        qDebug()<<__func__<<":top_left_corner_case";
        setPos(pen_width() - rect_.x(), pen_width() - rect_.y());
        return true;
    }else if(bottom_left_corner){
        qDebug()<<__func__<<":bottom_left_corner_case";
        setPos(pen_width() - rect_.x(), parent_brect.height() - rect_.y() - rect_.height() - pen_width());
        return true;
    }else if(bottom_right_corner){
        qDebug()<<__func__<<":bottom_right_corner_case";
        setPos(parent_brect.width() - rect_.x() - rect_.width() - pen_width(),
               parent_brect.height() - rect_.y() - rect_.height() - pen_width());
        return true;
    }else if(top_right_corner){
        qDebug()<<__func__<<":top_right_corner_case";
        setPos(parent_brect.width() - rect_.x() - rect_.width() - pen_width(),
               pen_width() - rect_.y());
        return true;
    }else if(is_negative_x){
        qDebug()<<__func__<<":is_negative_x:"<<rect_;
        setPos(pen_width() - rect_.x(), parent_pos.y() - event_pos.y());
        return true;
    }else if(is_negative_y){
        qDebug()<<__func__<<":is_negative_y";
        setPos(parent_pos.x() - event_pos.x(), pen_width() - rect_.y());
        return true;
    }else if(bigger_than_scene_height){
        qDebug()<<__func__<<":bigger_than_scene_height";
        setPos(parent_pos.x() - event_pos.x(),
               parent_brect.height() - rect_.y() - rect_.height() - pen_width());
        return true;
    }else if(bigger_than_scene_width){
        qDebug()<<__func__<<":bigger_than_scene_width";
        setPos(parent_brect.width() - rect_.x() - rect_.width() - pen_width(),
               parent_pos.y() - event_pos.y());
        return true;
    }

    return false;
}

void rubber_band_rect::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    qDebug()<<__func__<<": delete roi";
    if(enable_context_menu_){
        qDebug()<<__func__<<": delete roi menu";
        QMenu menu;
        QAction *delete_rubber_band = menu.addAction(QIcon(QStringLiteral(":/assets/icons/rubber_band_delete.png")),
                                                     tr("Delete region of interest"));
        connect(delete_rubber_band, &QAction::triggered, this, &rubber_band_base::delete_rubber_band);
        menu.exec(event->screenPos());
    }
}

QRectF rubber_band_rect::get_resize_region() const noexcept
{
    if(rect_.height() > 10 && rect_.width() > 10){
        return QRectF(rect_.right() - 10, rect_.bottom() - 10, 10,10);
    }else if(fuzzy_equal(rect_.height(), 0.0) && fuzzy_equal(rect_.width(), 10.0)){
        return rect_;
    }

    return QRectF();
}

}
