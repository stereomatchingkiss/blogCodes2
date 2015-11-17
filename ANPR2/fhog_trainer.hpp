#ifndef FHOG_TRAINER_HPP
#define FHOG_TRAINER_HPP

#include <dlib/image_processing.h>

#include <boost/program_options.hpp>

class fhog_number_plate_trainer
{
public:
    fhog_number_plate_trainer(int argc, char **argv);

private:
    boost::program_options::variables_map
    parse_command_line(int argc, char **argv);

    void preprocess(std::string const &train_xml,
                    std::string const &test_xml);

    dlib::array<dlib::array2d<unsigned char>> images_train;
    dlib::array<dlib::array2d<unsigned char>> images_test;
    std::vector<std::vector<dlib::rectangle>> face_boxes_train;
    std::vector<std::vector<dlib::rectangle>> face_boxes_test;
};

#endif
