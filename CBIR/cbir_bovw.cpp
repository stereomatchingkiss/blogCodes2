#include "cbir_bovw.hpp"

#include <ocv_libs/arma/dist_metric.hpp>
#include <ocv_libs/cbir/bovw_hist_creator.hpp>
#include <ocv_libs/cbir/build_inverted_index.hpp>
#include <ocv_libs/cbir/code_book_builder.hpp>
#include <ocv_libs/cbir/color_descriptor.hpp>
#include <ocv_libs/cbir/f2d_descriptor.hpp>
#include <ocv_libs/cbir/spatial_verifier.hpp>
#include <ocv_libs/cbir/searcher.hpp>
#include <ocv_libs/cbir/visualize_feature.hpp>
#include <ocv_libs/core/inverted_index.hpp>
#include <ocv_libs/core/montage.hpp>
#include <ocv_libs/core/crop_images.hpp>
#include <ocv_libs/file/utility.hpp>
#include <ocv_libs/profile/measure.hpp>

#include <boost/filesystem.hpp>

#include <opencv2/features2d.hpp>
#include <opencv2/hdf.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <armadillo>

#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <string>

namespace{

template<typename T>
struct cv_type_map;

template<>
struct cv_type_map<float>
{
    enum{type = CV_32F};
};

template<>
struct cv_type_map<uchar>
{
    enum{type = CV_8U};
};

}

cbir_bovw::cbir_bovw() :
    ifs_("setting.json"),
    isw_(ifs_)
{        
    if(!ifs_.is_open()){
        throw std::runtime_error("cannot open setting.json");
    }
    setting_.ParseStream(isw_);

    //std::cout<<setting_["images_info"].GetString()<<std::endl;
    fi_.open(setting_["images_info"].GetString());
    std::cout<<"can open hdf5"<<std::endl;
    if(setting_["create_hdf5"] == true){
        //create the dataset
        //16 specify the maximum length of the name of image
        //64 specify the size of the features
        //CV_32F specify the type of the features
        fi_.create_dataset(16, 64,
                           cv_type_map<feature_type>::type);
        //buffer size specify how many data of the images
        //should kept in the memory before the class flush
        //them out to the hdf5
        fi_.set_buffer_size(1000);
    }
}

void cbir_bovw::run()
{
    using namespace ocv::time;

    if(setting_["add_data"] == true){
        auto const add_data_duration = measure<>::execution([&]()
        {
            add_data();
        });
        std::cout<<"add_data_duration(msec) : "
                <<add_data_duration<<std::endl;
    }

    //Read the code size array, iterate through different code size
    //Different code size will
    auto const &size_array = setting_["code_size"];
    for(rapidjson::SizeType i = 0;
        i != size_array.Size(); ++i){
        std::cout<<"result of code size "<<
                   size_array[i].GetInt()<<std::endl;

        if(setting_["build_code_book"] == true){
            //measure is a function to calculate the process
            //time of the function
            auto const build_code_book_duration = measure<>::execution([&]()
            {
                build_code_book(size_array[i].GetInt());
            });
            std::cout<<"build_code_book_duration(msec) : "
                    <<build_code_book_duration<<std::endl;
        }

        if(setting_["visualize"] == true){
            auto const visualize_duration = measure<>::execution([&]()
            {
                visualize_code_book(size_array[i].GetInt());
            });
            std::cout<<"visualize_duration(msec) : "
                    <<visualize_duration<<std::endl;
        }

        std::cout<<std::endl;
    }
}

void cbir_bovw::
add_data()
{
    using namespace ocv;

    //use kaze as feature detector and descriptor
    cv::Ptr<cv::KAZE> detector = cv::KAZE::create();
    cv::Ptr<cv::KAZE> descriptor = detector;
    cbir::f2d_detector f2d(detector, descriptor);

    //read the folder path from setting.json
    auto const folder =
            std::string(setting_["img_folder"].GetString());
    //iterate through the image inside the folder,
    //extract features and keypoints
    for(auto const &name : file::get_directory_files(folder)){
        auto const img = read_img(folder + "/" + name);
        if(!img.empty()){
            //find the keypoints and features by detector
            //and descriptor
            auto const result = f2d.get_descriptor(img);
            //first is keypoints, second is features
            fi_.add_features(name, result.first,
                             result.second);
        }else{
            throw std::runtime_error("image is empty");
        }
    }
}

void cbir_bovw::build_code_book(size_t code_size)
{
    ocv::cbir::code_book_builder<feature_type>
            cb(fi_, setting_["features_ratio"].GetDouble(),
            cv_type_map<feature_type>::type);
    cb.create_code_book(arma::uword(code_size),
                        arma::uword(15), true);
    cb.get_code_book().save(setting_["code_book"].GetString() +
            std::string("_") +
            std::to_string(code_size),
            arma::arma_ascii);
}

cv::Mat cbir_bovw::
read_img(const std::string &name, bool to_gray) const
{
    if(to_gray){
        return cv::imread(name, cv::IMREAD_GRAYSCALE);
    }else{
        return cv::imread(name);
    }
}

void cbir_bovw::visualize_code_book(size_t code_size)
{
    using namespace ocv;

    arma::Mat<feature_type> vocab;
    vocab.load(std::string(setting_["code_book"].GetString()) +
            "_" + std::to_string(code_size),
            arma::arma_ascii);

    //create the folder to store the code book if it do not exist
    auto const vocab_dir = "vocab_" + std::to_string(code_size);
    boost::filesystem::create_directory(vocab_dir);

    //find out the features of the code book describe
    cbir::visualize_features<float, feature_type> vf;
    auto const result = vf.find_top_results(fi_, vocab);
    auto const folder =
            std::string(setting_["img_folder"].GetString());
    size_t index = 0;
    for(auto const &vecs : result){
        ocv::montage montage({64,64}, 4, 4);
        //vecs store the keypoints of each image which
        //closest to the code book element want to present
        for(auto const &res : vecs){
            auto const img = cv::imread(folder + "/" + res.img_name_);
            if(!img.empty()){
                auto const pt = res.kp_.pt;
                //crop the image around the key points
                //left 32 pixels, right 32 pixels,
                //up 32 pixels, down 32 pixels
                montage.add_image(ocv::crop_image(img, pt.x,
                                                  pt.y, 32, 32));
            }else{
                throw std::runtime_error("cannot read image");
            }
        }
        cv::imwrite(vocab_dir + "/img_" +
                    std::to_string(index++) + ".jpg",
                    montage.get_montage());
    }
}
