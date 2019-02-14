#ifndef FACE_REG_DB_HPP
#define FACE_REG_DB_HPP

#include "face_key.hpp"

#include <vector>

namespace mxnet_tool{

class face_reg_db
{
public:
    struct id_info
    {
        double confident_ = -1.0;
        size_t id_ = 0;
    };

    face_reg_db();
    ~face_reg_db();

    size_t add_new_face(face_key const &input);
    id_info find_most_similar_face(face_key const &input) const;

private:
    std::vector<face_key> face_keys_;
};

}

#endif // FACE_REG_DB_HPP
