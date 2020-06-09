#ifndef FRAME_WORKER_BASE_HPP
#define FRAME_WORKER_BASE_HPP

#include <opencv2/core.hpp>

#include <QObject>

#include <functional>

namespace frame_capture{

struct frame_capture_config;

class worker_base : public QObject
{
    Q_OBJECT
public:
    explicit worker_base(QObject *parent = nullptr);

    /**
     * Add listener to process the frame
     * @param functor Process the frame
     * @param key Key of the functor, we need it to remove the functor
     * @return True if able to add the listener and vice versa
     */
    virtual bool add_image_listener(std::function<void(cv::Mat)> functor, void *key) = 0;

    virtual frame_capture_config get_params() const = 0;
    virtual QString get_url() const = 0;
    virtual bool is_stop() const = 0;
    /**
     * This function will stop the frame capturer, release all functor etc
     */
    virtual void release() = 0;
    /**
     * Remove the listener
     * @param key The key same as add_image_listener when you add the functor
     * @return True if able to remove the listener and vice versa
     * @warning Remember to remove_image_listener before the resources of the register functor
     * released, else the app may crash.
     */
    virtual bool remove_image_listener(void *key) = 0;
    virtual void set_max_fps(int input) = 0;
    virtual void set_params(frame_capture_config const &config) = 0;
    /**
     * Will start the frame captured with the url set by the start_url api
     */
    virtual void start() = 0;
    virtual void start_url(QString const &url) = 0;
    virtual void stop() = 0;

signals:
    void cannot_open(QString const &media_url);    
};

}

#endif // FRAME_WORKER_BASE_HPP
