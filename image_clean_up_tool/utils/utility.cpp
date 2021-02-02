#include "utility.hpp"

#include <QCheckBox>
#include <QHBoxLayout>

#include <opencv2/imgcodecs.hpp>

#include <fstream>

QWidget *place_to_center_widget(QWidget *target)
{
    QWidget *widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(target);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
    widget->setLayout(layout);

    return widget;
}

cv::Mat read_image_by_fstream(const std::wstring &fpath)
{
    std::ifstream input(fpath, std::ios::binary);
    if(input.is_open()){

        input.seekg(0, std::ios::end);
        size_t const fileSize = static_cast<size_t>(input.tellg());
        input.seekg(0, std::ios::beg);

        if(fileSize == 0){
            return cv::Mat();
        }

        std::vector<unsigned char> data(fileSize);
        input.read(reinterpret_cast<char*>(&data[0]), static_cast<std::streamsize>(sizeof(unsigned char) * fileSize));

        if(!input){
            return cv::Mat();
        }

        return cv::imdecode(cv::Mat(data), cv::IMREAD_COLOR);
    }

    return {};
}
