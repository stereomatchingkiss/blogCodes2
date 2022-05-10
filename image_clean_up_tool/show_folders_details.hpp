#ifndef SHOW_FOLDERS_DETAILS_HPP
#define SHOW_FOLDERS_DETAILS_HPP

#include <QWidget>

namespace Ui {
class show_folders_details;
}

class show_folders_details : public QWidget
{
    Q_OBJECT

public:
    explicit show_folders_details(QWidget *parent = nullptr);
    ~show_folders_details();

private slots:
    void on_pushButtonOk_clicked();

private:
    Ui::show_folders_details *ui;
};

#endif // SHOW_FOLDERS_DETAILS_HPP
