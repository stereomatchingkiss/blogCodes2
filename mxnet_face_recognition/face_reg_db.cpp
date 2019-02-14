#include "face_reg_db.hpp"

namespace mxnet_tool{

void face_reg_db::add_new_face(face_key input, std::string id)
{
    face_info info;
    info.id_.swap(id);
    info.key_ = std::move(input);
    face_keys_.emplace_back(std::move(info));
}

face_reg_db::id_info face_reg_db::find_most_similar_face(const face_key &input) const
{
    id_info result;
    for(size_t i = 0; i != face_keys_.size(); ++i){
        auto const confident = face_keys_[i].key_.similarity(input);
        if(confident > result.confident_){
            result.confident_ = confident;
            result.id_ = face_keys_[i].id_;
        }
    }

    return result;
}


}
