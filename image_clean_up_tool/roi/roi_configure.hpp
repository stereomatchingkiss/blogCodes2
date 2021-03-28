#ifndef ROI_CONFIGURE_HPP
#define ROI_CONFIGURE_HPP

#include <QWidget>

namespace Ui {
class roi_configure;
}

namespace eroi{

class rubber_band_base;
class rubber_band_rect;

}

class cv_algo_base;
class custom_graphics_view_pixmap;
class roi_configure_data_creator;
class QGraphicsScene;

class roi_configure : public QWidget
{
    Q_OBJECT

public:
    explicit roi_configure(QWidget *parent = nullptr);
    ~roi_configure() override;

    QSize get_image_size() const noexcept;
    QPolygonF get_relative_polygon() const;
    QRectF get_relative_roi() const;    
    int get_rotation_angle() const;
    QJsonObject get_states() const;

    void set_qimage(QImage const &img);
    void set_states(QJsonObject const &val);    

private:
    void changeEvent(QEvent *event) override;
    void create_rubber_band();

    void restore_roi_on_image();    

    Ui::roi_configure *ui;

    QSize img_size_;    
    custom_graphics_view_pixmap *pixmap_item_;    
    roi_configure_data_creator *roi_creator_ = nullptr;
    QGraphicsScene *scene_ = nullptr;
    cv_algo_base *stream_receiver_ = nullptr;
};

#endif // ROI_CONFIGURE_HPP
