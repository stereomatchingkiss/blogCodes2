#ifndef READ_INPUT_DATA_HPP
#define READ_INPUT_DATA_HPP

#include "../../general_functions/dlib/random_crop.hpp"
#include "../../general_functions/dlib/utility.hpp"

#include <ocv_libs/ml/utility/shuffle_data.hpp>
#include <ocv_libs/ml/utility/split_train_test.hpp>

#include <dlib/data_io.h>
#include <dlib/dir_nav.h>

#include <algorithm>
#include <random>
#include <thread>

struct image_info
{
    image_info(std::string filename, int numeric_label);
    image_info() = default;    

    std::string filename_;
    int numeric_label_ = 0;
};

std::vector<image_info> read_image_info_from_file(std::string const &file);

void read_image_info_from_file(std::string const &file,
                               std::vector<image_info> &output);

template<typename ImgType>
std::vector<ImgType>
read_img(const std::string &input, bool augment,
         size_t width, size_t height, size_t augment_ratio)
{
    std::vector<ImgType> imgs;
    static dlib::rand rnd(time(0));
    ImgType img;
    dlib::load_image(img, input);
    if(img.nc() > 0){
        if(augment){
            dlib::array<ImgType> temp;
            randomly_crop_images(img, temp, rnd, augment_ratio,
                                 width, height, 0.6);
            for(auto &aimg : temp){
                imgs.emplace_back(std::move(aimg));
            }
        }
        ImgType rimg(width, height);
        dlib::resize_image(img, rimg, dlib::interpolate_bilinear());
        imgs.emplace_back(std::move(rimg));
    }

    return {std::move(imgs)};
}

template<typename ImgType, typename LabelType>
std::tuple<std::vector<ImgType>, std::vector<LabelType>>
read_img(const image_info &input, bool augment,
         size_t width, size_t height, size_t augment_ratio)
{
    std::vector<ImgType> imgs;
    std::vector<LabelType> labels;
    static dlib::rand rnd(time(0));
    ImgType img;
    dlib::load_image(img, input.filename_);
    if(img.nc() > 0){
        if(augment){
            dlib::array<ImgType> temp;
            randomly_crop_images(img, temp, rnd, augment_ratio,
                                 width, height, 0.6);
            for(auto &aimg : temp){
                imgs.emplace_back(std::move(aimg));
                labels.emplace_back(input.numeric_label_);
            }
        }
        ImgType rimg(width, height);
        dlib::resize_image(img, rimg, dlib::interpolate_bilinear());
        imgs.emplace_back(std::move(rimg));
        labels.emplace_back(input.numeric_label_);
    }

    return std::make_tuple(std::move(imgs), std::move(labels));
}

template<typename ImgType, typename LabelType>
std::tuple<std::vector<ImgType>, std::vector<LabelType>>
read_img(const std::vector<image_info> &input, bool augment,
         size_t width, size_t height,
         size_t augment_ratio)
{
    std::vector<ImgType> imgs;
    std::vector<LabelType> labels;
    for(auto const &info : input){
        auto temp = read_img<ImgType, LabelType>(info, augment, width, height, augment_ratio);
        std::move(std::begin(std::get<0>(temp)), std::end(std::get<0>(temp)), std::back_inserter(imgs));
        labels.insert(std::end(labels), std::begin(std::get<1>(temp)), std::end(std::get<1>(temp)));
    }

    ocv::ml::shuffles(imgs, labels, std::random_device{}());

    return std::make_tuple(std::move(imgs), std::move(labels));
}

std::vector<image_info> read_img_info(std::string const &folder);

#endif // READ_INPUT_DATA_HPP
