#ifndef FRAME_CAPTURE_CONFIG_HPP
#define FRAME_CAPTURE_CONFIG_HPP

#include <QString>

namespace frame_capture{

struct frame_capture_config
{
    //True will copy the frame captured, useful if the functors
    //you add work in different thread
    bool deep_copy_ = false;
    int fps_ = 30;
    QString url_;
};

}

#endif // FRAME_CAPTURE_CONFIG_HPP
