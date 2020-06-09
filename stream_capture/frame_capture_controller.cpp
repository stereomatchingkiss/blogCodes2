#include "frame_capture_controller.hpp"

#include "frame_capture_config.hpp"

#include "frame_capture_opencv_worker.hpp"

#include <QDebug>

namespace frame_capture{

void capture_controller::init_frame_capture()
{
    frame_capture_->moveToThread(&thread_);
    connect(&thread_, &QThread::finished, frame_capture_, &QObject::deleteLater);

    connect(frame_capture_, &worker_base::cannot_open, this, &capture_controller::cannot_open);    

    connect(this, &capture_controller::start, frame_capture_, &worker_base::start);
    connect(this, &capture_controller::start_url, frame_capture_, &worker_base::start_url);

    thread_.start();
}

capture_controller::capture_controller(frame_capture_config const &config) :
    QObject(),
    frame_capture_(new capture_opencv_worker(config))
{
    init_frame_capture();
}

capture_controller::~capture_controller()
{        
    qDebug()<<__func__<<": quit";
    //must called release or before quit and wait, else the
    //frame capture will fall into infinite loop
    frame_capture_->release();
    thread_.quit();
    qDebug()<<__func__<<": wait";
    thread_.wait();
    qDebug()<<__func__<<": wait exit";
}

bool capture_controller::add_image_listener(std::function<void (cv::Mat)> functor, void *key)
{
    return frame_capture_->add_image_listener(std::move(functor), key);
}

frame_capture_config capture_controller::get_params() const
{
    return frame_capture_->get_params();
}

QString capture_controller::get_url() const
{
    return frame_capture_->get_url();
}

bool capture_controller::is_stop() const
{
    return frame_capture_->is_stop();
}

bool capture_controller::remove_image_listener(void *key)
{
    return frame_capture_->remove_image_listener(key);
}

void capture_controller::set_max_fps(int input)
{
    frame_capture_->set_max_fps(input);
}

void capture_controller::set_params(const frame_capture_config &config)
{
    frame_capture_->set_params(config);
}

void capture_controller::stop()
{
    frame_capture_->stop();
}

}
