#ifndef RUBBER_BAND_POINTS_HPP_POERWIER92342
#define RUBBER_BAND_POINTS_HPP_POERWIER92342

#include "rubber_band_base.hpp"

#include <QBrush>
#include <QPen>
#include <QPolygonF>

namespace eroi{

class rubber_band_points : public rubber_band_base
{
    Q_OBJECT
public:
    enum type{
        Type = UserType + 4
    };

    explicit rubber_band_points(QGraphicsItem *parent = nullptr);
    explicit rubber_band_points(QPolygonF polygon, QGraphicsItem *parent = nullptr);

    void add_point(QPointF const &pt);
    QRectF boundingRect() const override;
    void clear_points();
    QPolygonF get_polygon() const;
    QRectF get_roi() const noexcept override;
    bool is_select() const noexcept override;

    void pop_point();

    void scale_down();
    void scale_to_normal();
    void scale_up();
    void select() override;

    int type() const override;

    void unselect() override;

signals:
    void error(QString const &msg);

private:
    bool back_to_parent_region(QPointF const &item_corner);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    bool should_be_clipped() noexcept;

    QPen brect_pen_;
    QBrush fill_color_;
    QColor polygon_color_;
    QPen polygon_pen_;
    QPolygonF polygon_;
    qreal scale_;
};

}

#endif // RUBBER_BAND_POINTS_HPP_POERWIER92342
