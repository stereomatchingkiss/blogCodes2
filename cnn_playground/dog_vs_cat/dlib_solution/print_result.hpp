#ifndef PRINT_RESULT_HPP
#define PRINT_RESULT_HPP

#include "global_var.hpp"

#include <string>
#include <vector>

template<typename Net, typename ImgType, typename LabelType>
void print_test_result(std::vector<LabelType> const &id,
                       std::vector<ImgType> const &imgs,
                       std::string const &net_name,
                       Net &net)
{
    std::ofstream out(net_name + ".txt");
    out<<"id,label"<<std::endl;

    for(size_t i = 0; i != id.size(); ++i){
        out<<id[i]<<","<<dlib::mat(net(imgs[i]))(1)<<std::endl;
    }
}

template<typename ImgType, typename LabelType>
void print_train_result(std::vector<ImgType> const &imgs,
                        std::vector<LabelType> const &labels,
                        std::vector<LabelType> const &predict_labels,
                        std::string const &msg)
{
    size_t is_cat_and_right = 0;
    size_t is_dog_and_right = 0;
    size_t should_be_dog = 0;
    size_t should_be_cat = 0;
    for (size_t i = 0; i < imgs.size(); ++i)
    {
        if(predict_labels[i] == labels[i]){
            labels[i] == global::cat_label ? ++is_cat_and_right : ++is_dog_and_right;
        }else{
            labels[i] == global::cat_label ? ++should_be_dog : ++should_be_cat;
        }
    }
    size_t const total = is_cat_and_right+ is_dog_and_right +
            should_be_dog + should_be_cat;

    using namespace std;
    cout << msg + " is_cat_and_right: " << is_cat_and_right << endl;
    cout << msg + " is_dog_and_right: " << is_dog_and_right << endl;
    cout << msg + " should_be_dog: " << should_be_dog << endl;
    cout << msg + " should_be_cat: " << should_be_cat << endl;
    cout << msg + " cat true positive: "
         <<(is_cat_and_right)/double(is_cat_and_right + should_be_dog)<< endl;
    cout << msg + " dog true positive: "
         <<(is_dog_and_right)/double(is_dog_and_right + should_be_cat)<< endl;
    cout << msg + " accuracy:  "
         << (is_cat_and_right + is_dog_and_right)/(double)(total) << endl;
}

#endif // PRINT_RESULT_HPP
