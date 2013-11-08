#include <fstream>
#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>

#include <QApplication>

#include <qwtHelp/qwtUtility.hpp>

#include "batchGradientDescent.hpp"
#include "readNumber.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    try{
        using Type = float;

        cv::Mat_<Type> const x_axis = read_number("../linearRegression/ex2x.dat");
        cv::Mat_<Type> const labels = read_number("../linearRegression/ex2y.dat");
        //attach "1" to the first cols of x_axis
        cv::Mat_<Type> features = cv::Mat_<Type>::ones(x_axis.rows, x_axis.cols + 1);        
        x_axis.col(0).copyTo(features.col(1));

        cv::Mat_<Type> const theta = batch_gradient_descent<Type>(features, labels, 0.07, 1500);
        std::cout<<theta<<std::endl;        

        auto plot = create_plot("Linear regression");
        auto curve_origin = create_plot_curve("training data", QwtPlotCurve::NoCurve);
        auto origin_points = create_points(x_axis.ptr<Type>(0), x_axis.ptr<Type>(0) + x_axis.rows, labels.ptr<Type>(0));

        cv::Mat_<Type> const new_features = features * theta;
        auto curve_new = create_plot_curve("batch gradient descent", QwtPlotCurve::Lines, false);
        auto points_new = create_points(x_axis.ptr<Type>(0), x_axis.ptr<Type>(0) + x_axis.rows, new_features.ptr<Type>(0));

        curve_origin->setSamples(origin_points);
        curve_origin->attach( plot.get() );

        curve_new->setSamples(points_new);
        curve_new->attach( plot.get() );

        plot->resize( 600, 400 );
        plot->show();

        return a.exec();

    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }


    return 0;
}
