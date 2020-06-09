#ifndef FRAME_CAPTURE_OPENCV_CONTROLLER_HPP
#define FRAME_CAPTURE_OPENCV_CONTROLLER_HPP

#include <QObject>
#include <QThread>
#include <QVariant>

#include <opencv2/core.hpp>

#include <functional>

namespace frame_capture{

struct frame_capture_config;

class worker_base;

class capture_controller : public QObject
{
    Q_OBJECT
public:
    explicit capture_controller(frame_capture_config const &config);
    ~capture_controller() override;

    bool add_image_listener(std::function<void(cv::Mat)> functor, void *key);
    frame_capture_config get_params() const;
    QString get_url() const;
    bool is_stop() const;
    bool remove_image_listener(void *key);
    void set_max_fps(int input);
    void set_params(frame_capture_config const &config);    
    void stop();

signals:
    void cannot_open(QString const &media_url);
    void reach_the_end();

    void start();
    void start_url(QString const &url);

private:
    void init_frame_capture();

    worker_base *frame_capture_;
    QThread thread_;
};

}

#endif // FRAME_CAPTURE_OPENCV_CONTROLLER_HPP
