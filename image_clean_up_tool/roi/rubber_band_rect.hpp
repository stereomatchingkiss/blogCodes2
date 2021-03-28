#ifndef RUBBER_BAND_RECT_HPP
#define RUBBER_BAND_RECT_HPP

#include "rubber_band_base.hpp"

#include "roi_shape.hpp"

#include <QBrush>

namespace eroi{
/**
 * @warning This class expect to have a parent item
 */
class rubber_band_rect : public rubber_band_base
{
    Q_OBJECT
public:
    enum type{
        Type = UserType + 3
    };

    explicit rubber_band_rect(eroi::rect_shape shape,
                              QRectF const &rect,
                              QColor const &color = QColor(255,127,80,255),
                              QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;    
    QRectF get_roi() const noexcept override;
    eroi::rect_shape get_shape() const noexcept;
    bool is_select() const noexcept override;

    void select() override;
    void set_bounding_rect(QRectF const &rect);
    void set_enable_context_menu(bool val) override;

    int type() const override;

    void unselect() override;    

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    bool back_to_parent_region(QPointF const &event_pos);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    bool clipped_if_too_big() noexcept;

    QRectF get_resize_region() const noexcept;

    void resize_brect(QPointF const &top_left, QPointF const &bottom_right);

    corner_place corner_place_;
    bool enable_context_menu_ = true;
    QBrush fill_color_;    
    QColor rband_color_;
    QRectF rect_;
    eroi::rect_shape shape_;
};

}

#endif // RUBBER_BAND_RECT_HPP
