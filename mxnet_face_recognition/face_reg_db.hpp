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
        std::string id_;
    };

    face_reg_db();
    ~face_reg_db();

    void add_new_face(face_key input, std::string id);
    id_info find_most_similar_face(face_key const &input) const;

private:
    struct face_info
    {
       face_key key_;
       std::string id_;
    };

    std::vector<face_info> face_keys_;
};

}

#endif // FACE_REG_DB_HPP
