#include <fstream>
#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>

#include <QApplication>

#include <qwt_plot_renderer.h>

#include <qwtHelp/simple2DCurve.hpp>

#include <batchGradientDescent.hpp>
#include <readNumber.hpp>

int main(int argc, char *argv[])
{    
    try{
        QApplication a(argc, argv);

        using Type = float;

        cv::Mat_<Type> const x_axis = read_number("../linearRegression/ex2x.dat");
        cv::Mat_<Type> const labels = read_number("../linearRegression/ex2y.dat");
        //attach "1" to the first cols of x_axis
        cv::Mat_<Type> features = cv::Mat_<Type>::ones(x_axis.rows, x_axis.cols + 1);        
        x_axis.col(0).copyTo(features.col(1));

        cv::Mat_<Type> const theta = batch_gradient_descent<Type>(features, labels, 0.07, 1500);
        std::cout<<theta<<std::endl;        

        simple2DPlot plot("Linear regression", "ages", "height");
        plot.insert_curve(std::begin(x_axis), std::end(x_axis), std::begin(labels));
        plot.get_curve(0).setPen(QColor(255 ,0, 0), 5);
        plot.get_curve(0).setTitle("training data");
        plot.get_curve(0).setStyle(QwtPlotCurve::Dots);
        plot.replot();

        QwtPlotRenderer render;
        render.renderDocument(&plot, "true.png", {600, 400});

        cv::Mat_<Type> const new_features = features * theta;
        plot.insert_curve(std::begin(x_axis), std::end(x_axis), std::begin(new_features));
        plot.get_curve(1).setPen(QColor(0 ,0, 255));
        plot.get_curve(1).setTitle("batch gradient descent");

        plot.replot();
        render.renderDocument(&plot, "true_vs_predict.png", {600, 400});

        plot.resize(600, 400);
        plot.show();

        return a.exec();

    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }

    return 0;
}
