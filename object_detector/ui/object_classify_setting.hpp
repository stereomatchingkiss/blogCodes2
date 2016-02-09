#ifndef OBJECT_CLASSIFY_SETTING_HPP
#define OBJECT_CLASSIFY_SETTING_HPP

#include <QWidget>

namespace Ui {
class object_classify_setting;
}

class object_classify_setting : public QWidget
{
    Q_OBJECT

public:
    explicit object_classify_setting(QWidget *parent = 0);
    ~object_classify_setting();

private:
    Ui::object_classify_setting *ui;
};

#endif // OBJECT_CLASSIFY_SETTING_HPP
