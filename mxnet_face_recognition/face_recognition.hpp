#ifndef FACE_RECOGNITION_HPP
#define FACE_RECOGNITION_HPP

#include "face_reg_info.hpp"

#include <opencv2/core.hpp>

#include <memory>

#include <string>
#include <vector>

namespace ocv
{

namespace dlib_aux{

class cnn_face_detector;
struct face_detector_params;

}

}

namespace ocv
{

namespace mxnet_aux{

class face_key_extractor;
struct face_key_extractor_params;

}

}

class face_reg_db;
struct face_reg_info;

class face_recognition
{
public:
    face_recognition(ocv::dlib_aux::face_detector_params face_det_params,
                     ocv::mxnet_aux::face_key_extractor_params face_key_params);
    ~face_recognition();

    face_recognition(face_recognition &&) = default;
    face_recognition& operator=(face_recognition &&) = default;

    bool add_new_face(cv::Mat const &input, std::string id);
    std::vector<face_reg_info> recognize_faces(cv::Mat const &input);

private:
    std::unique_ptr<ocv::dlib_aux::cnn_face_detector> face_detector_;
    std::unique_ptr<face_reg_db> face_reg_db_;
    std::unique_ptr<ocv::mxnet_aux::face_key_extractor> key_extractor_;
};

#endif // FACE_RECOGNITION_HPP
