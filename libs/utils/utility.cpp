#include "utility.hpp"

#include <QCheckBox>
#include <QDate>
#include <QHBoxLayout>
#include <QSettings>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace flt::ui{

namespace{

int get_offset_of_maximum_page(int data_size, int max_data_per_page)
{
    if(data_size < max_data_per_page){
        return 0;
    }else{
        double const remainder = std::fmod(data_size,
                                           static_cast<double>(max_data_per_page));
        return remainder > 0.0 ? 1 : 0;
    }
}

}

int calculate_maximum_page(int data_size, int max_data_per_page) noexcept
{
    return std::max(0, data_size / max_data_per_page - 1) +
            get_offset_of_maximum_page(data_size, max_data_per_page);
}

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

QString create_current_date_time_string()
{
    return QString("%2_%3").arg(QDate::currentDate().toString("yyyy.MM.dd"),
                                 QTime::currentTime().toString("hh.mm.ss"));
}

bool within_time_period(QDateTime const &current_time,
                        std::deque<bool> const &days_checked,
                        std::vector<QTime> const &times_from,
                        std::vector<QTime> const &times_to) noexcept
{
    size_t const index = static_cast<size_t>(current_time.date().dayOfWeek() - 1);
    if(days_checked[index]){
        QTime const time = current_time.time();
        if(times_from[index] <= times_to[index]){
            return time >= times_from[index] && time <= times_to[index];
        }else{
            return !(time >= times_to[index] && time <= times_from[index]);
        }
    }else{
        return false;
    }
}

}
