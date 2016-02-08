#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <opencv2/core.hpp>

#include <QMainWindow>
#include <QStringList>

#include <memory>

class cvmat_to_pixmap;

namespace Ui {
class MainWindow;
}

class setting_window;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpenFolder_triggered();

    void on_actionStart_triggered();

    void on_actionStop_triggered();

    void on_actionSetting_triggered();

    void on_actionClearAll_triggered();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<cvmat_to_pixmap> mat_to_pixmap_;
    QStringList file_names_;
    cv::Mat img_;
    QSize maximum_size_;
    setting_window *setting_win_;
};

#endif // MAINWINDOW_HPP
