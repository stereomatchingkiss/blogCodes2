#include <QSize>
#include <QWidget>

#include <qwt_legend.h>

#include "simple2DCurve.hpp"

/**
 * @brief contructor
 * @param parent parent of the 2d plot
 * @param plot_name name of the 2d plot
 * @param x_axis_name name of x axis
 * @param y_axis_name name of y axis
 */
simple2DPlot::simple2DPlot(QWidget *parent, QString const &plot_name, QString const &x_axis_name, QString const &y_axis_name) :
    QwtPlot(plot_name, parent)
{
    setCanvasBackground(Qt::white);
    insertLegend( new QwtLegend );
    setAxisTitle(QwtPlot::xBottom, x_axis_name);
    setAxisTitle(QwtPlot::yLeft, y_axis_name);
}

simple2DPlot::simple2DPlot(QString const &plot_name, QString const &x_axis_name, QString const &y_axis_name, QWidget *parent)
    :simple2DPlot(parent, plot_name, x_axis_name, y_axis_name)
{

}

/**
 * @brief delete all of the curves
 */
void simple2DPlot::clear()
{
    detachItems(QwtPlotItem::Rtti_PlotItem, false);
    curves_.clear();
}

/**
 * @brief take specific curve by id
 * @param id id of curve
 * @return associate curve
 */
QwtPlotCurve& simple2DPlot::get_curve(size_t id)
{
    return *curves_[id];
}

/**
 * @brief remove curve with specific id
 * @param id id of curve
 */
void simple2DPlot::remove_curve(size_t id)
{
    if(id < curves_.size()){
        detachItems(curves_[id]->rtti(), false);
        curves_.erase(std::begin(curves_) + id);
    }else{
        throw std::out_of_range("out of range");
    }
}
