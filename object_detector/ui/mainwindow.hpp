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

    void on_actionRegion_triggered();

private:
    void has_image(bool value);

    Ui::MainWindow *ui;

    bool label_has_focus_;
    QStringList file_names_;
    cv::Mat img_;
    QSize maximum_size_;
    std::unique_ptr<cvmat_to_pixmap> mat_to_pixmap_;
    setting_window *setting_win_;
};

#endif // MAINWINDOW_HPP
