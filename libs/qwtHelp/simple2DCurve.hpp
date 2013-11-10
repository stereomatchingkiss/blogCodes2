#ifndef SIMPLE2DCURVE_HPP
#define SIMPLE2DCURVE_HPP

#include <memory>
#include <vector>

#include <QColor>
#include <QSize>
#include <QString>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_renderer.h>

#include <qwtHelp/qwtUtility.hpp>

class QWidget;

/**
 * @brief simplify for the chores of adding and removing curve from QwtPlot
 */
class simple2DPlot : public QwtPlot
{
public:
    simple2DPlot(QWidget *parent, QString const &plot_name, QString const &x_axis_name, QString const &y_axis_name);
    simple2DPlot(QString const &plot_name, QString const &x_axis_name, QString const &y_axis_name, QWidget *parent = nullptr);
    simple2DPlot(simple2DPlot const&) = delete;
    simple2DPlot& operator=(simple2DPlot const&) = delete;

    void clear();

    QwtPlotCurve& get_curve(size_t id);

    template<typename ForwardIter, typename ForwardIter2>
    void insert_curve(ForwardIter x_begin, ForwardIter x_end, ForwardIter2 y_begin, QString const &title = "");    

    void remove_curve(size_t id);

    void render_document(QString const &file_name, QSizeF const &size_mm, int resolution = 85);

private:
    std::vector<std::unique_ptr<QwtPlotCurve>> curves_;

    QwtPlotRenderer renderer_;
};

/**
 * @brief insert curve into the 2d plot
 * @param x_begin begin position of x-axis
 * @param x_end past the position of x_axis
 * @param y_begin begin position of y-axis, the number of elements of y-axis must not less than x-axis
 * @param title of the curve
 */
template<typename ForwardIter, typename ForwardIter2>
void simple2DPlot::insert_curve(ForwardIter x_begin, ForwardIter x_end, ForwardIter2 y_begin, QString const &title)
{
    auto points = create_points(x_begin, x_end, y_begin);
    auto curve = create_plot_curve(title);
    curve->setSamples(points);
    curve->attach(this);
    curves_.emplace_back(std::move(curve));
}

#endif // SIMPLE2DCURVE_HPP
