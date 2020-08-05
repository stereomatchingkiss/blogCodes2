#ifndef LABELS_CHECK_HPP
#define LABELS_CHECK_HPP

#include <QWidget>

#include <set>

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

    void on_pushButtonDeleteUselessLabels_clicked();

private:
    std::set<QString> get_im_path_set() const;

    Ui::labels_check *ui;
};

#endif // LABELS_CHECK_HPP
