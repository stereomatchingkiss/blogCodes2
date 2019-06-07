#include "text_detector_east.hpp"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

vector<vector<Point>> find_contours(cv::Mat const &input)
{
    vector<vector<Point>> contours;
    findContours(input, contours, {}, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    return contours;
}

void process_by_east(cv::Mat &img)
{
    cv::Size const process_size(320, 320);
    text_detector_east te("C:/Users/yyyy/programs/Qt/computer_vision_model/ocr/frozen_east_text_detection.pb",
                          0.5f, 0.4f, process_size);
    auto const detect_result = te.detect(img);
    Point2f const ratio(static_cast<float>(img.cols) / process_size.width,
                        static_cast<float>(img.rows) / process_size.height);
    cout<<"detect:"<<detect_result.boxes_.size()<<" results"<<endl;
    for (size_t i = 0; i < detect_result.indices_.size(); ++i){
        RotatedRect const &box = detect_result.boxes_[static_cast<size_t>(detect_result.indices_[i])];
        Point2f vertices[4];
        box.points(vertices);
        for(int j = 0; j < 4; ++j){
            vertices[j].x *= ratio.x;
            vertices[j].y *= ratio.y;
        }
        for (int j = 0; j < 4; ++j){
            cv::line(img, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0), 1);
        }
    }

    cv::imshow("detect", img);
    cv::waitKey();
}

void read_text_by_tesseract(cv::Mat const &input)
{
    // Create Tesseract object
    std::unique_ptr<tesseract::TessBaseAPI> ocr(new tesseract::TessBaseAPI());
    // Initialize tesseract to use English (eng) and the LSTM OCR engine.
    ocr->Init("C:/Users/yyyy/programs/Qt/computer_vision_model/ocr", "eng", tesseract::OEM_LSTM_ONLY);

    ocr->SetPageSegMode(tesseract::PSM_SINGLE_LINE);

    ocr->SetImage(input.data, input.cols, input.rows, 3, static_cast<int>(input.step));
    cout<<"out text:"<<ocr->GetUTF8Text()<<endl;

    ocr->End();
}

int main()
{
    cv::Mat img = cv::imread("img2.jpg");
    if(img.cols > 480){
        cv::resize(img, img, {480, static_cast<int>(img.rows * 480.0/img.cols)});
    }
    cv::Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, gray, {9,9}, 0);
    cv::threshold(gray, gray, 125, 255, THRESH_BINARY_INV);
    auto const contours = find_contours(gray);
    cv::Mat target;
    for(auto const &contour : contours){
        auto const brect = boundingRect(contour);
        if(brect.width > brect.height){
            target = img(brect).clone();
            cv::rectangle(img, brect, {255, 0, 0}, 2);
        }
    }
    Mat target_gray;
    cvtColor(target, target_gray, COLOR_BGR2GRAY);
    Mat edges;
    //GaussianBlur(target_gray, target_gray, {3,3}, 0);
    Canny(target_gray, edges, 60, 150);

    Mat morph;
    dilate(edges, morph, getStructuringElement(MORPH_RECT, {3,3}));
    //dilate(morph, morph, getStructuringElement(MORPH_RECT, {3,3}));
    //morphologyEx(edges, morph, MORPH_CLOSE, getStructuringElement(MORPH_CROSS, {3,3}));
    //cv::threshold(text_line, text_line, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
    //cv::adaptiveThreshold(text_line, text_line, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 10);
    cv::imshow("detect", target);
    //cv::imshow("edges", edges);
    cv::imshow("morph", morph);
    //read_text_by_tesseract(target);
    cv::waitKey();

    //process_by_east(target);
}
