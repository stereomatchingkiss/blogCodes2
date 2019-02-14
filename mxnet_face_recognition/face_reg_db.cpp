#include "face_reg_db.hpp"

namespace mxnet_tool{

size_t face_reg_db::add_new_face(const face_key &input)
{
    face_keys_.emplace_back(input);

    return face_keys_.size() - 1;
}

face_reg_db::id_info face_reg_db::find_most_similar_face(const face_key &input) const
{
    id_info result;
    for(size_t i = 0; i != face_keys_.size(); ++i){
        auto const confident = face_keys_[i].similarity(input);
        if(confident > result.confident_){
            result.confident_ = confident;
            result.id_ = i;
        }
    }

    return result;
}


}
