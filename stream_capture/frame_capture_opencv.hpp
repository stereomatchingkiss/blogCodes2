#ifndef FRAME_CAPTURE_OPENCV_HPP
#define FRAME_CAPTURE_OPENCV_HPP

#include "frame_capture_controller.hpp"

#include <memory>

namespace frame_capture{

using opencv_camera = std::shared_ptr<capture_controller>;

}

#endif // FRAME_CAPTURE_OPENCV_HPP
