#ifndef OBJECT_DETECTOR_HPP
#define OBJECT_DETECTOR_HPP

#include "ssd_detector.hpp"

#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QQuickPaintedItem>

#include <map>
#include <memory>

class QCameraImageCapture;

class object_detector : public QQuickPaintedItem
{
    Q_OBJECT    
public:
    explicit object_detector(QQuickItem *parent = nullptr);
    ~object_detector();

    Q_INVOKABLE void detect(QObject *qml_cam);

    void paint(QPainter *painter);

signals:
    void message(QString msg);
    void objectDetected();

private:
    void draw_detect_results(ssd_detector::result_type const &results,
                             cv::Mat &inout) const;
    void image_capture(int id, QImage const &img);

    QImage buffer_;
    QCameraImageCapture *cam_capture_;
    QFuture<void> future_;
    std::unique_ptr<ssd_detector> detector_;
    std::map<QString, cv::Scalar> name_to_color_;
    QFutureWatcher<void> watcher_;
    void init();
};

#endif // OBJECT_DETECTOR_HPP
