#include "../fps_estimator.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

using namespace cv;
using namespace std;

struct frame_pack
{
    Mat first_frame;
    Mat first_frame_gray;
    Mat first_frame_resize;
    Mat input_frame;
    Mat input_frame_resize;
    Mat input_frame_gray;
    Mat frame_delta;
    Mat thresh;
};

struct js_params
{
    std::string background_image;
    int capture_background_at_x_frame;
    Size dilate_kernal_size;
    Size gaussian_kernal_size;
    double min_countour_area;
    Size resize_to;
    int resize_width;
    double thresh_val;
    int wait_between_frame_ms;
};

cv::VideoCapture create_video_capture(cv::FileStorage const &input)
{
    if(input["input_media"].isInt()){
        return cv::VideoCapture(static_cast<int>(input["input_media"].real()));
    }

    return cv::VideoCapture(input["input_media"].string());
}

int draw_contours(js_params const &params, vector<vector<Point>> const &contours, frame_pack &pack)
{
    int people_count = 0;
    for(auto const &contour : contours){
        auto const carea = contourArea(contour);
        if(carea >= params.min_countour_area){
            ++people_count;
            auto const brect = boundingRect(contour);
            cv::rectangle(pack.input_frame_resize, brect, {255, 0, 0}, 2);
            cv::putText(pack.input_frame_resize, std::to_string(carea), Point(brect.x, brect.y - 10),
                        FONT_HERSHEY_SIMPLEX, 1, {0,200,200}, 4);
        }
    }

    return people_count;
}

vector<vector<Point>> find_contours(js_params const &params, frame_pack &pack)
{
    vector<vector<Point>> contours;
    cv::resize(pack.input_frame, pack.input_frame_resize, params.resize_to);
    cvtColor(pack.input_frame_resize, pack.input_frame_gray, COLOR_BGR2GRAY);
    GaussianBlur(pack.input_frame_gray, pack.input_frame_gray, params.gaussian_kernal_size, 0);
    absdiff(pack.input_frame_gray, pack.first_frame_gray, pack.frame_delta);
    cv::threshold(pack.frame_delta, pack.thresh, params.thresh_val, 255, cv::THRESH_BINARY);
    cv::dilate(pack.thresh, pack.thresh,
               getStructuringElement(MORPH_RECT, params.dilate_kernal_size),
               Point(-1, -1), 2);
    findContours(pack.thresh, contours, {}, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    return contours;
}

cv::Size get_dilate_kernal_size(cv::FileStorage const &input)
{
    return cv::Size(static_cast<int>(input["dilate_kernal_width"].real()),
            static_cast<int>(input["dilate_kernal_height"].real()));
}

cv::Size get_gaussian_kernal_size(cv::FileStorage const &input)
{
    return cv::Size(static_cast<int>(input["gaussian_kernal_width"].real()),
            static_cast<int>(input["gaussian_kernal_height"].real()));
}


js_params get_params(cv::FileStorage const &input)
{
    js_params params;
    params.background_image = input["background_image"].string();
    cout<<"background image:"<<params.background_image<<endl;
    params.capture_background_at_x_frame = static_cast<int>(input["capture_background_at_x_frame"].real());
    params.dilate_kernal_size = get_dilate_kernal_size(input);
    cout<<"dilate kernal size:"<<params.dilate_kernal_size.area()<<endl;
    params.gaussian_kernal_size = get_gaussian_kernal_size(input);
    cout<<"gaussian kernal size:"<<params.gaussian_kernal_size.area()<<endl;
    params.min_countour_area = input["min_contour_area"].real();
    cout<<"min area:"<<params.min_countour_area<<endl;
    params.resize_width = static_cast<int>(input["resize_width_to"].real());
    cout<<"resize width:"<<params.resize_width<<endl;
    params.thresh_val = input["threshold"].real();
    cout<<"thresh val:"<<params.thresh_val<<endl;
    params.wait_between_frame_ms = static_cast<int>(input["wait_between_frame_ms"].real());

    return params;
}

void process_first_frame(js_params &params, frame_pack &pack)
{
    cv::resize(pack.first_frame, pack.first_frame_resize,params. resize_to);
    cvtColor(pack.first_frame_resize, pack.first_frame_gray, COLOR_BGR2GRAY);
}

void prepare_first_frame(int frame_count, js_params &params, frame_pack &pack)
{
    if(pack.first_frame.empty()){
        if(params.resize_to.area() == 0){            
            if(params.resize_width != 0){
                double const resize_scale = params.resize_width/static_cast<double>(pack.input_frame.cols);
                params.resize_to = Size(params.resize_width,
                                        static_cast<int>(pack.input_frame.rows * resize_scale));
            }else{
                params.resize_to = Size(pack.input_frame.cols, pack.input_frame.rows);
            }
        }
        if(!params.background_image.empty()){
            pack.first_frame = cv::imread(params.background_image);
            process_first_frame(params, pack);
        }else if(frame_count == params.capture_background_at_x_frame){
            pack.first_frame = pack.input_frame.clone();
            process_first_frame(params, pack);
        }
    }
}

void process_by_simple_subraction(cv::FileStorage const &input)
{
    auto capture = create_video_capture(input);
    if(capture.isOpened()){
        int frame_count = 0;
        js_params params = get_params(input);
        frame_pack pack;
        fps_estimator fps_est_;
        cv::VideoWriter vwriter;
        while(1){
            fps_est_.start();
            capture>>pack.input_frame;
            if(!pack.input_frame.empty()){
                prepare_first_frame(frame_count, params, pack);
                int people_count = 0;
                if(!pack.first_frame_gray.empty()){
                    auto const contours = find_contours(params, pack);
                    people_count = draw_contours(params, contours, pack);
                }else{
                    cv::resize(pack.input_frame, pack.input_frame_resize, params.resize_to);
                }
                if(!vwriter.isOpened()){
                    vwriter.open("yolov3.mp4",
                                 cv::VideoWriter::fourcc('H','2','6','4'),
                                 24.0, params.resize_to);
                }

                cv::putText(pack.input_frame_resize, "fps: " + std::to_string(fps_est_.get_fps()),
                            Point(0, 30), FONT_HERSHEY_SIMPLEX, 1, {255,0,255}, 2);
                cv::putText(pack.input_frame_resize, std::to_string(people_count) + " people",
                            Point(0, 65), FONT_HERSHEY_SIMPLEX, 1, {255,0,255}, 2);
                cv::imshow("current_frame", pack.input_frame_resize);
                if(!pack.first_frame.empty()){
                    cv::imshow("first_frame", pack.first_frame);
                }
                if(vwriter.isOpened()){
                    vwriter<<pack.input_frame_resize;
                }
                ++frame_count;
                auto const key = cv::waitKey(params.wait_between_frame_ms);
                if(key == 'q'){
                    break;
                }
                fps_est_.end();
            }else{
                break;
            }
        }
    }else{
        cout<<"cannot open video"<<endl;
    }
}

int main(int argc, char *argv[])try
{
    if(argc != 2){
        cerr<<"Must enter the location of json\n";
        return -1;
    }

    cv::FileStorage fs(argv[1], cv::FileStorage::READ);
    if(fs.isOpened()){
        process_by_simple_subraction(fs);
    }else{
        cerr<<"cannot open file:"<<argv[1]<<endl;
    }

    return 0;
}catch(std::exception const &ex){
    cerr<<ex.what()<<endl;
}
