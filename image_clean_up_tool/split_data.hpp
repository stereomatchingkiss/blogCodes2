#ifndef SPLIT_DATA_HPP
#define SPLIT_DATA_HPP

#include <QWidget>

#include <vector>

namespace Ui {
class split_data;
}

class split_data : public QWidget
{
    Q_OBJECT

public:
    explicit split_data(QWidget *parent = nullptr);
    ~split_data();

private slots:
    void on_pushButtonStart_clicked();

private:
    std::vector<QString> get_sources();

    Ui::split_data *ui;
};

#endif // SPLIT_DATA_HPP
