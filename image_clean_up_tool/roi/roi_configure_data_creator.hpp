#ifndef ROI_CONFIGURE_DATA_CREATOR_HPP
#define ROI_CONFIGURE_DATA_CREATOR_HPP

#include "roi_shape.hpp"

#include <QObject>
#include <QPolygonF>

namespace eroi
{

class rubber_band_base;

}

class custom_graphics_view_pixmap;
class QJsonObject;
class QGraphicsScene;

class roi_configure_data_creator : public QObject
{
    Q_OBJECT
public:
    explicit roi_configure_data_creator(custom_graphics_view_pixmap &pixmap,
                                        QGraphicsScene &scene,
                                        QObject *parent = nullptr);
    ~roi_configure_data_creator();

    void add_point_to_polygon(QPointF const &pt);
    void create_rubber_band_rect(QRectF const &rect);
    QPolygonF get_relative_polygon() const;    
    QRectF get_relative_roi() const;
    int get_roi_size() const noexcept;
    QJsonObject get_states() const;
    void release();
    /**
     * This function designed for set_states/get_states.
     * The size of pixmap do not update when the dialog of roi_configure first shown,
     * so we need to call this function to adjust the size of roi/polygons
     */
    void rescaled_roi();

    void set_enable_context_menu(bool value);
    void set_states(QJsonObject const &val);

signals:
    void rubber_band_rect_mouse_press();
    void rubber_band_rect_mouse_release();
    void rubber_band_released();

private:
    enum class scaled_mode
    {
        divide,
        multiply
    };

    enum class roi_type
    {
        rect,
        polygon
    };

    eroi::rubber_band_base* create_new_rubber_band(QRectF const &rect, roi_type shape);    
    QRectF scaled_roi(QRectF const &roi, QSize const &img_size, scaled_mode mode = scaled_mode::divide) const noexcept;

    //we need these first_init_* to restore the states of the rectangle/polygon when
    //the dialog of roi show on the first time, because size of the QGraphicsPixmap
    //do not update when initialized
    QPolygonF first_init_polygon_;
    QRectF first_init_roi_;
    custom_graphics_view_pixmap &pixmap_item_;
    eroi::rubber_band_base *rubber_band_ = nullptr;
    QGraphicsScene &scene_;
};

#endif // ROI_CONFIGURE_DATA_CREATOR_HPP
