#include "frame_capture_opencv_worker.hpp"

#include "frame_capture_config.hpp"

#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

#include <QTimer>

#include <chrono>
#include <thread>

namespace frame_capture{

capture_opencv_worker::capture_opencv_worker(frame_capture_config const &config) :
    worker_base(),
    deep_copy_(config.deep_copy_),
    media_url_(config.url_),
    stop_(true)
{
    set_max_fps(config.fps_);
}

capture_opencv_worker::~capture_opencv_worker()
{
    release();
}

bool capture_opencv_worker::add_image_listener(std::function<void (cv::Mat)> functor, void *key)
{    
    std::lock_guard<std::mutex> lock(mutex_);
    return functors_.insert(std::make_pair(key, std::move(functor))).second;
}

frame_capture_config capture_opencv_worker::get_params() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    frame_capture_config config;
    config.deep_copy_ = deep_copy_;
    config.fps_ = max_fps_;
    config.url_ = media_url_;

    return config;
}

QString capture_opencv_worker::get_url() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return media_url_;
}

void capture_opencv_worker::set_max_fps(int input)
{
    std::lock_guard<std::mutex> lock(mutex_);
    set_max_fps_non_ts(input);
}

void capture_opencv_worker::open_media(const QString &media_url)
{   
    qDebug()<<__func__<<": "<<media_url;
    bool can_convert_to_int = 0;
    if(timer_){
        timer_->stop();
    }
    media_url.toInt(&can_convert_to_int);
    stop_ = true;
    try{
        capture_.release();
        //If you pass in int, opencv will open webcam if it could
        if(can_convert_to_int){
            capture_.open(media_url.toInt());
        }else{
            capture_.open(media_url.toStdString());
        }
    }catch(std::exception const &ex){
        qDebug()<<__func__<<ex.what();
    }

    if(capture_.isOpened()){
        stop_ = false;        
    }else{
        stop_ = true;
        emit cannot_open(media_url);
    }
}

bool capture_opencv_worker::is_stop() const
{    
    std::lock_guard<std::mutex> lock(mutex_);
    return stop_;
}

void capture_opencv_worker::release()
{
    qDebug()<<__func__<<": delete cam with url = "<<media_url_;
    std::lock_guard<std::mutex> lock(mutex_);
    qDebug()<<__func__<<": enter lock region";
    stop_ = true;
    qDebug()<<__func__<<": clear functor";
    functors_.clear();
    qDebug()<<__func__<<": release capture";
    capture_.release();
    qDebug()<<__func__<<": delete timer later";
}

bool capture_opencv_worker::remove_image_listener(void *key)
{    
    std::lock_guard<std::mutex> lock(mutex_);
    return functors_.erase(key) > 0;
}

void capture_opencv_worker::set_params(const frame_capture_config &config)
{
    std::lock_guard<std::mutex> lock(mutex_);
    deep_copy_ = config.deep_copy_;
    media_url_ = config.url_;
    set_max_fps_non_ts(config.fps_);
}

void capture_opencv_worker::start()
{
    start_url(media_url_);
}

void capture_opencv_worker::start_url(QString const &url)
{    
    stop();
    qDebug()<<__func__<<": stop = "<<stop_<<", url = "<<url;
    std::lock_guard<std::mutex> lock(mutex_);
    open_media(url);
    if(capture_.isOpened()){
        media_url_ = url;
        captured_frame();
    }
}

void capture_opencv_worker::stop()
{        
    std::lock_guard<std::mutex> lock(mutex_);
    stop_ = true;
    qDebug()<<__func__<<": stop the worker = "<<stop_;
}

void capture_opencv_worker::captured_frame()
{        
    qDebug()<<__func__<<": capture_.isOpened()";

    //You must initialize and delete timer in the same thread
    //of the VideoCapture running, else you may trigger undefined
    //behavior
    if(!timer_){
        qDebug()<<__func__<<": init timer";
        timer_ = new QTimer;
        timer_->setSingleShot(true);
        connect(timer_, &QTimer::timeout, this, &capture_opencv_worker::time_out);
    }

    qDebug()<<__func__<<": start timer";
    timer_->start();
    qDebug()<<__func__<<": called start timer";
}

void capture_opencv_worker::set_max_fps_non_ts(int input)
{
    max_fps_ = std::max(input, 1);
    frame_duration_ = std::max(1000 / max_fps_, 1);
}

void capture_opencv_worker::time_out()
{        
    QElapsedTimer elapsed;
    elapsed.start();
    std::lock_guard<std::mutex> lock(mutex_);    
    if(!stop_ && timer_){
        capture_.grab();
        cv::Mat frame;
        capture_.retrieve(frame);
        if(!frame.empty()){
            //qDebug()<<__func__<<": can access frame of url = "<<media_url_;
            for(auto &iter : functors_){
                iter.second(deep_copy_ ? frame.clone() : frame);
            }
            auto const interval = frame_duration_ - elapsed.elapsed();
            timer_->start(std::max(static_cast<int>(interval), 10));
        }else{
            //qDebug()<<__func__<<": cannot access frame";
            open_media(media_url_);
            timer_->start();
        }
    }else{
        capture_.release();
        if(timer_){
            //you must delete timer in the thread where you initiaize it
            delete timer_;
            timer_ = nullptr;
        }
    }
}

}
