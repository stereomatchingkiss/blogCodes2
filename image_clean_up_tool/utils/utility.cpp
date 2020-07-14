#include "utility.hpp"

#include <QCheckBox>
#include <QHBoxLayout>

QWidget *place_to_center_widget(QWidget *target)
{
    QWidget *widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(target);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
    widget->setLayout(layout);

    return widget;
}
