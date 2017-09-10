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

    Q_INVOKABLE void detect(QObject *qml_cam, float confident, QString const &device_id);
    Q_INVOKABLE void clear_graph();

    void paint(QPainter *painter);

signals:
    void message(QString msg);
    void objectDetected();

private:
    QString copy_asset_file(QString const &source);
    void draw_detect_results(ssd_detector::result_type const &results,
                             cv::Mat &inout);
    void init();
    void image_capture(int id, QImage const &img);

    QImage buffer_;
    QCameraImageCapture *cam_capture_;
    float confident_ = 0.2f;
    QString device_id_;    
    std::unique_ptr<ssd_detector> detector_;    
    QFuture<void> future_;
    QString message_;
    std::map<QString, cv::Scalar> name_to_color_;
    QFutureWatcher<void> watcher_;    
};

#endif // OBJECT_DETECTOR_HPP
