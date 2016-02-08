#ifndef GENERAL_SETTING_HPP
#define GENERAL_SETTING_HPP

#include <QWidget>

namespace Ui {
class general_setting;
}

class general_setting : public QWidget
{
    Q_OBJECT

public:
    explicit general_setting(QWidget *parent = nullptr);
    ~general_setting();

private:
    Ui::general_setting *ui;
};

#endif // GENERAL_SETTING_HPP
