#ifndef SETTING_WINDOW_HPP
#define SETTING_WINDOW_HPP

#include <QDialog>

namespace Ui {
class setting_window;
}

class general_setting;

class setting_window : public QDialog
{
    Q_OBJECT

public:
    explicit setting_window(QWidget *parent = nullptr);
    ~setting_window();

private:
    Ui::setting_window *ui;

    //general_setting *general_setting_;
};

#endif // SETTING_WINDOW_HPP
