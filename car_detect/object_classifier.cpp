#include "object_classifier.hpp"

#include <opencv2/imgproc.hpp>

#include <fstream>
#include <iostream>

object_classifier::object_classifier(std::string model_txt,
                                     std::string model_bin,
                                     std::string class_name,
                                     cv::Size const &target_size) :
    class_name_(std::move(class_name)),
    model_bin_(std::move(model_bin)),
    model_txt_(std::move(model_txt)),
    target_size_(target_size)
{
    populate_net();
}

void object_classifier::populate_net()
{
    using namespace cv;
    try{
        std::cout<<"import class"<<std::endl;
        Ptr<dnn::Importer> importer =
                dnn::createCaffeImporter(model_txt_, model_bin_);
        if (!importer)
        {
            std::cout << "Can't load network by using the following files: " << std::endl;
            std::cout << "prototxt:   " << model_txt_ << std::endl;
            std::cout << "caffemodel: " << model_bin_ << std::endl;
            exit(-1);
        }
        std::cout<<"populate net"<<std::endl;
        importer->populateNet(net_);
    }catch(std::exception const &ex){
        std::cout<<ex.what()<<std::endl;
    }
}

void object_classifier::populate_net(std::string const &model_txt,
                                     std::string const &model_bin,
                                     std::string const &class_name,
                                     cv::Size const &target_size)
{
    class_name_ = class_name;
    model_bin_ = model_bin;
    model_txt_ = model_txt;
    target_size_ = target_size;
    populate_net();
}

std::tuple<std::vector<std::string> const&,int,double> const
object_classifier::predict(const cv::Mat &img)
{
    using namespace cv;

    std::cout<<"predict"<<std::endl;
    resize(img, resize_img_, target_size_);
    dnn::Blob inputBlob = dnn::Blob(resize_img_);   //Convert Mat to dnn::Blob image batch
    net_.setBlob(".data", inputBlob);       //set the network input
    net_.forward();                         //compute output
    dnn::Blob prob = net_.getBlob("prob");  //gather output of "prob" layer

    int class_id;
    double class_prob;
    get_max_class(prob, &class_id, &class_prob);//find the best class
    read_class_names();
    std::cout << "Best class: #" << class_id << " '" << class_names_.at(class_id) << "'" << std::endl;
    std::cout << "Probability: " << class_prob * 100 << "%" << std::endl;

    return std::make_tuple(std::cref(class_names_),
                           class_id, class_prob * 100);
}

void object_classifier::get_max_class(cv::dnn::Blob &probBlob,
                                      int *class_id, double *class_prob)
{
    using namespace cv;

    Mat probMat = probBlob.matRefConst().reshape(1, 1); //reshape the blob to 1x1000 matrix
    Point classNumber;
    minMaxLoc(probMat, NULL, class_prob, NULL, &classNumber);
    *class_id = classNumber.x;
}

void object_classifier::read_class_names()
{
    class_names_.clear();
    std::ifstream fp(class_name_);
    if (!fp.is_open())
    {
        std::cout << "File with classes labels not found: "
                  << class_name_ << std::endl;
        exit(-1);
    }

    std::string name;
    while(!fp.eof())
    {
        std::getline(fp, name);
        if(name.length()){
            class_names_.emplace_back(name.substr(name.find(' ')+1));
        }
    }
}
