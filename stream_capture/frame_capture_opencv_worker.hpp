#ifndef FRAME_CAPTURED_OPENCV_WORKER_HPP
#define FRAME_CAPTURED_OPENCV_WORKER_HPP

#include "frame_worker_base.hpp"

#include <QObject>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

#include <atomic>
#include <functional>
#include <map>
#include <mutex>

class QTimer;

namespace frame_capture{

struct frame_capture_config;

class capture_opencv_worker : public worker_base
{
    Q_OBJECT
public:
    explicit capture_opencv_worker(frame_capture_config const &config);
    ~capture_opencv_worker() override;

    bool add_image_listener(std::function<void(cv::Mat)> functor, void *key) override;
    frame_capture_config get_params() const override;
    QString get_url() const override;
    void set_max_fps(int input) override;

    bool is_stop() const override;
    void release() override;
    bool remove_image_listener(void *key) override;
    void set_params(frame_capture_config const &config) override;
    void start() override;
    void start_url(QString const &url) override;
    void stop() override;

private:
    void open_media(QString const &media_url);
    void captured_frame();
    void set_max_fps_non_ts(int input);
    void time_out();

    cv::VideoCapture capture_;
    bool deep_copy_ = false;
    int frame_duration_ = 30;
    std::map<void*, std::function<void(cv::Mat)>> functors_;
    int max_fps_;
    QString media_url_;
    mutable std::mutex mutex_;
    bool stop_;
    QTimer *timer_ = nullptr;
    int webcam_index_ = 0;
};

}

#endif // FRAME_CAPTURED_OPENCV_WORKER_HPP
