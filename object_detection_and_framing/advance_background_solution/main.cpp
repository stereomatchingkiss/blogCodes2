#include "../fps_estimator.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include <iostream>

using namespace cv;
using namespace std;

struct frame_pack
{
    Mat fore_ground_mask_;
    Mat frame_delta;
    Mat input_frame;
    Mat input_frame_gray;
    Mat input_frame_resize;
    Mat thresh;
};

struct js_params
{
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
    GaussianBlur(pack.fore_ground_mask_, pack.fore_ground_mask_, params.gaussian_kernal_size, 0);
    cv::threshold(pack.fore_ground_mask_, pack.thresh, params.thresh_val, 255, cv::THRESH_BINARY);
    cv::dilate(pack.thresh, pack.thresh,
               getStructuringElement(MORPH_RECT, params.dilate_kernal_size),
               Point(-1, -1), 2);
    findContours(pack.thresh, contours, {}, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    return contours;
}

void resize_input_frame(js_params &params, frame_pack &pack)
{
    if(pack.input_frame_resize.empty()){
        if(params.resize_width != 0){
            double const resize_scale = params.resize_width/static_cast<double>(pack.input_frame.cols);
            params.resize_to = Size(params.resize_width,
                                    static_cast<int>(pack.input_frame.rows * resize_scale));
        }else{
            params.resize_to = Size(pack.input_frame.cols, pack.input_frame.rows);
        }
    }
    cv::resize(pack.input_frame, pack.input_frame_resize, params. resize_to);
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

void process_by_simple_subraction(cv::FileStorage const &input)
{
    auto capture = create_video_capture(input);
    if(capture.isOpened()){
        js_params params = get_params(input);
        frame_pack pack;
        fps_estimator fps_est_;
        cv::VideoWriter vwriter;
        auto const detect_shadow = input["detect_shadow"].real() == 0.0 ? false : true;
        auto const var_threshold = input["var_threshold"].real();
        auto const history = static_cast<int>(input["history"].real());
        auto bg_subtractor = cv::createBackgroundSubtractorMOG2(history, var_threshold, detect_shadow);
        while(1){
            fps_est_.start();
            capture>>pack.input_frame;
            if(!pack.input_frame.empty()){
                resize_input_frame(params, pack);
                int people_count = 0;
                bg_subtractor->apply(pack.input_frame_resize, pack.fore_ground_mask_);
                auto const contours = find_contours(params, pack);
                people_count = draw_contours(params, contours, pack);
                if(!vwriter.isOpened()){
                    vwriter.open(input["save_output_as"].string(),
                            cv::VideoWriter::fourcc('H','2','6','4'),
                            24.0, params.resize_to);
                }

                cv::putText(pack.input_frame_resize, "fps: " + std::to_string(fps_est_.get_fps()),
                            Point(0, 30), FONT_HERSHEY_SIMPLEX, 1, {255,0,255}, 2);
                cv::putText(pack.input_frame_resize, std::to_string(people_count) + " people",
                            Point(0, 65), FONT_HERSHEY_SIMPLEX, 1, {255,0,255}, 2);
                cv::imshow("current_frame", pack.input_frame_resize);
                if(!pack.fore_ground_mask_.empty()){
                    cv::imshow("fore_ground", pack.fore_ground_mask_);
                    cv::imshow("thresh", pack.thresh);
                }
                if(vwriter.isOpened()){
                    vwriter<<pack.input_frame_resize;
                }
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
