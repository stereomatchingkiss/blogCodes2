#pragma once

#include <QLayout>
#include <QString>
#include <QWidget>

class QCheckBox;
class QImage;
class QSpinBox;
class QTableWidgetItem;

namespace flt::ui
{

 QWidget* create_alarm_group(int group);

 QWidget* create_checkbox();

 QWidget* create_copy_to();

 QCheckBox* create_credit_on(bool on);

 QSpinBox* create_credit_value(int credit_value, bool on);

 QSpinBox* create_spinbox(int max_range);

 QTableWidgetItem* create_thumbnail(QImage const &img);
 QTableWidgetItem* create_thumbnail(QString const &url);
 QTableWidgetItem* create_thumbnail_from_base64_string(QString const &str);

template<typename T>
T* access_cell_widget(QWidget *widget)
{
    if(auto *layout = widget->layout()->itemAt(0)->widget()){
        if(auto *cbox = qobject_cast<T*>(layout)){
            return cbox;
        }
    }

    return nullptr;
}

template<typename T>
int to_int(T header)
{
    return static_cast<int>(header);
}

}
