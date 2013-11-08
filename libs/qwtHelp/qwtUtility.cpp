#include <qwt_legend.h>
#include <qwt_symbol.h>

#include "qwtUtility.hpp"

/**
 * @brief create_plot : create QwtPlot with some default setting
 * @param title : title of the plot
 * @return QwtPlot with some default setting
 */
std::unique_ptr<QwtPlot> create_plot(QString const &title)
{
    std::unique_ptr<QwtPlot> plot(new QwtPlot(title));

    plot->setCanvasBackground(Qt::white);
    plot->insertLegend( new QwtLegend );

    return plot;
}

/**
 * @brief create QwtPlotCurve with some default setting
 * @param title : title of the plot
 * @param style : the style of curve
 * @param default_symbol : true : add default symbol;false, don't add symbol
 * @return QwtPlotCurve with some default setting
 */
std::unique_ptr<QwtPlotCurve> create_plot_curve(QString const &title, QwtPlotCurve::CurveStyle style, bool default_symbol)
{
    std::unique_ptr<QwtPlotCurve> curve(new QwtPlotCurve(title));
    curve->setPen( Qt::blue, 4 );
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve->setStyle(style);

    if(default_symbol){
        std::unique_ptr<QwtSymbol> symbol(new QwtSymbol( QwtSymbol::Ellipse,
                                                         QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) ));
        curve->setSymbol( symbol.release() );
    }

    return curve;
}
