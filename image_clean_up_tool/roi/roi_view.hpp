#ifndef ROI_VIEW_HPP
#define ROI_VIEW_HPP

#include <QGraphicsView>

/**
 * @todo fix the class name
 */
class roi_view : public QGraphicsView
{
    Q_OBJECT
public:
    explicit roi_view(QWidget *parent = nullptr);

signals:
    void mouse_left_click(QPoint const &point);
    void mouse_right_click(QPoint const &pt);

private:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // ROI_VIEW_HPP
