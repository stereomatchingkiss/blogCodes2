#include "fhog_trainer.hpp"

#include <ocv_libs/ml/utility/split_train_test.hpp>

#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/data_io.h>

#include <algorithm>
#include <thread>

void fhog_number_plate_trainer::preprocess(std::string const &train_xml)
{
    using namespace dlib;
    using namespace ocv::ml;

    load_image_dataset(images_input_, face_boxes_input_,
                       train_xml);

    std::cout<<"input sizes : " <<images_input_.size()<<", "
            <<face_boxes_input_.size()<<std::endl;

    split_train_test_inplace(images_input_, face_boxes_input_,
                             images_train_, face_boxes_train_,
                             images_test_, face_boxes_test_,
                             0.25);

    images_input_.swap(images_train_);
    face_boxes_input_.swap(face_boxes_train_);
    split_train_test_inplace(images_input_, face_boxes_input_,
                             images_train_, face_boxes_train_,
                             images_validate_, face_boxes_validate_,
                             0.1);
    std::cout<<"train sizes : " <<images_train_.size()<<", "
            <<face_boxes_train_.size()<<std::endl;
    std::cout<<"test sizes : " <<images_test_.size()<<", "
            <<face_boxes_test_.size()<<std::endl;
    std::cout<<"validate sizes : " <<images_validate_.size()<<", "
            <<face_boxes_validate_.size()<<std::endl;

    //upsample_image_dataset<pyramid_down<2> >(images_train, face_boxes_train);
    //upsample_image_dataset<pyramid_down<2> >(images_test,  face_boxes_test);

    //add_image_left_right_flips(images_train, face_boxes_train);
}

void fhog_number_plate_trainer::visualize_result(dlib::object_detector<image_scanner_type> &detector)
{
    using namespace dlib;

    image_window win;
    for(size_t i = 0; i < images_test_.size(); ++i)
    {
        // Run the detector and get the number plate detections
        std::vector<rectangle> dets = detector(images_test_[i]);
        std::cout<<i<<" detect "<<dets.size()<<" number plate"<<std::endl;
        for(auto &rect : dets){
            //increase the size of possible plate region, this could
            //help us locate full plate informations
            rectangle const tg(rect.left() - rect.left() * 0.15,
                               rect.top() - rect.top() * 0.05,
                               rect.right() + rect.right() * 0.15,
                               rect.bottom() + rect.bottom() * 0.05);
            rect = tg;
        }
        win.clear_overlay();
        win.set_image(images_test_[i]);
        win.add_overlay(dets, rgb_pixel(255,0,0));
        std::cout << "Hit enter to process the next image..." << std::endl;
        std::cin.get();
    }
}

void fhog_number_plate_trainer::
show_train_result(dlib::object_detector<image_scanner_type> &detector)
{
    using namespace dlib;

    constexpr size_t ratio = 2;
    upsample_image_dataset<pyramid_down<ratio>>(images_train_, face_boxes_train_);
    std::cout << "training results(precision,recall,AP): "
              << test_object_detection_function(detector, images_train_, face_boxes_train_);

    upsample_image_dataset<pyramid_down<ratio>>(images_validate_, face_boxes_validate_);
    std::cout << "validate results(precision,recall,AP): "
              << test_object_detection_function(detector, images_validate_, face_boxes_validate_);

    upsample_image_dataset<pyramid_down<ratio>>(images_test_, face_boxes_test_);
    std::cout << "testing results(precision,recall,AP):  "
              << test_object_detection_function(detector, images_test_, face_boxes_test_);

    //visualize_result(detector);
}

fhog_number_plate_trainer::fhog_number_plate_trainer(int argc, char **argv)
{
    try{
        //get required commands
        auto map = parse_command_line(argc, argv);

        if(!map.count("help")){
            using namespace dlib;

            auto const train_xml = map["train_xml"].as<std::string>();
            preprocess(train_xml);

            object_detector<image_scanner_type> detector = train();
            show_train_result(detector);
            serialize("number_plate_detector.svm") << detector;
            std::cout<<'\a'<<std::endl;
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
             "The xml file of training");

    variables_map vm;
    store(boost::program_options::parse_command_line(argc, argv, desc), vm);

    if(vm.count("help")){
        std::cout<<desc<<std::endl;
        return vm;
    }

    notify(vm);

    return vm;
}

dlib::object_detector<fhog_number_plate_trainer::image_scanner_type>
fhog_number_plate_trainer::train() const
{
    using namespace dlib;

    image_scanner_type scanner;
    scanner.set_detection_window_size(70,20);
    scanner.set_nuclear_norm_regularization_strength(0.08);
    structural_object_detection_trainer<image_scanner_type> trainer(scanner);
    trainer.set_num_threads(std::thread::hardware_concurrency());
    trainer.set_c(12);
    trainer.set_match_eps(0.3);
    //trainer.be_verbose();
    trainer.set_epsilon(0.005);

    return trainer.train(images_train_, face_boxes_train_);
}
