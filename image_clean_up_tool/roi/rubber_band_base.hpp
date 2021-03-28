#ifndef RUBBER_BAND_BASE_HPP
#define RUBBER_BAND_BASE_HPP

#include <QGraphicsObject>

namespace eroi{
/**
 * A base class for different rubber band
 */
class rubber_band_base : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit rubber_band_base(QGraphicsItem *parent = nullptr);

    virtual QRectF get_roi() const noexcept = 0;

    virtual bool is_select() const noexcept = 0;

    virtual void select() = 0;

    /**
     * Enable/disable normal features of rubber band object.
     * Default implementation enable/disable following features
     *
     * QGraphicsItem::ItemIsSelectable
     * QGraphicsItem::ItemIsMovable
     * QGraphicsItem::ItemSendsGeometryChanges
     * AcceptHoverEvents
     * @param value Enable normal features if true and vice versa
     */
    virtual void set_enable_normal_features(bool value);

    virtual void set_enable_context_menu(bool value);

    virtual void unselect() = 0;

signals:
    void delete_rubber_band();
    void mouse_press();
    void mouse_press(rubber_band_base *rband);
    void mouse_release(rubber_band_base *rband);    

protected:
    enum class corner_place{
        top_left,
        top_right,
        bottom_right,
        bottom_left,
        other
    };

    virtual QRectF bottom_left_corner() const noexcept;
    virtual QRectF bottom_right_corner() const noexcept;    

    virtual rubber_band_base::corner_place process_hover_event(QGraphicsSceneHoverEvent *event);

    QRectF top_left_corner() const noexcept;
    QRectF top_right_corner() const noexcept;    
};

}

#endif // RUBBER_BAND_BASE_HPP
