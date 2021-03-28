#ifndef CUSTOM_GRAPHICS_VIEW_PIXMAP_HPP
#define CUSTOM_GRAPHICS_VIEW_PIXMAP_HPP

#include <QGraphicsPixmapItem>

class custom_graphics_view_pixmap : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    enum type{
        Type = UserType + 5
    };

    explicit custom_graphics_view_pixmap(QGraphicsItem *parent = nullptr);

    int type() const override;

signals:
    void hover_enter();
    void hover_leave();

private:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
};

#endif // CUSTOM_GRAPHICS_VIEW_PIXMAP_HPP
