#ifndef FACE_RECOGNITION_HPP
#define FACE_RECOGNITION_HPP

#include "face_reg_info.hpp"

#include <opencv2/core.hpp>

#include <memory>

#include <string>
#include <vector>

namespace ocv
{

namespace face{

class dlib_cnn_face_detector;
struct dlib_cnn_face_detector_params;

}

}

namespace ocv
{

namespace face{

class insight_face_key_extractor;
struct insight_face_key_extractor_params;

}

}

class face_reg_db;
struct face_reg_info;

class face_recognition
{
public:
    face_recognition(ocv::face::dlib_cnn_face_detector_params face_det_params,
                     ocv::face::insight_face_key_extractor_params face_key_params);
    ~face_recognition();

    face_recognition(face_recognition &&) = default;
    face_recognition& operator=(face_recognition &&) = default;

    bool add_new_face(cv::Mat const &input, std::string id);
    std::vector<face_reg_info> recognize_faces(cv::Mat const &input);

private:
    std::unique_ptr<ocv::face::dlib_cnn_face_detector> face_detector_;
    std::unique_ptr<face_reg_db> face_reg_db_;
    std::unique_ptr<ocv::face::insight_face_key_extractor> key_extractor_;
};

#endif // FACE_RECOGNITION_HPP
