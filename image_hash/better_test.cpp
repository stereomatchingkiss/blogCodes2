#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/img_hash.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/algorithm/string.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace cv;

size_t const outWidth = 15;

std::vector<std::string> const file_names
{
    "average.tsv",
    "phash.tsv",
    "marr_hash.tsv",
    "radial_hash.tsv",
    "bmh_zero.tsv",
    "bmh_one.tsv",
    "color_moment.tsv"
};

std::vector<cv::Ptr<img_hash::ImgHashBase>> algos
{
    img_hash::AverageHash::create(),
            img_hash::PHash::create(),
            img_hash::MarrHildrethHash::create(),
            img_hash::RadialVarianceHash::create(),
            img_hash::BlockMeanHash::create(0),
            img_hash::BlockMeanHash::create(1),
            img_hash::ColorMomentHash::create()
};

void contrast_attack();
void create_overall_result();
void gaussian_blur_attack();
void gaussian_noise_attack();
void jpeg_compression_attack();
void resize_attack();
void rotation_attack();
void salt_and_pepper_noise_attack();
void watermark_attack();

int main()
{      
    cv::ocl::setUseOpenCL(false);

    assert(file_names.size() == algo.size());

    try{
        create_overall_result();

        //contrast_attack();
        //gaussian_blur_attack();
        //gaussian_noise_attack();
        //jpeg_compression_attack();
        //resize_attack();
        //rotation_attack();
        //salt_and_pepper_noise_attack();
        //watermark_attack();
    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<"\n";
    }
}

void contrast_attack(std::vector<double> const &gamma,
                     std::string const &img_location,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                     std::ofstream &out_file);
void contrast_attack(std::ofstream &out_file,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo);

void gaussian_blur_attack(std::vector<double> const &stddev,
                          std::string const &img_location,
                          cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                          std::ofstream &out_file);
void gaussian_blur_attack(std::ofstream &out_file,
                          cv::Ptr<cv::img_hash::ImgHashBase> &algo);

void gaussian_noise_attack(std::vector<double> const &stddev,
                           std::string const &img_location,
                           cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                           std::ofstream &out_file);
void gaussian_noise_attack(std::ofstream &out_file,
                           cv::Ptr<cv::img_hash::ImgHashBase> &algo);

std::string get_phash_img_name(std::string const &location);
std::vector<std::string> get_phash_image();

void jpeg_compression_attack(std::vector<int> const &quality_factor,
                             std::string const &img_location,
                             cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                             std::ofstream &out_file);
void jpeg_compression_attack(std::ofstream &out_file,
                             cv::Ptr<cv::img_hash::ImgHashBase> &algo);

void resize_attack(std::ofstream &out_file,
                   cv::Ptr<cv::img_hash::ImgHashBase> &algo);
//ratio : first is horizontal ratio, second is vertical ratio
void resize_attack(std::vector<std::pair<double, double>> const &ratio,
                   std::string const &img_location,
                   cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                   std::ofstream &out_file);

void rotation_attack(std::ofstream &out_file,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo);
void rotation_attack(std::vector<int> const &angle,
                     std::string const &img_location,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                     std::ofstream &out_file);

void salt_and_pepper_noise_attack(std::ofstream &out_file,
                                  cv::Ptr<cv::img_hash::ImgHashBase> &algo);
//first is lower bound, second is upper bound
void salt_and_pepper_noise_attack(std::vector<std::pair<uchar, uchar>> const &ratio,
                                  std::string const &img_location,
                                  cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                                  std::ofstream &out_file);

void watermark_attack(std::ofstream &out_file,
                      cv::Ptr<cv::img_hash::ImgHashBase> &algo);
void watermark_attack(std::vector<double> const &alpha,
                      std::string const &img_location,
                      cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                      std::ofstream &out_file);

double test_img_hash(cv::Mat const &input, cv::Mat const &target,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo);

void create_overall_result()
{
    std::vector<std::string> const attacks
    {
        "contrast_",
        "gaussian_blur_",
        "gaussian_noise_",
        "jpeg_compression_",
        "resize_",
        "rotation_",
        "salt_pepper_noise_",
        "watermark_"
    };

    std::vector<std::pair<double,double>> const range
    {
        {0,5}, //average
        {0,5}, //phash
        {0,30}, //marr_hash
        {0.9,1.0}, //radial_hash
        {0,12}, //bmh_zero
        {0,48}, //bmh_one
        {0,8}, //color_moment
    };

    std::ofstream out("overall_result.tsv");
    out<<"\t";
    for(auto const &file : file_names){
        out<<file.substr(0, file.rfind("."))<<"\t";
    }
    out<<"\n";

    for(auto const &atk : attacks){
        out<<atk.substr(0, atk.size()-1)<<"\t";
        for(size_t i = 0; i != file_names.size(); ++i){
            std::ifstream in(atk + file_names[i]);
            if(!in.is_open()){
                throw std::runtime_error("cannot open file");
            }
            std::string line;
            std::getline(in, line);
            double pass = 0;
            double fail = 0;
            while(std::getline(in, line)){
                std::vector<std::string> strs;
                boost::split(strs, line, boost::is_any_of("\t"));
                for(size_t j = 1; j != strs.size()-1; ++j){
                    double const val = std::stod(strs[j]);
                    if(val >= range[i].first && val <= range[i].second){
                        ++pass;
                    }else{
                        ++fail;
                    }
                }
            }
            out<<(pass/(pass+fail)*100)<<"\t";
        }
        out<<"\n";
    }
}

void watermark_attack()
{
    for(size_t i = 0; i != file_names.size(); ++i){
        std::ofstream out("watermark_" + file_names[i]);
        watermark_attack(out, algos[i]);
    }
}

void watermark_attack(std::ofstream &out_file,
                      cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    out_file<<"\t";
    std::vector<double> const alpha
    {
        0.2, 0.4, 0.6, 0.8, 1.0
    };
    for(auto const &val : alpha){
        out_file<<val<<"\t";
    }
    out_file<<"\n";

    auto const img_names = get_phash_image();
    for(auto const &name : img_names){
        watermark_attack(alpha, name, algo, out_file);
    }
}

void watermark_attack(std::vector<double> const &alpha,
                      std::string const &img_location,
                      cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                      std::ofstream &out_file)
{
    cv::Mat const input = cv::imread(img_location);
    if(input.empty()){
        throw std::runtime_error("cannot open image");
    }

    cv::Mat watermark = cv::imread("watermark.png");

    //create water mark, 0 means 100% transparent
    //255 means 100% opaque
    std::vector<cv::Mat> channels;
    cv::split(watermark, channels);
    cv::Mat blendMark(watermark.size(), CV_8U);
    blendMark.setTo(255);
    for(auto &ch : channels){
        cv::bitwise_and(ch, blendMark, ch);
    }
    channels.emplace_back(blendMark);
    cv::merge(channels, watermark);

    //append alpha channel into input image
    //because addWeighted need two inputs to
    //have same channels, size and type
    cv::Mat input_alpha(input.size(), CV_8U);
    input_alpha.setTo(255);
    cv::split(input, channels);
    channels.emplace_back(input_alpha);
    cv::Mat input_with_alpha;
    cv::merge(channels, input_with_alpha);


    cv::Mat target;
    cv::Mat overlay(input.size(), CV_8UC4);
    overlay.setTo(0);
    watermark.copyTo(overlay(cv::Rect(0,0,watermark.cols, watermark.rows)));

    out_file<<get_phash_img_name(img_location)<<"\t";
    for(auto &val : alpha){
        cv::addWeighted(input_with_alpha, 1.0,
                        overlay, val, 0, target);
        cv::cvtColor(target, target, CV_BGRA2BGR);
        out_file<<test_img_hash(input, target, algo)<<"\t";
    }
    out_file<<"\n";
}

void jpeg_compression_attack()
{
    for(size_t i = 0; i != file_names.size(); ++i){
        std::ofstream out("jpeg_compression_" + file_names[i]);
        jpeg_compression_attack(out, algos[i]);
    }
}

void jpeg_compression_attack(std::vector<int> const &quality_factor,
                             std::string const &img_location,
                             cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                             std::ofstream &out_file)
{
    cv::Mat const input = cv::imread(img_location);
    if(input.empty()){
        throw std::runtime_error("cannot open image");
    }

    out_file<<get_phash_img_name(img_location)<<"\t";
    for(size_t i = 0; i != quality_factor.size(); ++i){
        std::vector<int> const params
        {CV_IMWRITE_JPEG_QUALITY, quality_factor[i]};
        cv::imwrite("compress.jpg", input, params);
        cv::Mat const target = cv::imread("compress.jpg");
        out_file<<test_img_hash(input, target, algo)<<"\t";
    }
    out_file<<"\n";
}

void jpeg_compression_attack(std::ofstream &out_file,
                             cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    out_file<<"\t";
    std::vector<int> const quality_factor
    {
        10,20,30,40,50,60,70,80,90,100
    };
    for(auto const &val : quality_factor){
        out_file<<val<<"\t";
    }
    out_file<<"\n";

    auto const img_names = get_phash_image();
    for(auto const &name : img_names){
        jpeg_compression_attack(quality_factor, name, algo, out_file);
    }
}

void gaussian_blur_attack()
{
    for(size_t i = 0; i != file_names.size(); ++i){
        std::ofstream out("gaussian_blur_" + file_names[i]);
        gaussian_blur_attack(out, algos[i]);
    }
}

void gaussian_blur_attack(std::vector<double> const &stddev,
                          std::string const &img_location,
                          cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                          std::ofstream &out_file)
{
    cv::Mat const input = cv::imread(img_location);
    if(input.empty()){
        throw std::runtime_error("cannot open image");
    }

    cv::Mat target;
    out_file<<get_phash_img_name(img_location)<<"\t";
    for(size_t i = 0; i != stddev.size(); ++i){
        cv::GaussianBlur(input, target, {3,3}, stddev[i], stddev[i]);
        out_file<<test_img_hash(input, target, algo)<<"\t";
    }
    out_file<<"\n";
}

void gaussian_blur_attack(std::ofstream &out_file,
                          cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    out_file<<"\t";
    std::vector<double> const stddev
    {
        0.3,0.5,0.7,0.9,1.1
    };
    for(auto const &val : stddev){
        out_file<<val<<"\t";
    }
    out_file<<"\n";

    auto const img_names = get_phash_image();
    for(auto const &name : img_names){
        gaussian_blur_attack(stddev, name, algo, out_file);
    }
}

void rotation_attack()
{
    for(size_t i = 0; i != file_names.size(); ++i){
        std::ofstream out("rotation_" + file_names[i]);
        rotation_attack(out, algos[i]);
    }
}

void rotation_attack(std::vector<int> const &angle,
                     std::string const &img_location,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                     std::ofstream &out_file)
{
    cv::Mat const input = cv::imread(img_location);
    if(input.empty()){
        throw std::runtime_error("cannot open image");
    }

    cv::Mat target;
    out_file<<get_phash_img_name(img_location)<<"\t";
    for(size_t i = 0; i != angle.size(); ++i){
        cv::Point2f const center(input.cols/2.0f, input.rows/2.0f);
        cv::Mat const rotMat =
                cv::getRotationMatrix2D(center,
                                        angle[i],1.0);
        cv::warpAffine(input, target, rotMat, input.size());
        out_file<<test_img_hash(input, target, algo)<<"\t";
    }
    out_file<<"\n";
}

void rotation_attack(std::ofstream &out_file,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    out_file<<"\t";
    std::vector<int> const angle
    {
        -15,-45,-90,15,45,90
    };
    for(auto const &val : angle){
        out_file<<val<<"\t";
    }
    out_file<<"\n";

    auto const img_names = get_phash_image();
    for(auto const &name : img_names){
        rotation_attack(angle, name, algo, out_file);
    }
}

void salt_and_pepper_noise_attack(std::ofstream &out_file,
                                  cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    out_file<<"\t";
    std::vector<std::pair<uchar, uchar>> const range
    {
        {15,90},{90,180},{15,180},{90,250},{15,225}
    };
    for(auto const &val : range){
        out_file<<static_cast<int>(val.first)<<","
               <<static_cast<int>(val.second)<<"\t";
    }
    out_file<<"\n";

    auto const img_names = get_phash_image();
    for(auto const &name : img_names){
        salt_and_pepper_noise_attack(range, name, algo, out_file);
    }
}

void salt_and_pepper_noise_attack(std::vector<std::pair<uchar, uchar>> const &range,
                                  std::string const &img_location,
                                  cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                                  std::ofstream &out_file)
{
    cv::Mat const input = cv::imread(img_location);
    if(input.empty()){
        throw std::runtime_error("cannot open image");
    }

    cv::Mat noise(input.size(), CV_8U);
    noise = 0;
    cv::randu(noise, 0, 255);
    cv::Mat target;
    out_file<<get_phash_img_name(img_location)<<"\t";
    for(size_t i = 0; i != range.size(); ++i){
        input.copyTo(target);
        target.setTo(0, noise < range[i].first);
        target.setTo(255, noise > range[i].second);
        out_file<<test_img_hash(input, target, algo)<<"\t";
    }
    out_file<<"\n";
}

void salt_and_pepper_noise_attack()
{
    for(size_t i = 0; i != file_names.size(); ++i){
        std::ofstream out("salt_pepper_noise_" + file_names[i]);
        salt_and_pepper_noise_attack(out, algos[i]);
    }
}

void gaussian_noise_attack(std::vector<double> const &stddev,
                           std::string const &img_location,
                           cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                           std::ofstream &out_file)
{
    cv::Mat const input = cv::imread(img_location);
    if(input.empty()){
        throw std::runtime_error("cannot open image");
    }

    int const fType = input.channels() == 1 ? CV_64F : CV_64FC3;
    cv::Mat noise(input.size(), fType);
    cv::Mat result;
    cv::Mat target;
    out_file<<get_phash_img_name(img_location)<<"\t";
    for(size_t i = 0; i != stddev.size(); ++i)
    {
        cv::normalize(input, result, 0, 1, CV_MINMAX, fType);
        cv::randn(noise, 0, 0.05);
        result += noise;
        int const tType = input.channels() == 1 ? CV_8U : CV_8UC3;
        cv::normalize(result, target, 0, 255, CV_MINMAX, tType);
        out_file<<test_img_hash(input, target, algo)<<"\t";
    }
    out_file<<"\n";
}

void gaussian_noise_attack(std::ofstream &out_file,
                           cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    out_file<<"\t";
    std::vector<double> const
            stddev{0.3, 0.4, 0.5, 0.6, 0.7, 0.9};
    for(auto const &val : stddev){
        out_file<<val<<"\t";
    }
    out_file<<"\n";

    auto const img_names = get_phash_image();
    for(auto const &name : img_names){
        gaussian_noise_attack(stddev, name, algo, out_file);
    }
}

void gaussian_noise_attack()
{
    for(size_t i = 0; i != file_names.size(); ++i){
        std::ofstream out("gaussian_noise_" + file_names[i]);
        gaussian_noise_attack(out, algos[i]);
    }
}

void resize_attack()
{
    for(size_t i = 0; i != file_names.size(); ++i){
        std::ofstream out("resize_" + file_names[i]);
        resize_attack(out, algos[i]);
    }
}

void resize_attack(std::vector<std::pair<double, double>> const &ratio,
                   std::string const &img_location,
                   cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                   std::ofstream &out_file)
{
    cv::Mat const input = cv::imread(img_location);
    if(input.empty()){
        throw std::runtime_error("cannot open image");
    }

    cv::Mat target;
    out_file<<get_phash_img_name(img_location)<<"\t";
    for(size_t i = 0; i != ratio.size(); ++i)
    {
        cv::resize(input, target, {},
                   ratio[i].first,
                   ratio[i].second);
        out_file<<test_img_hash(input, target, algo)<<"\t";
    }
    out_file<<"\n";
}

void resize_attack(std::ofstream &out_file,
                   cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    out_file<<"\t";
    std::vector<std::pair<double, double>> const
            ratio{{0.25,0.25}, {0.25,0.5}, {0.25,0.75},
                  {0.75,0.25}, {0.5,0.25}};
    for(auto const &val : ratio){
        out_file<<val.first<<","<<val.second<<"\t";
    }
    out_file<<"\n";

    auto const img_names = get_phash_image();
    for(auto const &name : img_names){
        resize_attack(ratio, name, algo, out_file);
    }
}

double test_img_hash(cv::Mat const &input, cv::Mat const &target,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    cv::Mat inHash;
    cv::Mat targetHash;
    algo->compute(input, inHash);
    algo->compute(target, targetHash);

    return algo->compare(inHash, targetHash);
}

void contrast_attack()
{    
    for(size_t i = 0; i != file_names.size(); ++i){
        std::ofstream out("contrast_" + file_names[i]);
        contrast_attack(out, algos[i]);
    }
}

void contrast_attack(std::vector<double> const &gamma,
                     std::string const &img_location,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo,
                     std::ofstream &out_file)
{    
    cv::Mat const input = cv::imread(img_location);
    if(input.empty()){
        throw std::runtime_error("cannot open image");
    }

    cv::Mat target;
    cv::Mat targetFloat;
    out_file<<get_phash_img_name(img_location)<<"\t";
    for(size_t i = 0; i != gamma.size(); ++i){
        input.convertTo(targetFloat, CV_32F);
        targetFloat /= 255;
        cv::pow(targetFloat, 1.0/gamma[i], targetFloat);
        targetFloat *= 255;
        targetFloat.convertTo(target, CV_8U);
        out_file<<test_img_hash(input, target, algo)<<"\t";
    }
    out_file<<"\n";
}

void contrast_attack(std::ofstream &out_file,
                     cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    out_file<<"\t";
    std::vector<double> const gamma{0.5, 0.75, 1.25, 1.5, 1.75};
    for(auto const val : gamma){
        out_file<<val<<"\t";
    }
    out_file<<"\n";

    auto const img_names = get_phash_image();
    for(auto const &name : img_names){
        contrast_attack(gamma, name, algo, out_file);
    }
}

std::string get_phash_img_name(std::string const &location)
{
    size_t const pos = location.rfind("/");
    return location.substr(pos+1,
                           location.size() - pos - 5);
}

std::vector<std::string> get_phash_image()
{
    std::ifstream in("phash_misc_loc.txt");
    if(!in.is_open()){
        throw std::runtime_error("Cannot open phash_misc_loc.txt");
    }

    std::vector<std::string> files;
    std::string str;
    while(std::getline(in, str)){
        files.emplace_back(str);
    }

    return files;
}
