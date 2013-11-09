#ifndef SIMPLE2DCURVE_HPP
#define SIMPLE2DCURVE_HPP

#include <memory>
#include <vector>

#include <QColor>
#include <QSize>
#include <QString>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <qwtHelp/qwtUtility.hpp>

class QWidget;

/**
 * @brief simplify for the chores of adding and removing curve from QwtPlot
 */
class simple2DPlot : public QwtPlot
{
public:
    simple2DPlot(QWidget *parent, QString const &plot_name, QString const &x_axis_name, QString const &y_axis_name);

    void clear();

    QwtPlotCurve& get_curve(size_t id);

    template<typename ForwardIter, typename ForwardIter2>
    void insert_curve(ForwardIter x_begin, ForwardIter x_end, ForwardIter2 y_begin, QString const &title,
                      QColor const &color = QColor(255, 0, 0), QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines);

    void remove_curve(size_t id);

private:
    std::vector<std::unique_ptr<QwtPlotCurve>> curves_;
};

/**
 * @brief insert curve into the 2d plot
 * @param x_begin begin position of x-axis
 * @param x_end past the position of x_axis
 * @param y_begin begin position of y-axis, the number of elements of y-axis must not less than x-axis
 * @param title of the curve
 * @param color of the curve
 * @param style of the curve
 */
template<typename ForwardIter, typename ForwardIter2>
void simple2DPlot::insert_curve(ForwardIter x_begin, ForwardIter x_end, ForwardIter2 y_begin, QString const &title,
                                QColor const &color, QwtPlotCurve::CurveStyle style)
{
    auto points = create_points(x_begin, x_end, y_begin);
    auto curve = create_plot_curve(title, style);
    curve->setPen(color);
    curve->setSamples(points);
    curve->attach(this);
    curves_.emplace_back(std::move(curve));
}

#endif // SIMPLE2DCURVE_HPP
