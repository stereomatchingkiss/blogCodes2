#include "mainwindow.hpp"

#include "algo/saliency_image_generator.hpp"

#include <QApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <iostream>
#include <random>

QJsonArray rect_to_points(cv::Rect const &rect)
{
    QJsonArray top_left;
    top_left.push_back(rect.x);
    top_left.push_back(rect.y);

    QJsonArray bottom_right;
    bottom_right.push_back(rect.br().x);
    bottom_right.push_back(rect.br().y);

    QJsonArray points;
    points.push_back(top_left);
    points.push_back(bottom_right);

    return points;
}

void generate_labelme_json(std::string const &save_as,
                           cv::Size const &im_size,
                           QString const &im_path,
                           std::vector<cv::Rect> const &rects)
{
    QJsonObject jobj;
    jobj["version"] = "5.0.1";
    jobj["flags"] = QJsonObject();
    QJsonArray shapes;
    for(auto const &rect : rects){
        QJsonObject obj;
        obj["label"] = "0";
        obj["points"] = rect_to_points(rect);
        obj["group_id"] = QJsonValue();
        obj["shape_type"] = "rectangle";
        obj["flags"] = QJsonObject();
        shapes.push_back(obj);
    }
    jobj["shapes"] = shapes;
    jobj["imagePath"] = im_path;
    jobj["imageData"] = QJsonValue();
    jobj["imageHeight"] = im_size.height;
    jobj["imageWidth"] = im_size.width;

    QFile file(save_as.c_str());
    if(file.open(QIODevice::WriteOnly)){
        file.write(QJsonDocument(jobj).toJson());
    }
}

std::tuple<cv::Mat, cv::Mat> extract_foreground(cv::Mat const &input)
{
    static saliency_image_generator sig("models_saliency/u2net_sim_opt.param", "models_saliency/u2net_sim_opt.bin");
    cv::Mat otsu_mask = sig.generate_otsu_mask(input);
    auto const bboxes = sig.generate_bboxes(otsu_mask);
    cv::cvtColor(otsu_mask, otsu_mask, cv::COLOR_GRAY2BGR);
    std::cout<<otsu_mask.channels()<<", "<<input.channels()<<std::endl;
    std::cout<<otsu_mask.size()<<", "<<input.size()<<std::endl;
    std::cout<<otsu_mask.type()<<", "<<input.type()<<std::endl;
    if(!bboxes.empty()){
        auto const &brect = bboxes[0];
        return {otsu_mask(brect).clone(), input(brect).clone()};
    }

    return {otsu_mask, input};
}

void create_foregrounds()
{
    namespace fs = std::filesystem;
    auto const index = std::to_string(100);
    for(auto const &it : fs::directory_iterator("E:/computer_vision_dataset/coins/world_coins/coins/data/jp_subset_sync/" + index)){
        std::cout<<it.path().filename()<<std::endl;
        auto src = cv::imread(it.path().string());
        cv::Mat mask, rsrc;
        std::tie(mask, rsrc) = extract_foreground(src);
        if(!mask.empty()){
            cv::imwrite("mask/" + index + "/" + it.path().filename().string(), mask);
            cv::imwrite("coins/" + index + "/" + it.path().filename().string(), rsrc);
        }
        cv::destroyAllWindows();
    }
}

cv::Rect generate_random_rect(cv::Size const &src_im_size, cv::Size const &target_img_size)
{
    std::mt19937 gen;
    gen.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> random_x(0, target_img_size.width - src_im_size.width - 1);
    std::uniform_int_distribution<std::mt19937::result_type> random_y(0, target_img_size.height - src_im_size.height - 1);
    return cv::Rect(random_x(gen), random_y(gen), src_im_size.width, src_im_size.height);
}

cv::Rect generate_random_rect_without_intersect(cv::Size const &src_im_size, cv::Size const &target_img_size,
                                                std::vector<cv::Rect> const &exist_rect)
{
    std::mt19937 gen;
    gen.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> random_x(0, target_img_size.width - src_im_size.width - 1);
    std::uniform_int_distribution<std::mt19937::result_type> random_y(0, target_img_size.height - src_im_size.height - 1);
    auto const no_intersect = [&exist_rect](auto const &rect)
    {
        for(auto const &val : exist_rect){
            auto const intersect_rect = rect & val;
            if(intersect_rect.area() != 0){
                return false;
            }
        }

        return true;
    };
    for(size_t i = 0; i != 20; ++i){
        auto const rect = cv::Rect(random_x(gen), random_y(gen), src_im_size.width, src_im_size.height);
        if(no_intersect(rect)){
            return rect;
        }
    }

    return {};
}

void resize_fg_and_bg(cv::Mat &coin_img, cv::Mat &mask_img, cv::Size const &target_size, int coins_size)
{
    if(coin_img.rows > (target_size.height / coins_size) || coin_img.cols > (target_size.width / coins_size)){
        if(coin_img.rows > coin_img.cols){
            cv::resize(coin_img, coin_img, cv::Size(coin_img.rows / coins_size, coin_img.rows / coins_size));
            cv::resize(mask_img, mask_img, cv::Size(mask_img.rows / coins_size, mask_img.rows / coins_size));
        }else{
            cv::resize(coin_img, coin_img, cv::Size(coin_img.cols / coins_size, coin_img.cols / coins_size));
            cv::resize(mask_img, mask_img, cv::Size(mask_img.cols / coins_size, mask_img.cols / coins_size));
        }
    }
}

void resize_aspect_ratio_to_height(cv::Mat const &src, cv::Mat &dst, int height, int interpolation = cv::INTER_NEAREST)
{
    auto const ratio = height / static_cast<double>(src.rows);
    cv::resize(src, dst, cv::Size(static_cast<int>(src.cols) * ratio, height), 0, 0, interpolation);
}

void resize_aspect_ratio_to_width(cv::Mat const &src, cv::Mat &dst, int width, int interpolation = cv::INTER_NEAREST)
{
    auto const ratio = width / static_cast<double>(src.cols);
    cv::resize(src, dst, cv::Size(width, static_cast<int>(src.rows) * ratio), 0, 0, interpolation);
}

void synthesize_random_data()
{
    namespace fs = std::filesystem;

    struct im_path_struct
    {
        im_path_struct(std::string_view im_path, std::string_view mask_path) :
            im_path_(im_path),
            mask_path_(mask_path)
        {}

        std::string im_path_;
        std::string mask_path_;
    };

    std::vector<im_path_struct> coins_urls;
    for(auto const &it : fs::recursive_directory_iterator("coins/")){
        if(fs::is_regular_file(it.path())){
            std::string const im_path = it.path().string();
            coins_urls.emplace_back(im_path, "masks/" + im_path.substr(6));
        }
    }

    std::vector<std::string> bg_urls;
    for(auto const &it : fs::directory_iterator("textures/")){
        bg_urls.emplace_back(it.path().string());
    }

    fs::create_directories("gen_images");
    fs::create_directories("gen_labels");
    std::mt19937 gen;
    gen.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> random_texture(0, bg_urls.size() - 1);
    std::uniform_int_distribution<std::mt19937::result_type> random_coins(0, coins_urls.size() - 1);
    std::uniform_int_distribution<std::mt19937::result_type> random_coins_num(2, 5);
    int constexpr morph_size = 2;
    int constexpr max_dims = 500;
    auto const element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(2 * morph_size + 1, 2 * morph_size + 1));
    for(size_t i = 0; i != 200; ++i){
        std::vector<cv::Rect> rects;
        auto const texture_path = bg_urls[random_texture(gen)];
        //auto const texture_path = bg_urls[1];
        auto texture = cv::imread(texture_path);
        if(texture.rows > texture.cols){
            if(texture.rows > max_dims){
                resize_aspect_ratio_to_height(texture, texture, max_dims);
            }
        }else{
            if(texture.cols > max_dims){
                resize_aspect_ratio_to_width(texture, texture, max_dims);
            }
        }
        auto const coins_size = random_coins_num(gen);
        std::cout<<"texture size = "<<texture.size()<<", coins size = "<<coins_size<<std::endl;
        for(size_t j = 0; j != coins_size; ++j){
            auto const &curl = coins_urls[random_coins(gen)];
            auto coin_img = cv::imread(curl.im_path_);
            auto mask_img = cv::imread(curl.mask_path_, cv::IMREAD_GRAYSCALE);
            cv::morphologyEx(mask_img, mask_img, cv::MORPH_OPEN, element);
            resize_fg_and_bg(coin_img, mask_img, texture.size(), coins_size);
            std::cout<<"sizes of coins = "<<curl.im_path_<<", "<<curl.mask_path_<<std::endl;
            std::cout<<texture.size()<<","<<coin_img.size()<<std::endl;
            if(coin_img.rows < texture.rows && coin_img.cols < texture.cols){
                auto const rect = generate_random_rect_without_intersect(coin_img.size(), texture.size(), rects);
                if(rect.area() > 0){
                    rects.emplace_back(rect);
                    std::cout<<j<<":"<<rect<<std::endl;
                    coin_img.copyTo(texture(rect), mask_img);
                }
            }
        }
        if(!rects.empty()){
            generate_labelme_json("gen_labels/" + std::to_string(i) + ".json",
                                  texture.size(),
                                  QString("../gen_images/%1.jpg").arg(i),
                                  rects);
            cv::imwrite("gen_images/" + std::to_string(i) + ".jpg", texture);
        }
    }
    std::cout<<coins_urls.size()<<std::endl;
}

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    synthesize_random_data();

    //return a.exec();
}
