#include <opencv2/core.hpp>
#include <ocv_libs/core/resize.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <string>
#include <vector>

enum class ContourType
{
    x_type,
    o_type,
    unknown
};

using Contour = std::vector<cv::Point>;

Contour get_approx_poly(Contour const &contour);

std::string parse_command_line(int argc, char **argv);

cv::Mat preprocess(cv::Mat &color_img);

void print_contour_properties(Contour const &contour);

ContourType recognize_countour_type(Contour const &contour);

void show_contours_properties(cv::Mat const &input,
                              std::vector<Contour> const &contour_vec);

void write_x_and_o(cv::Mat const &input,
                   std::vector<Contour> const &contour_vec);

int main(int argc, char **argv)
{
    auto const ImageName = parse_command_line(argc, argv);
    cv::Mat color_img = cv::imread(ImageName);
    if(!color_img.empty()){
        cv::Mat binary_img = preprocess(color_img);
        cv::imshow("threshold", binary_img);
        cv::waitKey();

        std::vector<Contour> contour_vec;
        //findContours will change the input image, provide a copy if you
        //want to resue original image. In pyimage serach guru,
        //it use CV_RETR_EXTERNAL, but this will omit inner
        //contour, to detect center X we need to retrieve deeper contour
        cv::findContours(binary_img,
                         contour_vec, CV_RETR_CCOMP,
                         CV_CHAIN_APPROX_SIMPLE);

        //show the properties of contours one by one
        show_contours_properties(color_img, contour_vec);

        //after you observe the information of contour properties
        //you can, it is time to recognize the x and o
        write_x_and_o(color_img, contour_vec);
    }else{
        std::cerr<<"cannot open image "<<ImageName<<"\n";
    }
}

Contour get_approx_poly(Contour const &contour)
{
    Contour poly_contour;
    double const Epsillon = cv::arcLength(contour, true) * 0.02;
    bool const Close = true;
    cv::approxPolyDP(contour, poly_contour, Epsillon, Close);

    return poly_contour;
}

std::string parse_command_line(int argc, char **argv)
{
    using namespace boost::program_options;

    try
    {
        options_description desc{"Options"};
        desc.add_options()
                //set up --help or -h to show help information
                ("help,h", "Help screen")
                //set up --image or -i as a required argument to store the name of image
                ("image,i", value<std::string>()->required(), "Image to process");

        //parse the command line and store it in containers
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")){
            std::cout << desc << '\n';
            return {};
        }else{
            return vm["image"].as<std::string>();
        }

        //the magic to make the other required arguments
        //become optional if the users input "help"
        notify(vm);
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
    }

    return {};
}

cv::Mat preprocess(cv::Mat &color_img)
{
    //resize the image to width 480 and keep the aspect ratio
    ocv::resize_aspect_ratio(color_img, color_img, {480, 0});

    //find coutours need a binary image, so we must transfer the origin img
    //to binary image.
    cv::Mat gray_img;
    cv::cvtColor(color_img, gray_img, CV_BGR2GRAY);

    //this would not copy the value of gray_img but create a new header
    //I declare an alias to make code easier to read
    cv::Mat binary_img = gray_img;
    cv::threshold(gray_img, binary_img, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
    cv::Mat const Kernel =
            cv::getStructuringElement( cv::MORPH_RECT, {5,5});
    cv::morphologyEx(binary_img, binary_img, cv::MORPH_CLOSE, Kernel);

    return binary_img;
}

void print_contour_properties(Contour const &contour)
{
    double const ContourArea = cv::contourArea(contour);

    Contour convex_hull;
    cv::convexHull(contour, convex_hull);
    double const ConvexHullArea = cv::contourArea(convex_hull);

    auto const BoundingRect = cv::boundingRect(contour);
    Contour const Poly = get_approx_poly(contour);

    std::cout<<"Contour area : "<<ContourArea<<std::endl;
    std::cout<<"Aspect ratio : "<<
               (BoundingRect.width / static_cast<double>(BoundingRect.height))
            <<std::endl;
    std::cout<<"Extend : "<<ContourArea/BoundingRect.area()<<std::endl;
    std::cout<<"Solidity : "<<ContourArea/ConvexHullArea<<std::endl;
    std::cout<<"Poly size : "<<Poly.size()<<", is convex "<<std::boolalpha<<
               cv::isContourConvex(Poly)<<std::endl;
    std::cout<<std::endl;
}

ContourType recognize_countour_type(Contour const &contour)
{
    double const ContourArea = cv::contourArea(contour);

    Contour const Poly = get_approx_poly(contour);

    ContourType type = ContourType::unknown;
    if((Poly.size() > 7 && Poly.size() < 10) && ContourArea > 1000){
        type = ContourType::o_type;
    }else if(Poly.size() >= 10 && ContourArea < 10000){
        type = ContourType::x_type;
    }

    return type;
}

void show_contours_properties(cv::Mat const &input,
                              std::vector<Contour> const &contour_vec)
{
    cv::Mat input_cpy;
    for(size_t i = 0; i != contour_vec.size(); ++i){
        input.copyTo(input_cpy);
        cv::Scalar const Color{255};
        int const ThickNess = 3;
        cv::drawContours(input_cpy, contour_vec,
                         static_cast<int>(i), Color, ThickNess);
        print_contour_properties(contour_vec[i]);

        auto const WindowName = "contour " + std::to_string(i);
        cv::imshow(WindowName, input_cpy);
        cv::waitKey();
        //without this line, previous contour image will not be closed
        //before the program end
        cv::destroyWindow(WindowName);
    }
}

void write_x_and_o(cv::Mat const &input,
                   std::vector<Contour> const &contour_vec)
{
    cv::Mat input_cpy = input.clone();
    std::string const TypeName[] = {"X", "O"};

    for(size_t i = 0; i != contour_vec.size(); ++i){
        ContourType type = recognize_countour_type(contour_vec[i]);

        if(type != ContourType::unknown){
            cv::Scalar const Color{255};
            int const ThickNess = 3;
            cv::drawContours(input_cpy, contour_vec, static_cast<int>(i),
                             Color, ThickNess);
            auto point = cv::boundingRect(contour_vec[i]).tl();
            point.y -= 10;
            double const Scale = 1;
            cv::putText(input_cpy, TypeName[static_cast<int>(type)],
                    point, cv::FONT_HERSHEY_COMPLEX, Scale, Color,
                    ThickNess);
        }
    }
    cv::imshow("result", input_cpy);
    cv::waitKey();
}
