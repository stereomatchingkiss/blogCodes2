#ifndef FEATURE_EXTRACTOR_RESNET34_H
#define FEATURE_EXTRACTOR_RESNET34_H

#include <dlib/matrix.h>

#include <memory>

/**
 * Purpose of this class is reduce the compile
 * time incur by resnet34.
 */
class feature_extractor_resnet34
{
public:
    using img_type = dlib::matrix<dlib::rgb_pixel>;

    /**
     * @param file_location location of the resnet34
     */
    feature_extractor_resnet34(std::string const &file_location);
    ~feature_extractor_resnet34();

    dlib::matrix<float> get_feature(img_type const &input);
    std::vector<dlib::matrix<float>> get_features(std::vector<img_type> const &input);

private:
    struct pimpl;

    std::unique_ptr<pimpl> pimpl_;
};

#endif // FEATURE_EXTRACTOR_RESNET34_H
