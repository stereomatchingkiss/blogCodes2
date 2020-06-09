#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <QImage>

namespace Ui {
class MainWindow;
}

namespace frame_capture{

class capture_controller;

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void display_image(QImage input);

private slots:
    void on_pushButtonPlay_clicked();

private:
    void display_image_on_label(QImage input);

    Ui::MainWindow *ui;

    frame_capture::capture_controller *frame_capture_;
};

#endif // MAINWINDOW_HPP
