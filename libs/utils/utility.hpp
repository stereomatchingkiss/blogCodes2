#pragma once

#include <QString>
#include <QTime>

#include <deque>
#include <vector>

class QDateTime;
class QWidget;

namespace flt::ui{

int calculate_maximum_page(int data_size, int max_data_per_page) noexcept;
QString create_current_date_time_string();

QWidget* place_to_center_widget(QWidget *target);

bool  within_time_period(QDateTime const &current_time,
                         std::deque<bool> const &days_checked,
                         std::vector<QTime> const &times_from,
                         std::vector<QTime> const &times_to) noexcept;

}
