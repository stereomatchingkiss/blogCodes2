#include "face_recognition.hpp"

#include "face_detector.hpp"
#include "face_detector_params.hpp"
#include "face_key_extractor.hpp"
#include "face_key_extractor_params.hpp"
#include "face_reg_db.hpp"

using namespace dlib_tool;
using namespace mxnet_tool;

face_recognition::face_recognition(face_detector_params face_det_params,
                                   face_key_extractor_params face_key_params) :
    face_detector_(std::make_unique<face_detector>(std::move(face_det_params))),
    face_reg_db_(std::make_unique<face_reg_db>()),
    key_extractor_(std::make_unique<face_key_extractor>(std::move(face_key_params)))
{

}

face_recognition::~face_recognition()
{

}

bool face_recognition::add_new_face(const cv::Mat &input, std::string id)
{
    auto const detected_faces = face_detector_->forward(input);
    if(!detected_faces.empty()){
        auto key = key_extractor_->forward(detected_faces[0].face_aligned_);
        face_reg_db_->add_new_face(std::move(key), std::move(id));

        return true;
    }

    return false;
}

std::vector<face_reg_info> face_recognition::recognize_faces(const cv::Mat &input)
{
    std::vector<face_reg_info> result;
    auto const detected_faces = face_detector_->forward(input);
    for(auto const &det_face : detected_faces){
        auto const key = key_extractor_->forward(det_face.face_aligned_);
        auto id_info = face_reg_db_->find_most_similar_face(key);
        face_reg_info fri;
        fri.confident_ = id_info.confident_;
        fri.id_ = std::move(id_info.id_);
        auto const &rect = det_face.rect_.rect;
        fri.roi_ = cv::Rect(rect.left(), rect.top(),
                            static_cast<int>(rect.width()), static_cast<int>(rect.height()));
        fri.roi_.x = std::max(0, fri.roi_.x);
        fri.roi_.y = std::max(0, fri.roi_.y);
        fri.roi_.height = std::min(input.rows, fri.roi_.height);
        fri.roi_.width = std::min(input.cols, fri.roi_.width);
        result.emplace_back(std::move(fri));
    }

    return result;
}
