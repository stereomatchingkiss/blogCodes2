#ifndef FHOG_TRAINER_HPP
#define FHOG_TRAINER_HPP

#include <dlib/image_processing.h>

#include <boost/program_options.hpp>

class fhog_number_plate_trainer
{
public:
    fhog_number_plate_trainer(int argc, char **argv);

private:
    using image_scanner_type =
    dlib::scan_fhog_pyramid<dlib::pyramid_down<8>>;

    boost::program_options::variables_map
    parse_command_line(int argc, char **argv);
    void preprocess(std::string const &train_xml);

    void show_train_result(dlib::object_detector<image_scanner_type> &detector);

    void visualize_result(dlib::object_detector<image_scanner_type> &detector);

    dlib::object_detector<image_scanner_type> train() const;

    using images_type = dlib::array<dlib::array2d<unsigned char>>;
    using rects_type = std::vector<std::vector<dlib::rectangle>>;

    images_type images_input_;
    images_type images_test_;
    images_type images_train_;
    images_type images_validate_;
    rects_type face_boxes_input_;
    rects_type face_boxes_test_;
    rects_type face_boxes_train_;
    rects_type face_boxes_validate_;
};

#endif
