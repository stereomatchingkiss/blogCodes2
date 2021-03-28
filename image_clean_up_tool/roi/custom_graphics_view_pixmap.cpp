#include "custom_graphics_view_pixmap.hpp"

#include <QGraphicsSceneHoverEvent>

custom_graphics_view_pixmap::custom_graphics_view_pixmap(QGraphicsItem *parent) :
    QGraphicsPixmapItem(parent)
{
    setAcceptHoverEvents(true);
}

int custom_graphics_view_pixmap::type() const
{
    return custom_graphics_view_pixmap::Type;
}

void custom_graphics_view_pixmap::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit hover_enter();
    QGraphicsPixmapItem::hoverEnterEvent(event);
}

void custom_graphics_view_pixmap::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit hover_leave();
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}
