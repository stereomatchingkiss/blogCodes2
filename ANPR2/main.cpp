#include "anpr_recognizer.hpp"
#include "bbps_char_recognizer.hpp"
#include "croatia_general_pattern_recognizer.hpp"
#include "fhog_trainer.hpp"
#include "grab_character.hpp"
#include "morphology_localizer.hpp"
#include "prune_illegal_chars.hpp"
#include "segment_character.hpp"
#include "train_chars.hpp"
#include "utility.hpp"

#include <ocv_libs/cmd/command_prompt_utility.hpp>
#include <ocv_libs/core/resize.hpp>
#include <ocv_libs/file/utility.hpp>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>

#include <boost/filesystem.hpp>

#include <array>
#include <future>
#include <iostream>
#include <thread>

using vmap = boost::program_options::variables_map;

template<typename UnaryFunctor>
void test_algo(vmap const &map, UnaryFunctor functor);

void test_anpr_recognizer(int argc, char **argv);
void test_bbps_char_recognizer(int argc, char **argv);
void test_croatia_general_recognizer();
void test_grab_char(int argc, char **argv);
void test_number_plate_localizer(int argc, char **argv);
void test_prune_illegal_chars(int argc, char **argv);
void test_segment_character(int argc, char **argv);
void test_train_accuracy(int argc, char **argv);
void test_train_chars(int argc, char **argv);

int main(int argc, char **argv)
{                   
    try{
        //fhog_number_plate_trainer fhog_trainer(argc, argv);

        //test_anpr_recognizer(argc, argv);
        //test_bbps_char_recognizer(argc, argv);
        //test_croatia_general_recognizer();
        //test_grab_char(argc, argv);
        //test_number_plate_localizer(argc, argv);
        //test_prune_illegal_chars(argc, argv);
        //test_segment_character(argc, argv);
        test_train_chars(argc, argv);
        //test_train_accuracy(argc, argv);
    }catch(std::exception const &ex){
        std::cout<<ex.what()<<std::endl;
    }
}

template<typename BinaryFunctor>
void test_algo(vmap const &map, BinaryFunctor functor)
{
    if(map.count("image_folder")){
        auto const target = map["image_folder"].as<std::string>();
        auto images = ocv::file::get_directory_files(target);
        for(auto const &im_name : images){
            std::cout<<(target + "/" + im_name)<<std::endl;
            cv::Mat input = cv::imread(target + "/" + im_name);
            if(!input.empty()){
                boost::filesystem::path path(im_name);
                functor(input, path.stem().string());
            }
        }
    }else if(map.count("image")){
        auto const target = map["image"].as<std::string>();
        std::cout<<target<<std::endl;
        cv::Mat input = cv::imread(target);
        if(!input.empty()){
            boost::filesystem::path path(target);
            functor(input, path.stem().string());
        }
    }else{
        std::cerr<<"must specify image_folder or image"<<std::endl;
    }
}

void test_anpr_recognizer(int argc, char **argv)
{
    using croatia_general_plate_recognizer = anpr_recognizer<
    morphology_localizer,
    segment_character,
    prune_illegal_chars,
    bbps_char_recognizer,
    croatia_general_pattern_recognizer>;

    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    cv::Ptr<cv::ml::StatModel> ml = cv::ml::SVM::create();
    ml->read(cv::FileStorage("train_result/chars_classifier.xml",
                             cv::FileStorage::READ).root());
    bbps_char_recognizer bcr(ml);

    croatia_general_plate_recognizer
            cr(morphology_localizer(), segment_character(),
               prune_illegal_chars(), bcr,
               croatia_general_pattern_recognizer());

    test_algo(map, [&](cv::Mat const &input, std::string const &img_name)
    {
        auto results = cr.recognize(input);
        for(size_t i = 0; i != results.size(); ++i){
            std::cout<<results[i].first<<std::endl;
            auto img = input.clone();
            auto const rect = results[i].second;
            cv::rectangle(img, rect, {0,255,0}, 2);
            cv::putText(img, results[i].first, {rect.x - rect.x/5, rect.y - 30},
                        cv::FONT_HERSHEY_COMPLEX, 1.0, {0,255,0}, 2);
            cv::imshow("plate", img);
            cv::imshow("binary_plate", cr.get_binary_plate()[i]);
            ocv::resize_aspect_ratio(img, img, {320,0});
            cv::imwrite("recognize_result/" + img_name + ".jpg", img);
            cv::waitKey();
            cv::destroyAllWindows();
        }
    });
}

void test_bbps_char_recognizer(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    segment_character sc;
    prune_illegal_chars plc;
    cv::Ptr<cv::ml::StatModel> ml = cv::ml::RTrees::create();
    ml->read(cv::FileStorage("train_result/chars_classifier.xml",
                             cv::FileStorage::READ).root());
    bbps_char_recognizer bcr(ml);
    test_algo(map, [&](cv::Mat const &input, std::string const&)
    {
        lpl.localize_plate(input);
        for(auto const &contour : lpl.get_contours()){
            if(sc.detect_characters(lpl.get_resize_input(),
                                    contour)){
                auto &char_contours = sc.get_chars_contours();
                auto const &bird_eyes_plate = sc.get_bird_eyes_plate();
                plc.prune(bird_eyes_plate, char_contours);
                auto const &binary_plate = sc.get_binary_plate();
                if(char_contours.size() >= sc.get_min_char_num()){
                    for(size_t i = 0; i != char_contours.size(); ++i){
                        auto const rect = cv::boundingRect(char_contours[i]);
                        std::cout<<bcr.recognize(bird_eyes_plate(rect),
                                                 binary_plate(rect))<<std::endl;
                        auto mat = bird_eyes_plate.clone();
                        cv::drawContours(mat, char_contours, i, {0,255,0}, 2);
                        cv::imshow("plate", mat);
                        cv::imshow("char",binary_plate(rect));
                        cv::waitKey();
                        cv::destroyAllWindows();
                    }
                }
            }else{
                std::cout<<"not a license plate"<<std::endl;
            }
        }
    });//*/
}

void test_croatia_general_recognizer()
{
    croatia_general_pattern_recognizer cgpr;
    std::cout<<"ZG7029M, "<<cgpr.fit("ZG7029M")<<std::endl;
    std::cout<<"AB702M, "<<cgpr.fit("AB702M")<<std::endl;
    std::cout<<"A8702M, "<<cgpr.fit("A8702M")<<std::endl;
    std::cout<<"AB702ML, "<<cgpr.fit("AB702ML")<<std::endl;
    std::cout<<"HRZG7029M, "<<cgpr.fit("HRZG7029M")<<std::endl;
    std::cout<<"ZGM7029M, "<<cgpr.fit("ZGM7029M")<<std::endl;
    std::cout<<"ZGM7029ML, "<<cgpr.fit("ZGM7029ML")<<std::endl;
}

void test_grab_char(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    segment_character sc;
    //sc.set_show_debug_message(true);
    grab_character grab_char;
    test_algo(map, [&](cv::Mat const &input,
              std::string const &name)
    {
        lpl.localize_plate(input);
        auto const &contours = lpl.get_contours();
        for(size_t i = 0; i != contours.size(); ++i){
            sc.set_img_name(name + "_" + std::to_string(i));
            sc.detect_characters(lpl.get_resize_input(),
                                 contours[i]);
            if(sc.get_chars_contours().size() >= 6){
                grab_char.set_chars_name(name);
                grab_char.grab_chars(sc.get_bird_eyes_plate(),
                                     sc.get_chars_contours());
            }
        }
    });
}

void test_number_plate_localizer(int argc, char **argv)
{    
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    lpl.set_show_debug_message(true);
    test_algo(map, [&](cv::Mat const &input, std::string const&)
    {
        lpl.localize_plate(input);
    });
}

void test_prune_illegal_chars(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    segment_character sc;
    prune_illegal_chars plc;
    plc.set_show_debug_message(true);
    test_algo(map, [&](cv::Mat const &input, std::string const&)
    {
        lpl.localize_plate(input);
        for(auto const &contour : lpl.get_contours()){
            if(sc.detect_characters(lpl.get_resize_input(),
                                    contour)){
                plc.prune(sc.get_bird_eyes_plate(), 8,
                          sc.get_chars_contours());
            }else{
                std::cout<<"not a license plate"<<std::endl;
            }
        }
    });
}

void test_segment_character(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    morphology_localizer lpl;
    lpl.set_show_debug_message(false);
    segment_character sc;
    sc.set_show_debug_message(true);
    test_algo(map, [&](cv::Mat const &input,
              std::string const &name)
    {
        lpl.localize_plate(input);
        auto const &contours = lpl.get_contours();
        for(size_t i = 0; i != contours.size(); ++i){
            sc.set_img_name(name + "_" + std::to_string(i));
            sc.detect_characters(lpl.get_resize_input(),
                                 contours[i]);
        }
    });
}

void test_train_accuracy(int argc, char **argv)
{
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;

    if(map.count("image_folder")){
        using namespace ocv::file;
        auto const img_folder = map["image_folder"].as<std::string>();
        auto const folders = get_directory_folders(img_folder);

        auto func = [&](std::string const &train_file, map_type mtype)
        {
            boost::bimap<std::string, int> bm;
            std::map<std::string, int> bingo;
            ocv::block_binary_pixel_sum<> bbps;
            generate_map(bm, mtype);
            generate_map(bingo, mtype);
            for(auto &pair : bingo){
                pair.second = 0;
            }
            cv::Ptr<cv::ml::StatModel> ml = cv::ml::SVM::create();
            ml->read(cv::FileStorage(train_file,
                                     cv::FileStorage::READ).root());
            double total_accuracy = 0;
            for(size_t i = 0; i != folders.size(); ++i){
                auto const folder = img_folder + "/" + folders[i];
                auto const files = get_directory_files(folder);
                for(size_t j = 0; j != files.size(); ++j){
                    auto img = cv::imread(folder+"/"+files[j]);
                    if(!img.empty()){
                        auto const feature = bbps.describe(binarize_image(img));
                        int const label =
                                static_cast<int>(ml->predict(feature));
                        auto it = bm.right.find(label);
                        if(it != std::end(bm.right) &&
                                it->second == folders[i]){
                            ++bingo[it->second];
                        }
                    }
                }
                if(bingo.find(folders[i]) != std::end(bingo) &&
                        !files.empty()){
                    double const accuracy = bingo[folders[i]]/
                            static_cast<double>(files.size());
                    total_accuracy += accuracy;
                    std::cout<<folders[i]<<", "<<accuracy<<std::endl;
                }
            }
            std::cout<<"total accuracy = "
                    <<total_accuracy/static_cast<double>(bingo.size())<<std::endl;
        };
        func("train_result/num.xml", map_type::number);
        func("train_result/alphabet.xml", map_type::alpahbet);

    }else{
        std::cout<<"must specify image folder"<<std::endl;
    }
}

void test_train_chars(int argc, char **argv)
{    
    auto const map =
            ocv::cmd::default_command_line_parser(argc, argv).first;
    if(map.count("image_folder")){
        auto func = [&](std::string result_file, map_type mtype)
        {
            train_chars tc(map["image_folder"].as<std::string>(),
                    std::move(result_file), mtype);
            tc.train();
            tc.test_train_result();
        };
        auto f1 = std::async(func, "train_result/num.xml", map_type::number);
        auto f2 = std::async(func, "train_result/alphabet.xml", map_type::alpahbet);
        f1.wait();
        f2.wait();
    }else{
        std::cout<<"must speficy --image_folder"<<std::endl;
    }
}
