#ifndef CVMAT_TO_PIXMAP_H
#define CVMAT_TO_PIXMAP_H

#include <opencv2/core.hpp>

#include <QtCore>
#include <QPixmap>

class cvmat_to_pixmap
{
public:    
    explicit cvmat_to_pixmap(QSize const &size);

    QPixmap to_pixmap(cv::Mat input);
    QPixmap to_pixmap(QString const &name);

private:
    cv::Mat img_;
    QSize size_;
};

#endif // CVMAT_TO_PIXMAP_H
