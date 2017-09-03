#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "naive_worker.hpp"
#include "naive_controller.hpp"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonPrintCurThread_clicked();

    void on_pushButtonCallNormalFunc_clicked();

    void on_pushButtonCallSignalAndSlot_clicked();

private:
    naive_controller *controller_;
    naive_worker *worker_;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
