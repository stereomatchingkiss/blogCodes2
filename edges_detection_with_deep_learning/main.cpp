#include <montage.hpp>

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/layer.details.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>

#include <chrono>
#include <iostream>
#include <memory>

/**
 * This tutorial intend to share on the pyimagesearch, most of the readers
 * at there are more familiar with python, so I would add more comments
 */

void switch_to_cuda(cv::dnn::Net &net)
{
    try {
        for(auto const &vpair : cv::dnn::getAvailableBackends()){
            std::cout<<vpair.first<<", "<<vpair.second<<std::endl;
            if(vpair.first == cv::dnn::DNN_BACKEND_CUDA && vpair.second == cv::dnn::DNN_TARGET_CUDA){
                std::cout<<"can switch to cuda"<<std::endl;
                net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
                net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
                break;
            }
        }
    }catch(std::exception const &ex){
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        throw std::runtime_error(ex.what());
    }
}

std::tuple<cv::Mat, long long> forward_utils(cv::dnn::Net &net, cv::Mat const &input, cv::Size const &blob_size)
{
    using namespace std::chrono;

    //measure duration
    auto const start = high_resolution_clock::now();
    cv::Mat blob = cv::dnn::blobFromImage(input, 1.0, blob_size,
                                          cv::Scalar(104.00698793, 116.66876762, 122.67891434), false, false);
    net.setInput(blob);
    cv::Mat out = net.forward();
    cv::resize(out.reshape(1, blob_size.height), out, input.size());
    //the data type of out is CV_32F(single channel, floating point) so we need to upscale the value and convert
    //it to CV_8U(single channel, uchar)
    out *= 255;
    out.convertTo(out, CV_8U);
    //convert gray to bgr because we need to create montage(1 row, 3 column of images in our case)
    auto const finish = high_resolution_clock::now();
    auto const elapsed = duration_cast<milliseconds>(finish - start).count();
    cv::cvtColor(out, out, cv::COLOR_GRAY2BGR);

    return {out, elapsed};
}

class hed_edges_detector
{
public:
    hed_edges_detector(std::string const &weights, std::string const &config) :
        net_(cv::dnn::readNet(config, weights))
    {
        switch_to_cuda(net_);
    }

    long long elapsed() const
    {
        return elapsed_;
    }

    cv::Mat forward(cv::Mat const &input)
    {
        auto result = forward_utils(net_, input, {500, 500});
        elapsed_ += std::get<1>(result);
        return std::get<0>(result);
    }

private:
    long long elapsed_ = 0;
    cv::dnn::Net net_;
};

class dexi_edges_detector
{
public:
    explicit dexi_edges_detector(std::string const &model) :
        net_(cv::dnn::readNet(model))
    {
        switch_to_cuda(net_);
    }

    long long elapsed() const
    {
        return elapsed_;
    }

    cv::Mat forward(cv::Mat const &input)
    {
        auto result = forward_utils(net_, input, {400, 400});
        elapsed_ += std::get<1>(result);
        return std::get<0>(result);
    }

private:
    long long elapsed_ = 0;
    cv::dnn::Net net_;
};

//you need this class to obtain correct results
class CropLayer : public cv::dnn::Layer
{
public:
    CropLayer(const cv::dnn::LayerParams &params) : Layer(params)
    {
    }

    static cv::Ptr<cv::dnn::Layer> create(cv::dnn::LayerParams& params)
    {
        return cv::Ptr<cv::dnn::Layer>(new CropLayer(params));
    }

    virtual bool getMemoryShapes(const std::vector<std::vector<int> > &inputs,
                                 const int requiredOutputs,
                                 std::vector<std::vector<int> > &outputs,
                                 std::vector<std::vector<int> > &internals) const CV_OVERRIDE
    {
        CV_UNUSED(requiredOutputs); CV_UNUSED(internals);
        std::vector<int> outShape(4);
        outShape[0] = inputs[0][0];  // batch size
        outShape[1] = inputs[0][1];  // number of channels
        outShape[2] = inputs[1][2];
        outShape[3] = inputs[1][3];
        outputs.assign(1, outShape);
        return false;
    }

    virtual void forward(cv::InputArrayOfArrays inputs_arr,
                         cv::OutputArrayOfArrays outputs_arr,
                         cv::OutputArrayOfArrays internals_arr) CV_OVERRIDE
    {
        CV_UNUSED(internals_arr);
        std::vector<cv::Mat> inputs, outputs;
        inputs_arr.getMatVector(inputs);
        outputs_arr.getMatVector(outputs);

        cv::Mat& inp = inputs[0];
        cv::Mat& out = outputs[0];

        int ystart = (inp.size[2] - out.size[2]) / 2;
        int xstart = (inp.size[3] - out.size[3]) / 2;
        int yend = ystart + out.size[2];
        int xend = xstart + out.size[3];

        const int batchSize = inp.size[0];
        const int numChannels = inp.size[1];
        const int height = out.size[2];
        const int width = out.size[3];

        int sz[] = { (int)batchSize, numChannels, height, width };
        out.create(4, sz, CV_32F);
        for(int i=0; i<batchSize; i++){
            for(int j=0; j<numChannels; j++){
                cv::Mat plane(inp.size[2], inp.size[3], CV_32F, inp.ptr<float>(i,j));
                cv::Mat crop = plane(cv::Range(ystart,yend), cv::Range(xstart,xend));
                cv::Mat targ(height, width, CV_32F, out.ptr<float>(i,j));
                crop.copyTo(targ);
            }
        }
    }
};

void test_image(std::string const &mpath)
{
    cv::Mat img = cv::imread("2007_000129.jpg");
    hed_edges_detector hed(mpath + "hed_pretrained_bsds.caffemodel", mpath + "deploy.prototxt");
    auto hed_out = hed.forward(img);

    dexi_edges_detector dexi(mpath + "24_model.onnx");
    auto dexi_out = dexi.forward(img);

    cv::Size const frame_size(img.cols, img.rows);
    int constexpr grid_x = 3;
    int constexpr grid_y = 1;
    ocv::montage mt(frame_size, grid_x, grid_y);
    mt.add_image(img);
    mt.add_image(hed_out);
    mt.add_image(dexi_out);

    cv::imshow("results", mt.get_montage());
    cv::imwrite("results2.jpg", mt.get_montage());
    cv::waitKey();
}

void test_video(std::string const &mpath)
{
    cv::VideoCapture cap("pedestrian.mp4");
    if(cap.isOpened()){
        hed_edges_detector hed(mpath + "hed_pretrained_bsds.caffemodel", mpath + "deploy.prototxt");
        dexi_edges_detector dexi(mpath + "24_model.onnx");

        //unique_ptr is a resource manager class(smart pointer) of c++,
        //we allocate memory by the reset(or make_unique) api,
        //after leaving the scope(scope is surrounded by {}), the memory will be released. In c++, the
        //best way of manage the resource is avoid explicit memory allocation, if you really need to do it,
        //guard your memory by smart pointer. I use unique_ptr at here because I cannot
        //initialize the objects before I know the frame size of the video.
        std::unique_ptr<ocv::montage> mt;
        std::unique_ptr<cv::VideoWriter> vwriter;

        cv::Mat frame;
        float frame_count = 0;
        while(1){
            cap>>frame;
            if(frame.empty()){
                break;
            }

            ++frame_count;
            cv::resize(frame, frame, {}, 0.5, 0.5);
            auto const hed_out = hed.forward(frame);
            auto const dexi_out = dexi.forward(frame);
            if(!mt){
                //initialize the class to create montage
                //First arguments tell the class the size of each frame
                cv::Size const frame_size(frame.cols, frame.rows);
                int constexpr grid_x = 3;
                int constexpr grid_y = 1;
                mt.reset(new ocv::montage(frame_size, grid_x, grid_y));
            }
            if(!vwriter){
                auto const fourcc = cv::VideoWriter::fourcc('F', 'M', 'P', '4');
                int constexpr fps = 30;
                //because the montage is 3 columns and 1 row, so the cols need to multiply by 3
                vwriter.reset(new cv::VideoWriter("out.avi", fourcc, fps, {frame.cols * 3, frame.rows}));
            }
            mt->add_image(frame);
            mt->add_image(hed_out);
            mt->add_image(dexi_out);

            auto const montage = mt->get_montage();
            cv::imshow("out", mt->get_montage());
            vwriter->write(montage);
            cv::waitKey(10);
            mt->clear();
        }
        std::cout<<"hed elapsed time = "<<hed.elapsed()<<", frame count = "<<frame_count
                <<", fps = "<<1000.0f/hed.elapsed()/frame_count<<std::endl;
        std::cout<<"dexi elapsed time = "<<dexi.elapsed()<<", frame count = "<<frame_count
                <<", fps = "<<1000.0f/dexi.elapsed()/frame_count<<std::endl;
    }else{
        std::cerr<<"cannot open video pedestrian.mp4"<<std::endl;
    }
}

int main(int argc, char* argv[])try
{
    if(argc < 2){
        std::cerr<<"must specify the path of the models"<<std::endl;
        exit(1);
    }

    CV_DNN_REGISTER_LAYER_CLASS(Crop, CropLayer);
    std::string const mpath(argv[1]);

    //test_image(mpath);
    test_video(mpath);

    return 0;
}catch(std::exception const &ex){
    std::cerr<<ex.what()<<std::endl;
}
