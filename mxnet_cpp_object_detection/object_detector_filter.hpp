#ifndef OBJECT_DETECTOR_FILTER_HPP
#define OBJECT_DETECTOR_FILTER_HPP

#include <opencv2/core.hpp>

#include <mxnet-cpp/MxNetCpp.h>

#include <deque>
#include <vector>

class object_detector_filter
{
public:
    enum class item_type{
        person,
        bicycle,
        car,
        motorbike,
        airplane,
        bus,
        train,
        truck,
        boat,
        traffic_light,
        fire_hydrant,
        stop_sign,
        parking_meter,
        bench,
        bird,
        cat,
        dog,
        horse,
        sheep,
        cow,
        elephant,
        bear,
        zebra,
        giraffe,
        backpack,
        umbrella,
        handbag,
        tie,
        suitcase,
        frisbee,
        skis,
        snowboard,
        sports_ball,
        kite,
        baseball_bat,
        baseball_glove,
        skateboard,
        surfboard,
        tennis_racket,
        bottle,
        wine_glass,
        cup,
        fork,
        knife,
        spoon,
        bowl,
        banana,
        apple,
        sandwich,
        orange,
        broccoli,
        carrot,
        hot_dog,
        pizza,
        donut,
        cake,
        chair,
        sofa,
        pottedplant,
        bed,
        diningtable,
        toilet,
        tvmonitor,
        laptop,
        mouse,
        remote,
        keyboard,
        cell_phone,
        microwave,
        oven,
        toaster,
        sink,
        refrigerator,
        book,
        clock,
        vase,
        scissors,
        teddy_bear,
        hair_drier,
        toothbrush,
    };

    struct result_type
    {
        double confidence_ = 0.0;
        item_type item_;
        cv::Rect roi_;
    };

    explicit object_detector_filter(std::vector<item_type> const &items_to_detect,
                                    cv::Size const &obj_detector_process_size,
                                    cv::Size const &image_size,
                                    double min_confidence = 0.4);

    std::vector<result_type> filter(std::vector<mxnet::cpp::NDArray> const &input) const;

    void set_image_size(cv::Size const &input) noexcept;
    void set_min_confidence(double input) noexcept;
    void set_items_to_detect(std::vector<item_type> const &items_to_detect);
    void set_obj_detector_process_size(cv::Size const &obj_detector_process_size) noexcept;

private:
    cv::Rect normalize_points(float x1, float y1, float x2, float y2) const noexcept;

    double min_confidence_;
    cv::Size image_size_;
    std::deque<bool> items_to_detect_;
    cv::Size obj_detector_process_size_;
};

#endif // OBJECT_DETECTOR_FILTER_HPP
