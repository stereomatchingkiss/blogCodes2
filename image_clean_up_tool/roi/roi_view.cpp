#include "roi_view.hpp"

#include <QMouseEvent>

roi_view::roi_view(QWidget *parent) :
    QGraphicsView(parent)
{

}

void roi_view::mousePressEvent(QMouseEvent *event)
{    
    if(event->button() == Qt::LeftButton){
        emit mouse_left_click(event->pos());
    }else if(event->button() == Qt::RightButton){
        emit mouse_right_click(QPoint(static_cast<int>(event->screenPos().x()),
                                      static_cast<int>(event->screenPos().y())));
    }
    QGraphicsView::mousePressEvent(event);
}
