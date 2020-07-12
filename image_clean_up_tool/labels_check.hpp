#ifndef LABELS_CHECK_HPP
#define LABELS_CHECK_HPP

#include <QWidget>

namespace Ui {
class labels_check;
}

class labels_check : public QWidget
{
    Q_OBJECT

public:
    explicit labels_check(QWidget *parent = nullptr);
    ~labels_check();

private slots:
    void on_pushButtonCheck_clicked();

private:
    Ui::labels_check *ui;
};

#endif // LABELS_CHECK_HPP
