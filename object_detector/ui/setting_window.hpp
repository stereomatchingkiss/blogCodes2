#ifndef SETTING_WINDOW_HPP
#define SETTING_WINDOW_HPP

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class setting_window;
}

class general_setting;
class object_classify_setting;

class setting_window : public QDialog
{
    Q_OBJECT

public:
    explicit setting_window(QWidget *parent = nullptr);
    ~setting_window();

private slots:
    void algo_view_clicked(QModelIndex index);

private:
    Ui::setting_window *ui;

    //general_setting *general_setting_;
    object_classify_setting *obj_classify_setting_;
};

#endif // SETTING_WINDOW_HPP
