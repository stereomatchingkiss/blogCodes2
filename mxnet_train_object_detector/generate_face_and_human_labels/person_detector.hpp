#ifndef PERSON_DETECTOR_HPP
#define PERSON_DETECTOR_HPP

#include <mxnet_cpp_object_detection/object_detector_filter.hpp>

#include <QString>

#include <memory>

class object_detector;
class object_detector_filter;

class person_detector
{
public:
    explicit person_detector(QString const &setting_file_location);
    ~person_detector();

    std::vector<object_detector_filter::result_type> detect(cv::Mat const &img);

private:
    std::unique_ptr<object_detector_filter> filter_;
    std::unique_ptr<object_detector> object_detector_;
};

#endif // PERSON_DETECTOR_HPP
