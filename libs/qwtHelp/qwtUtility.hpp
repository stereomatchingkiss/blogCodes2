#ifndef CREATEQPOLYPOINTS_HPP
#define CREATEQPOLYPOINTS_HPP

#include <memory>

#include <QPolygon>
#include <QString>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

std::unique_ptr<QwtPlot> create_plot(QString const &title);

std::unique_ptr<QwtPlotCurve> create_plot_curve(QString const &title, QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines);

/**
 * @brief create points for Qwt, user should make sure the range of y_begin is same or bigger than [x_begin, x_end)
 * @return the points
 */
template<typename ForwardIter, typename ForwardIter2>
QPolygonF create_points(ForwardIter x_begin, ForwardIter x_end, ForwardIter2 y_begin)
{
    QPolygonF points;
    for(; x_begin != x_end; ++x_begin, ++y_begin){
        points<<QPointF(*x_begin, *y_begin);
    }

    return points;
}



#endif // CREATEQPOLYPOINTS_HPP
