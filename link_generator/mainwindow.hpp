#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStringList>

class simple_downloader;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonAddDownloadLink_clicked();

    void on_pushButtonGenLinks_clicked();

    void on_pushButtonOpenSaveAt_clicked();

private:
    Ui::MainWindow *ui;

    QStringList result_;
    simple_downloader *downloader_;

    void generate_link(int sharp_size, QString const &link_text);
};

#endif // MAINWINDOW_HPP
