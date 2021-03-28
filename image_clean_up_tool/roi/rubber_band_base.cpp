#include "rubber_band_base.hpp"

#include <QAction>
#include <QGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>

namespace eroi{

rubber_band_base::rubber_band_base(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    set_enable_normal_features(true);
}

void rubber_band_base::set_enable_normal_features(bool value)
{
    setFlag(QGraphicsItem::ItemIsSelectable, value);
    setFlag(QGraphicsItem::ItemIsMovable, value);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, value);
    setAcceptHoverEvents(value);
}

void rubber_band_base::set_enable_context_menu(bool)
{

}

QRectF rubber_band_base::bottom_left_corner() const noexcept
{
    return QRectF(boundingRect().left(), boundingRect().bottom() - 10, 10, 10);
}

QRectF rubber_band_base::bottom_right_corner() const noexcept
{
    return QRectF(boundingRect().right() - 10, boundingRect().bottom() - 10, 10, 10);
}

rubber_band_base::corner_place rubber_band_base::process_hover_event(QGraphicsSceneHoverEvent *event)
{
    QPointF const event_pos = event->pos();
    if(top_left_corner().contains(event_pos)){
        setCursor(QCursor(Qt::SizeFDiagCursor));
        return corner_place::top_left;
    }else if(bottom_right_corner().contains(event_pos)){
        setCursor(QCursor(Qt::SizeFDiagCursor));
        return corner_place::bottom_right;
    }else if(top_right_corner().contains(event_pos)){
        setCursor(QCursor(Qt::SizeBDiagCursor));
        return corner_place::top_right;
    }else if(bottom_left_corner().contains(event_pos)){
        setCursor(QCursor(Qt::SizeBDiagCursor));
        return corner_place::bottom_left;
    }else{
        setCursor(QCursor(Qt::OpenHandCursor));
        return corner_place::other;
    }
}

QRectF rubber_band_base::top_left_corner() const noexcept
{
    return QRectF(boundingRect().left(), boundingRect().top(), 10, 10);
}

QRectF rubber_band_base::top_right_corner() const noexcept
{
    return QRectF(boundingRect().right() - 10, boundingRect().top(), 10, 10);
}

}
