#ifndef RANDOM_CROP_HPP
#define RANDOM_CROP_HPP

#include <dlib/image_processing.h>
#include <dlib/matrix.h>

template<typename ImgType>
dlib::rectangle make_random_cropping_rect(ImgType const &img, dlib::rand& rnd,
                                          double mins = 0.7, double maxs = 0.85)
{
    // figure out what rectangle we want to crop from the image
    auto const scale = mins + rnd.get_random_double()*(maxs-mins);
    auto const size = scale*std::min(img.nr(), img.nc());
    dlib::rectangle rect(size, size);
    // randomly shift the box around
    dlib::point offset(rnd.get_random_32bit_number()%(img.nc()-rect.width()),
                       rnd.get_random_32bit_number()%(img.nr()-rect.height()));
    return dlib::move_rect(rect, offset);
}

template<typename ImgType>
void randomly_crop_image (ImgType const &img, ImgType &crop, dlib::rand& rnd,
                          size_t width, size_t height,
                          double mins = 0.7, double maxs = 0.85)
{
    using namespace dlib;

    auto const rect = make_random_cropping_rect(img, rnd, mins, maxs);
    extract_image_chip(img, chip_details(rect, chip_dims(height,width)), crop);

    // Also randomly flip the image
    if (rnd.get_random_double() > 0.5){
        crop = dlib::fliplr(crop);
    }

    // And then randomly adjust the colors.
    dlib::apply_random_color_offset(crop, rnd);
}

template<typename ImgType>
void randomly_crop_images(ImgType const &img, dlib::array<ImgType> &crops,
                          dlib::rand& rnd, long num_crops,
                          size_t width, size_t height,
                          double mins = 0.75, double maxs = 0.9)
{
    using namespace dlib;

    std::vector<chip_details> dets;
    for (long i = 0; i < num_crops; ++i)
    {
        auto rect = make_random_cropping_rect(img, rnd, mins, maxs);
        dets.push_back(chip_details(rect, chip_dims(width,height)));
    }

    extract_image_chips(img, dets, crops);

    for (auto&& img : crops)
    {
        // Also randomly flip the image
        if (rnd.get_random_double() > 0.5){
            img = fliplr(img);
        }

        // And then randomly adjust the colors.
        apply_random_color_offset(img, rnd);
    }
}

#endif // RANDOM_CROP_HPP
