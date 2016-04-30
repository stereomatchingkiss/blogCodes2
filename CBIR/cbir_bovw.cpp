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

using hist_type = arma::Mat<cbir_bovw::feature_type>;
using invert_value_type = arma::uword;
using invert_index =
ocv::inverted_index<arma::uword, invert_value_type>;

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

template<typename T>
T read_mat(rapidjson::Document const &setting,
           std::string const &file_name,
           size_t code_size)
{
    T mat;
    mat.load(setting[file_name.c_str()].GetString() +
            std::string("_") +
            std::to_string(code_size));

    return mat;
}

//api of this function is suck, but I think it is
//acceptable in this small case
template<typename Searcher, typename BOVW, typename SPatialVefirier>
double measure_impl(Searcher &searcher,
                    ocv::cbir::f2d_detector &f2d,
                    BOVW const &bovw,
                    hist_type const &hist,
                    arma::Mat<cbir_bovw::feature_type> const &code_book,
                    arma::Mat<float> const &idf_cf,
                    SPatialVefirier const &spv,
                    rapidjson::Document const &doc,
                    rapidjson::Document const &setting)
{
    double total_score = 0;
    auto const folder =
            std::string(setting["img_folder"].GetString());

    bool const use_idf = setting["use_idf"].GetBool();
    bool const use_spatial_info =
            setting["use_spatial_info"].GetBool();
    auto const files = ocv::file::get_directory_files(folder);
    for(int i = 0; i != files.size(); ++i){
        std::cout<<"target "<<i<<std::endl;
        cv::Mat gray =
                cv::imread(folder + "/" + files[i],
                           cv::IMREAD_GRAYSCALE);
        //f2d.get_descriptor is the bottle neck
        //of the program, more than 85% of computation
        //times come by it
        auto describe =
                f2d.get_descriptor(gray);
        arma::Mat<cbir_bovw::feature_type> const
                arma_features(describe.second.ptr<cbir_bovw::feature_type>(0),
                              describe.second.cols,
                              describe.second.rows,
                              false);
        auto target_hist =
                bovw.describe(arma_features,
                              code_book);
        if(use_idf){
            target_hist %= idf_cf;
        }
        auto result =
                searcher.search(target_hist, hist);
        if(use_spatial_info){
            auto const sort_res =
                    spv.rerank(describe.first,
                               describe.second,
                               result);
            std::copy(std::begin(sort_res), std::end(sort_res),
                      std::begin(result));
        }
        auto const &value = doc[files[i].c_str()];
        std::set<std::string> relevant;
        for(rapidjson::SizeType j = 0;
            j != value.Size(); ++j){
            relevant.insert(value[j].GetString());
        }
        size_t cur_score = 0;
        for(size_t j = 0; j != relevant.size(); ++j){
            auto it = relevant.find(files[result[j]]);
            if(it != std::end(relevant)){
                ++total_score;
                ++cur_score;
            }
        }
        //std::cout<<"cur score : "<<cur_score<<"\n";//*/
    }

    return total_score;
}

}

cbir_bovw::cbir_bovw() :
    ifs_("setting.json"),
    isw_(ifs_)
{        
    //std::cout<<"enter constructor"<<std::endl;
    if(!ifs_.is_open()){
        throw std::runtime_error("cannot open setting.json");
    }
    setting_.ParseStream(isw_);

    std::cout<<setting_["images_info"].GetString()<<std::endl;
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
    std::cout<<"run start"<<std::endl;

    if(setting_["add_data"] == true){
        auto const add_data_duration = measure<>::execution([&]()
        {
            add_data();
        });
        std::cout<<"add_data_duration(msec) : "
                <<add_data_duration<<std::endl;
    }
    std::cout<<"pass add data"<<std::endl;

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

        if(setting_["create_hist"] == true){
            auto const build_hist_duration = measure<>::execution([&]()
            {
                build_bovw_hist(size_array[i].GetInt());
            });
            std::cout<<"build_hist_durationbuild_hist_duration(msec) : "
                    <<build_hist_duration<<std::endl;
        }

        if(setting_["create_inverted_index"] == true){
            auto const build_invert_index_duration =
                    measure<>::execution([&]()
            {
                build_inverted_index(size_array[i].GetInt());
            });
            std::cout<<"build_invert_index_duration(msec) : "
                    <<build_invert_index_duration<<std::endl;
        }

        if(setting_["measure_result"] == true){
            auto const measure_duration =
                    measure<>::execution([&]()
            {
                measure_result(size_array[i].GetInt());
            });
            std::cout<<"measure_duration(msec) : "
                    <<measure_duration<<std::endl;
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

void cbir_bovw::build_bovw_hist(size_t code_size)
{
    using namespace ocv::cbir;
    using hist_creator =
    bovw_hist_creator<
    feature_type, feature_type,
    feature_type, arma::Mat
    >;

    arma::Mat<feature_type> code_book;
    code_book.load(setting_["code_book"].GetString() +
            std::string("_") +
            std::to_string(code_size),
            arma::arma_ascii);

    hist_creator bh(fi_);
    auto const hist = bh.create(code_book);
    hist.save(setting_["hist"].GetString() +
            std::string("_") +
            std::to_string(code_size));
}

void cbir_bovw::build_inverted_index(size_t code_size)
{
    hist_type hist;
    hist.load(setting_["hist"].GetString() +
            std::string("_") +
            std::to_string(code_size));

    invert_index invert;
    ocv::cbir::build_inverted_index(hist, invert);
    invert.save(setting_["inverted_index"].GetString() +
            std::string("_") +
            std::to_string(code_size));
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

rapidjson::Document cbir_bovw::read_relevant_json() const
{
    //the format of relevant.json should same as the one
    //come with ukbench data set
    std::ifstream ifs(setting_["relevant"].GetString());
    if(!ifs.is_open()){
        std::cout<<"cannot open relevant file : "<<
                   setting_["relevant"].GetString()
                <<std::endl;
        throw std::runtime_error("cannot open relevant json file");
    }

    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);

    return doc;
}

void cbir_bovw::measure_result(size_t code_size)
{
    auto const iv = read_mat<invert_index>(setting_,
                                           "inverted_index", code_size);

    auto hist = read_mat<hist_type>(setting_,
                                    "hist", code_size);

    //Reuse the data from histogram would be faster to compute.
    //I prefer to extract the features of the image again, because
    //I want to measure computation time of whole process
    cv::Ptr<cv::KAZE> detector = cv::KAZE::create();
    cv::Ptr<cv::KAZE> descriptor = detector;
    ocv::cbir::f2d_detector f2d(detector, descriptor);

    auto const code_book =
            read_mat<arma::Mat<feature_type>>(setting_,
                                              "code_book", code_size);
    arma::Mat<float> idf_cf;
    if(setting_["use_idf"] == true){
        std::map<arma::uword, float> idf;
        ocv::cbir::convert_to_idf(iv, hist.n_cols, idf);
        idf_cf.set_size(hist.n_rows, 1);
        for(auto const &val : idf){
            idf_cf[val.first] = val.second;
        }

        for(arma::uword i = 0; i != hist.n_cols; ++i){
            hist.col(i) %= idf_cf;
        }
    }

    ocv::cbir::spatial_verifier<
            arma::Mat<feature_type>,
            feature_type> spv(fi_, code_book);

    double total_score = 0;
    if(setting_["use_idf"] == true){
        ocv::cbir::searcher<invert_index,
                ocv::armd::cosine_similarity<feature_type>,
                std::greater<feature_type>>
                searcher(iv, 16);
        total_score = measure_impl(searcher, f2d,
                                   ocv::cbir::bovw<feature_type, hist_type>(),
                                   hist, code_book,
                                   idf_cf, spv, read_relevant_json(),
                                   setting_);
    }else{
        ocv::cbir::searcher<invert_index> searcher(iv, 16);
        total_score = measure_impl(searcher, f2d,
                                   ocv::cbir::bovw<feature_type, hist_type>(),
                                   hist, code_book,
                                   idf_cf, spv, read_relevant_json(),
                                   setting_);
    }
    //raw 3.044,
    //raw + idf 3.035
    //raw + spatial 3.082
    //raw + idf + spatial 3.13
    static std::ofstream out("result.txt");
    std::cout<<code_size<<" : "<<total_score / 1000.0<<std::endl;
    out<<code_size<<" : "<<total_score / 1000.0<<std::endl;//*/
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
