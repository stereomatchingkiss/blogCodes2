#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/img_hash.hpp>
#include <opencv2/imgproc.hpp>

#include <bitset>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace cv;

std::ofstream outFile("out.txt");

size_t const outWidth = 15;

void print_header(std::string const &type);

void print_header(std::string const &type,
                  std::ostream &out);

void test_by_class(cv::Mat const &input, cv::Mat const &target);

void test_img_hash(cv::Mat const &input, cv::Mat const &target,
                   cv::Mat &inHash, cv::Mat &targetHash,
                   cv::Ptr<cv::img_hash::ImgHashBase> &algo);

void contrast_attack(cv::Mat const &input);
void diff_img_attack(cv::Mat const &input);
void find_target();
void find_target(cv::Ptr<cv::img_hash::ImgHashBase> algo, bool smaller = true);
void gaussian_noise_attack(cv::Mat const &input);
void measure_comparison_time();
std::vector<cv::Mat> measure_comparison_time(Ptr<img_hash::ImgHashBase> hashFunc,
                                             std::string const &msg);
void measure_computation_time();
void measure_computation_time(Ptr<img_hash::ImgHashBase> hashFunc,
                              std::string const &msg);
void resize_attack(cv::Mat const &input);
void rotate_attack(cv::Mat const &input);
void saltpepper_noise(cv::Mat const &input);

int main()
{      
    cv::ocl::setUseOpenCL(false);
    //Mat input = imread("lena.png");
    //contrast_attack(input);
    //diff_img_attack(input);
    //find_target();
    //gaussian_noise_attack(input);
    //measure_comparison_time();
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
    print_header("diff");
    std::cout.setf(std::ios::right);
    std::cout<<"nothing\t";
    size_t index = 3000;
    for(size_t i = 0; i != 100; ++i){
        cv::Mat target = cv::imread("ukbench/ukbench0" +
                                    std::to_string(index + i) +
                                    ".jpg");
        test_by_class(input, target);
    }
}

void find_target()
{
    using namespace cv::img_hash;

    find_target(AverageHash::create());
    find_target(PHash::create());
    find_target(MarrHildrethHash::create());
    find_target(RadialVarianceHash::create(), false);
    find_target(BlockMeanHash::create(0));
    find_target(BlockMeanHash::create(1));
    find_target(ColorMomentHash::create());
}

void find_target(cv::Ptr<cv::img_hash::ImgHashBase> algo, bool smaller)
{
    using namespace cv::img_hash;

    cv::Mat input = cv::imread("ukbench/ukbench03037.jpg");
    //not a good way to reuse the codes by calling
    //measure comparision time, please bear with me
    std::vector<cv::Mat> targets = measure_comparison_time(algo, "");

    double idealValue;
    if(smaller)
    {
        idealValue = std::numeric_limits<double>::max();
    }
    else
    {
        idealValue = std::numeric_limits<double>::min();
    }
    size_t targetIndex = 0;
    cv::Mat inputHash;
    algo->compute(input, inputHash);
    for(size_t i = 0; i != targets.size(); ++i)
    {
        double const value = algo->compare(inputHash, targets[i]);
        if(smaller)
        {
            if(value < idealValue)
            {
                idealValue = value;
                targetIndex = i;
            }
        }
        else
        {
            if(value > idealValue)
            {
                idealValue = value;
                targetIndex = i;
            }
        }
    }
    std::cout<<"mismatch value : "<<idealValue<<std::endl;
    cv::Mat result = cv::imread("ukbench/ukbench0" +
                                std::to_string(targetIndex + 3000) +
                                ".jpg");
    cv::imshow("input", input);
    cv::imshow("found img " + std::to_string(targetIndex + 3000), result);
    cv::waitKey();
    cv::destroyAllWindows();
}

void gaussian_noise_attack(cv::Mat const &input)
{
    std::cout<<"gaussian noise attack"<<std::endl;
    print_header("stddev");

    int const fType = input.channels() == 1 ? CV_64F : CV_64FC3;
    cv::Mat noise(input.size(), fType);
    cv::Mat result;
    cv::Mat target;
    double const stddev[] = {0.01, 0.1, 0.5, 1.0, 1.5, 2.0};
    std::cout.setf(std::ios::right);
    for(size_t i = 0; i != sizeof(stddev)/sizeof(double); ++i){
        cv::normalize(input, result, 0, 1, CV_MINMAX, fType);
        cv::randn(noise, 0, 0.05);
        result += noise;
        int const tType = input.channels() == 1 ? CV_8U : CV_8UC3;
        cv::normalize(result, target, 0, 255, CV_MINMAX, tType);
        std::cout<<"("<<stddev[i]<<")\t";
        test_by_class(input, target);
    }
}

void measure_comparison_time()
{
    using namespace cv::img_hash;
    //measure_comparison_time(AverageHash::create(), "Averaga hash comparison time");
    //measure_comparison_time(PHash::create(), "PHash comparison time");
    //measure_comparison_time(MarrHildrethHash::create(), "Marr hash comparison time");
    //measure_comparison_time(RadialVarianceHash::create(), "Radial hash comparison time");
    //measure_comparison_time(BlockMeanHash::create(0), "BMH zero comparison time");
    //measure_comparison_time(BlockMeanHash::create(1), "BMH one comparison time");
    measure_comparison_time(ColorMomentHash::create(), "Color mean comparison time");
}

std::vector<cv::Mat> measure_comparison_time(Ptr<img_hash::ImgHashBase> hashFunc,
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

    std::vector<cv::Mat> hashes(imgs.size());
    for(size_t i = 0; i != imgs.size(); ++i)
    {
        hashFunc->compute(imgs[i], hash);
        hashes[i] = (hash.clone());
    }

    std::cout<<msg<<" : ";
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    for(size_t i = 0; i != imgs.size(); ++i)
    {
        hashFunc->compare(hashes[0], hashes[i]);
    }
    end = std::chrono::system_clock::now();
    std::cout<<"elapsed : "<<
               std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()<<std::endl;

    return hashes;
}

void measure_computation_time()
{
    using namespace cv::img_hash;
    //measure_computation_time(AverageHash::create(), "Averaga hash computation time");
    //measure_computation_time(PHash::create(), "PHash computation time");
    //measure_computation_time(MarrHildrethHash::create(), "Marr hash computation time");
    //measure_computation_time(RadialVarianceHash::create(), "Radial hash computation time");
    measure_computation_time(BlockMeanHash::create(0), "BMH zero computation time");
    //measure_computation_time(BlockMeanHash::create(1), "BMH one computation time");
    //measure_computation_time(ColorMomentHash::create(), "Color mean computation time");
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
        //std::cout<<hash<<"\n";
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
        outFile<<"angle("<<angle<<")\t";
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
    outFile<<std::setw(outWidth)<<std::right<<mismatch<<"\t";
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

    hashFunc = img_hash::ColorMomentHash::create();
    test_img_hash(input, target,
                  inHash, outHash, hashFunc);
    std::cout<<std::endl;
    outFile<<"\n";
}

void print_header(std::string const &type, std::ostream &out)
{
    out.setf(std::ios::left);
    out<<std::setw(outWidth)<<type<<
         std::setw(outWidth)<<"Average hash\t"<<
         std::setw(outWidth)<<"PHash\t"<<
         std::setw(outWidth)<<"Marr hash\t"<<
         std::setw(outWidth)<<"Radical hash\t"<<
         std::setw(outWidth)<<"BMH zero\t"<<
         std::setw(outWidth)<<"BMH one"<<
         std::setw(outWidth)<<"Color mean\t"<<std::endl;
}

void print_header(std::string const &type)
{
    print_header(type, std::cout);
    print_header(type, outFile);
}
