#include "qtable_item_utils.hpp"

#include "utility.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QTableWidgetItem>
#include <QWidget>

namespace flt::ui
{

namespace{

QTableWidgetItem* image_to_table_widget_item(QImage const &img, QSize const &size)
{
    if(!img.isNull()){
        QTableWidgetItem *thumbnail = new QTableWidgetItem;
        thumbnail->setData(Qt::DecorationRole, QPixmap::fromImage(img.scaled(size)));
        return thumbnail;
    }else{
        return new QTableWidgetItem;
    }
}

}

QWidget *create_alarm_group(int group)
{
    auto *cbox = new QComboBox;
    cbox->addItem(QObject::tr("Blacklist"));
    cbox->addItem(QObject::tr("WhiteList"));
    cbox->setCurrentIndex(group);

    return place_to_center_widget(cbox);
}

QWidget *create_checkbox()
{
    return place_to_center_widget(new QCheckBox);
}

QWidget *create_copy_to()
{
    auto *box = new QComboBox;
    box->addItem(QObject::tr("Do not copy"));
    box->addItem(QObject::tr("Recorded face"));

    return place_to_center_widget(box);
}

QCheckBox *create_credit_on(bool on)
{
    QCheckBox *cbox = new QCheckBox;
    cbox->setChecked(on);

    return cbox;
}

QSpinBox *create_credit_value(int credit_value, bool on)
{
    QSpinBox *sbox = new QSpinBox;
    sbox->setRange(0, 9999);
    sbox->setValue(credit_value);
    sbox->setEnabled(on);

    return sbox;
}

QSpinBox *create_spinbox(int max_range)
{
    auto *box =  new QSpinBox;
    box->setRange(0, std::max(0, max_range - 1));

    return box;
}

QTableWidgetItem *create_thumbnail(const QString &url)
{
    QImage img(url);
    return image_to_table_widget_item(img, QSize(100, 100));
}

QTableWidgetItem *create_thumbnail_from_base64_string(const QString &str)
{
    QByteArray by = QByteArray::fromBase64(str.toLatin1());
    auto const img = QImage::fromData(by,"JPEG");
    return image_to_table_widget_item(img, QSize(100, 100));
}

QTableWidgetItem *create_thumbnail(const QImage &img)
{
    return image_to_table_widget_item(img, QSize(100, 100));
}

}
