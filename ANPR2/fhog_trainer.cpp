#include "fhog_trainer.hpp"

#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/data_io.h>

#include <thread>

void fhog_number_plate_trainer::preprocess(std::string const &train_xml,
                                           std::string const &test_xml)
{
    using namespace dlib;

    load_image_dataset(images_train, face_boxes_train,
                       train_xml);
    load_image_dataset(images_test, face_boxes_test,
                       test_xml);

    //upsample_image_dataset<pyramid_down<2> >(images_train, face_boxes_train);
    //upsample_image_dataset<pyramid_down<2> >(images_test,  face_boxes_test);

    //add_image_left_right_flips(images_train, face_boxes_train);
}

fhog_number_plate_trainer::fhog_number_plate_trainer(int argc, char **argv)
{
    try{
        //get required commands
        auto map = parse_command_line(argc, argv);

        if(!map.count("help")){
            using namespace dlib;

            auto const train_xml = map["train_xml"].as<std::string>();
            auto const test_xml = map["test_xml"].as<std::string>();;
            preprocess(train_xml, test_xml);

            using image_scanner_type = scan_fhog_pyramid<pyramid_down<10>>;

            image_scanner_type scanner;
            scanner.set_detection_window_size(120, 40);
            //scanner.set_nuclear_norm_regularization_strength(1.0);
            structural_object_detection_trainer<image_scanner_type> trainer(scanner);
            trainer.set_num_threads(std::thread::hardware_concurrency());
            trainer.set_c(12);
            trainer.set_match_eps(0.4);
            //trainer.be_verbose();
            trainer.set_epsilon(0.005);

            object_detector<image_scanner_type> detector =
                    trainer.train(images_train, face_boxes_train);

            std::cout << "training results(precision,recall,AP): "
                      << test_object_detection_function(detector, images_train, face_boxes_train);

            //upsample_image_dataset<pyramid_down<2> >(images_test, face_boxes_test);
            std::cout << "testing results(precision,recall,AP):  "
                      << test_object_detection_function(detector, images_test, face_boxes_test);

            image_window win;
            for(size_t i = 0; i < images_test.size(); ++i)
            {
                // Run the detector and get the number plate detections
                std::vector<rectangle> dets = detector(images_test[i]);
                std::cout<<i<<" detect "<<dets.size()<<" number plate"<<std::endl;
                for(auto &rect : dets){
                    //increase the size of possible plate region, this could
                    //help us locate full plate informations
                    rectangle const tg(std::max(rect.left() - rect.left() * 0.05, 0.0),
                                       std::max(rect.top() - rect.top() * 0.02, 0.0),
                                       rect.right() + rect.right() * 0.15,
                                       rect.bottom());
                    rect = tg;
                }
                win.clear_overlay();
                win.set_image(images_test[i]);
                win.add_overlay(dets, rgb_pixel(255,0,0));
                std::cout << "Hit enter to process the next image..." << std::endl;
                std::cin.get();
            }
            serialize("number_plate_detector.svm") << detector;
        }

    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }
}

boost::program_options::variables_map
fhog_number_plate_trainer::parse_command_line(int argc, char **argv)
{
    using namespace boost::program_options;

    options_description desc{"options"};
    desc.add_options()
            ("help,h", "Help screen")
            ("train_xml,t", value<std::string>()->required(),
             "The xml file of training")
            ("test_xml,T", value<std::string>()->required(),
             "The xml file of testing");

    variables_map vm;
    store(boost::program_options::parse_command_line(argc, argv, desc), vm);

    if(vm.count("help")){
        std::cout<<desc<<std::endl;
        return {};
    }

    notify(vm);

    return vm;
}
