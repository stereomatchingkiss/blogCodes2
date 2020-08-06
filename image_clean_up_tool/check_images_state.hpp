#ifndef CHECK_IMAGES_STATE_HPP
#define CHECK_IMAGES_STATE_HPP

#include <QWidget>

namespace Ui {
class check_images_state;
}

class check_images_state : public QWidget
{
    Q_OBJECT

public:
    explicit check_images_state(QWidget *parent = nullptr);
    ~check_images_state();

private slots:
    void on_pushButtonOK_clicked();

private:
    Ui::check_images_state *ui;
};

#endif // CHECK_IMAGES_STATE_HPP
