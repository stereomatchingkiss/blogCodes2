#include "face_recognition.hpp"

#include "../libs/face_detect/face_detector.hpp"
#include "../libs/face_detect/face_detector_params.hpp"
#include "../libs/face_key/face_key_extractor.hpp"
#include "../libs/face_key/face_key_extractor_params.hpp"
#include "face_reg_db.hpp"

using namespace ocv::face;
using namespace ocv::face;

face_recognition::face_recognition(dlib_cnn_face_detector_params face_det_params,
                                   insight_face_key_extractor_params face_key_params) :
    face_detector_(std::make_unique<dlib_cnn_face_detector>(std::move(face_det_params))),
    face_reg_db_(std::make_unique<face_reg_db>()),
    key_extractor_(std::make_unique<insight_face_key_extractor>(std::move(face_key_params)))
{

}

face_recognition::~face_recognition()
{

}

bool face_recognition::add_new_face(const cv::Mat &input, std::string id)
{
    auto const detected_faces = face_detector_->forward(input);
    if(!detected_faces.face_aligned_.empty()){
        auto key = key_extractor_->forward(detected_faces.face_aligned_[0]);
        face_reg_db_->add_new_face(std::move(key), std::move(id));

        return true;
    }

    return false;
}

std::vector<face_reg_info> face_recognition::recognize_faces(const cv::Mat &input)
{
    std::vector<face_reg_info> result;
    auto const detected_faces = face_detector_->forward(input);
    if(!detected_faces.face_aligned_.empty()){
        auto keys = key_extractor_->forward(detected_faces.face_aligned_);        
        for(size_t i = 0; i != detected_faces.face_aligned_.size(); ++i){
            auto id_info = face_reg_db_->find_most_similar_face(keys[i]);
            face_reg_info fri;
            fri.confident_ = id_info.confident_;
            fri.id_ = std::move(id_info.id_);
            auto const &rect = detected_faces.rect_[i].rect;
            fri.roi_.x = std::max(0, static_cast<int>(rect.left()));
            fri.roi_.y = std::max(0, static_cast<int>(rect.top()));
            fri.roi_.height = std::min(input.rows, static_cast<int>(rect.height()));
            fri.roi_.width = std::min(input.cols, static_cast<int>(rect.width()));
            result.emplace_back(std::move(fri));
        }
    }

    return result;
}
