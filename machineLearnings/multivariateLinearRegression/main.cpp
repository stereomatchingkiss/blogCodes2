#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

#include <opencv2/core/core.hpp>

#include <QApplication>

#include <batchGradientDescent.hpp>
#include <readNumber.hpp>

#include <qwtHelp/qwtUtility.hpp>
#include <qwtHelp/simple2DCurve.hpp>

/*template<typename T>
void mat_info(T const &mat)
{
    std::cout<<mat.rows<<", "<<mat.cols<<std::endl;
}*/

/**
 * @brief normalization the input(features)
 * @param input input(features) need to normalize
 */
template<typename T>
void normalize(cv::Mat_<T> &input)
{
    cv::Scalar mean;
    cv::Scalar std_deviation;
    cv::meanStdDev(input, mean, std_deviation);
    input -= mean[0];
    if(std_deviation[0] != 0){
        input /= std_deviation[0];
    }
}

/**
 * @brief normal equation
 */
/*template<typename T>
cv::Mat_<T> normal_equation(cv::Mat_<T> const &features, cv::Mat_<T> const &labels)
{
    cv::Mat_<T> const inverse = (features.t() * features).inv();
    return inverse * features.t() * labels;
}*/

/**
 * @brief draw and show the charts of cost function vs iteration
 * @param plot a 2-d plotting widget
 * @param features input(features)
 * @param labels output(target)
 * @param a
 */
template<typename T>
void cost_vs_iteration(simple2DPlot &plot, cv::Mat_<T> const &features, cv::Mat_<T> const &labels, QApplication &a)
{
    size_t const iterate_times = 50;
    T const ratio[] = {0.01, 0.03, 0.1, 0.3, 1, 1.2};
    QColor const colors[] = { {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}, {255, 128, 128}, {128, 0, 128}};
    std::vector<T> iterates(50);
    std::iota(std::begin(iterates), std::end(iterates), 1);
    for(size_t i = 0; i != sizeof(ratio) / sizeof(T); ++i){
        cv::Mat_<T> const costs = batch_gradient_descent_cost<T>(features, labels, ratio[i], iterate_times);
        plot.insert_curve(std::begin(iterates), std::end(iterates), std::begin(costs));
        plot.get_curve(i).setTitle(QString::number(ratio[i]));
        plot.get_curve(i).setPen(colors[i]);
        plot.get_curve(i).setStyle(QwtPlotCurve::Lines);
    }

    plot.replot();
    plot.render_document("costs_vs_iteration.png", {300, 200});

    plot.resize(600, 400);
    plot.show();
    a.exec();

    plot.clear();
    cv::Mat_<T> const costs = batch_gradient_descent_cost<T>(features, labels, 1.3, iterate_times);
    plot.insert_curve(std::begin(iterates), std::end(iterates), std::begin(costs));
    plot.get_curve(0).setTitle("1.3");
    plot.get_curve(0).setStyle(QwtPlotCurve::Lines);
    plot.render_document("diverge.png", {300, 200});

    plot.replot();
    plot.show();

    a.exec();
}

int main(int argc, char *argv[])
{    
    try{
        QApplication a(argc, argv);

        using Type = float;

        cv::Mat_<Type> x_axis = read_number<Type>("../multivariateLinearRegression/ex3x.dat");
        x_axis = x_axis.reshape(1, x_axis.rows / 2);

        cv::Mat_<Type> features(x_axis.rows, x_axis.cols + 1);
        features.setTo(1);
        x_axis.copyTo(features(cv::Rect(cv::Point(1, 0), x_axis.size())));

        cv::Mat_<Type> const labels = read_number<Type>("../multivariateLinearRegression/ex3y.dat");

        cv::Mat_<Type> col_one = features.col(1);
        cv::Mat_<Type> col_two = features.col(2);
        normalize(col_one);
        normalize(col_two);

        simple2DPlot plot("Cost of Linear regression", "iterate times", "costs");

        cost_vs_iteration(plot, features, labels, a);

    }catch(std::exception const &ex){
        std::cerr<<ex.what()<<std::endl;
    }

    return 0;
}
