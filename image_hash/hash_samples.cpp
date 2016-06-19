#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/img_hash.hpp>
#include <opencv2/imgproc.hpp>

#include <bitset>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace cv;

size_t const outWidth = 15;

void print_header(std::string const &type);

void test_by_class(cv::Mat const &input, cv::Mat const &target);

void test_img_hash(cv::Mat const &input, cv::Mat const &target,
                   cv::Mat &inHash, cv::Mat &targetHash,
                   cv::Ptr<cv::img_hash::ImgHashBase> &algo);

void contrast_attack(cv::Mat const &input);
void gaussian_noise_attack(cv::Mat const &input);
void diff_img_attack(cv::Mat const &input);
void measure_computation_time();
void measure_computation_time(Ptr<img_hash::ImgHashBase> hashFunc,
                              std::string const &msg);
void resize_attack(cv::Mat const &input);
void rotate_attack(cv::Mat const &input);
void saltpepper_noise(cv::Mat const &input);

int main()
{   
    //Mat input = imread("lena.png", cv::IMREAD_GRAYSCALE);
    //contrast_attack(input);
    //diff_img_attack(input);
    //gaussian_noise_attack(input);
    measure_computation_time();
    //resize_attack(input);
    //rotate_attack(input);
    //saltpepper_noise(input);

    return 0;
}

void contrast_attack(cv::Mat const &input)
{
    std::cout<<"contrast attack"<<std::endl;
    print_header("gamma");
    cv::Mat target;
    cv::Mat targetFloat;
    double const gamma[] = {0.25, 0.5, 0.75, 1.25, 1.5, 1.75};
    std::cout.setf(std::ios::right);
    for(size_t i = 0; i != sizeof(gamma)/sizeof(double); ++i){
        input.convertTo(targetFloat, CV_32F);
        targetFloat /= 255;
        cv::pow(targetFloat, 1.0/gamma[i], targetFloat);
        targetFloat *= 255;
        targetFloat.convertTo(target, CV_8U);
        std::cout<<"("<<gamma[i]<<")\t";
        test_by_class(input, target);
    }
}

void diff_img_attack(cv::Mat const &input)
{
    cv::Mat target = cv::imread("001.jpg");
    print_header("diff");
    std::cout.setf(std::ios::right);
    std::cout<<"nothing\t";
    test_by_class(input, target);
}

void gaussian_noise_attack(cv::Mat const &input)
{
    std::cout<<"gaussian noise attack"<<std::endl;
    print_header("stddev");

    cv::Mat noise(input.size(), CV_64F);
    cv::Mat result;
    cv::Mat target;
    double const stddev[] = {0.01, 0.1, 0.5, 1.0, 1.5, 2.0};
    std::cout.setf(std::ios::right);
    for(size_t i = 0; i != sizeof(stddev)/sizeof(double); ++i){
        cv::normalize(input, result, 0, 1, CV_MINMAX, CV_64F);
        cv::randn(noise, 0, 0.05);
        result = result + noise;
        cv::normalize(result, target, 0, 255, CV_MINMAX, CV_8U);
        std::cout<<"("<<stddev[i]<<")\t";
        test_by_class(input, target);
    }
}

void measure_computation_time()
{
    using namespace cv::img_hash;
    measure_computation_time(AverageHash::create(), "Averaga hash computation time");
    measure_computation_time(PHash::create(), "PHash computation time");
    measure_computation_time(MarrHildrethHash::create(), "Marr hash computation time");
    measure_computation_time(RadialVarianceHash::create(), "Radial hash computation time");
    measure_computation_time(BlockMeanHash::create(0), "BMH zero computation time");
    measure_computation_time(BlockMeanHash::create(1), "BMH one computation time");
}

void measure_computation_time(Ptr<img_hash::ImgHashBase> hashFunc,
                              std::string const &msg)
{
    cv::Mat hash;
    std::vector<cv::Mat> imgs(100);
    for(size_t i = 0; i != imgs.size(); ++i)
    {
        //ukbench03000
        imgs[i] = cv::imread("ukbench/ukbench0" +
                             std::to_string(i + 3000) +
                             ".jpg");
    }

    std::cout<<msg<<" : ";
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    for(size_t i = 0; i != 100; ++i)
    {
        hashFunc->compute(imgs[i], hash);
        std::cout<<hash<<"\n";
    }
    end = std::chrono::system_clock::now();
    std::cout<<"elapsed : "<<
               std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()<<std::endl;
}

void resize_attack(cv::Mat const &input)
{
    print_header("resize attack");
    Mat target;
    double const xratio[] = {0.25, 0.25, 0.25, 0.5, 0.5,
                             0.25,  0.5, 0.75, 1.0, 1.0,
                             1.25, 1.25, 1.25};
    double const yratio[] = {0.5,  0.75,  1.0, 0.75, 1.0,
                             0.25, 0.25, 0.25, 0.25, 1.0,
                             1.5,  1.75, 2.0};

    std::cout.setf(std::ios::right);
    for(size_t i = 0; i != sizeof(xratio)/sizeof(double); ++i)
    {
        std::stringstream stream;
        stream<<"("<<xratio[i]<<", "<<yratio[i]<<")\t";
        std::cout<<std::setw(outWidth)<<stream.str();
        cv::resize(input, target, {}, xratio[i], yratio[i]);
        test_by_class(input, target);
    }
}

void rotate_attack(cv::Mat const &input)
{
    print_header("rotate attack");
    std::cout.setf(std::ios::right);
    for(int angle = 0; angle != 360; angle += 15){
        cv::Mat rotImg;
        cv::Point2f const center(input.cols/2.0f, input.rows/2.0f);
        cv::Mat const rotMat =
                cv::getRotationMatrix2D(center,
                                        angle,1.0);
        cv::warpAffine(input, rotImg, rotMat, input.size());
        std::cout<<"angle("<<angle<<")\t";
        test_by_class(input, rotImg);
    }
}

void saltpepper_noise(cv::Mat const &input)
{
    std::cout<<"saltpepper noise attack"<<std::endl;
    print_header("low th,high th");

    cv::Mat noise(input.size(), CV_8U);
    noise = 0;
    cv::randu(noise, 0, 255);
    cv::Mat target;
    uchar const lowTH[] =  { 15,  90,  15,  90,   15};
    uchar const highTH[] = { 90, 180, 180, 250,  225};
    std::cout.setf(std::ios::right);
    for(size_t i = 0; i != sizeof(lowTH)/sizeof(uchar); ++i){
        input.copyTo(target);
        target.setTo(0, noise < lowTH[i]);
        target.setTo(255, noise > highTH[i]);
        std::cout<<"("<<static_cast<size_t>(lowTH[i])
                 <<","<<static_cast<size_t>(highTH[i])<<")\t";
        test_by_class(input, target);
    }
}

void test_img_hash(cv::Mat const &input, cv::Mat const &target,
                   cv::Mat &inHash, cv::Mat &targetHash,
                   cv::Ptr<cv::img_hash::ImgHashBase> &algo)
{
    algo->compute(input, inHash);
    algo->compute(target, targetHash);
    double const mismatch = algo->compare(inHash, targetHash);
    std::cout<<std::setw(outWidth)<<std::right<<mismatch<<"\t";
}

//benefits of using class is potential performance gain, because
//class will reuse the buffer, function will allocate new memory
//every time you call it
void test_by_class(cv::Mat const &input, cv::Mat const &target)
{    
    cv::Mat inHash;
    cv::Mat outHash;

    Ptr<img_hash::ImgHashBase> hashFunc = img_hash::AverageHash::create();
    test_img_hash(input, target,
                  inHash, outHash, hashFunc);

    hashFunc = img_hash::PHash::create();
    test_img_hash(input, target,
                  inHash, outHash, hashFunc);

    hashFunc = img_hash::MarrHildrethHash::create();
    test_img_hash(input, target,
                  inHash, outHash, hashFunc);


    hashFunc = img_hash::RadialVarianceHash::create();
    test_img_hash(input, target,
                  inHash, outHash, hashFunc);

    hashFunc = img_hash::BlockMeanHash::create(0);
    test_img_hash(input, target,
                  inHash, outHash, hashFunc);
    hashFunc.staticCast<img_hash::BlockMeanHash>()->setMode(1);
    test_img_hash(input, target,
                  inHash, outHash, hashFunc);
    std::cout<<std::endl;
}

void print_header(std::string const &type)
{
    std::cout.setf(std::ios::left);
    std::cout<<std::setw(outWidth)<<type<<
               std::setw(outWidth)<<"Average hash\t"<<
               std::setw(outWidth)<<"PHash\t"<<
               std::setw(outWidth)<<"Marr hash\t"<<
               std::setw(outWidth)<<"Radical hash\t"<<
               std::setw(outWidth)<<"BMH zero\t"<<
               std::setw(outWidth)<<"BMH one"<<std::endl;
}
