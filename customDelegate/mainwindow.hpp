#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QVariant>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void init_model();
private slots:
    void drop_action_impl(int row,
                          QModelIndex const &target,
                          QVector<QVector<QVariant>> value);

    void on_pushButton_clicked();

    void on_pushButtonNative_clicked();

private:
    QStandardItemModel model_;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
