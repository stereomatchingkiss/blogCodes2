#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "simple_downloader.hpp"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QIntValidator>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    downloader_(new simple_downloader(this))
{
    ui->setupUi(this);
    qDebug()<<__func__;
    auto int_validator = new QIntValidator(this);
    ui->lineEditFrom->setValidator(int_validator);
    ui->lineEditTo->setValidator(int_validator);

    QSettings settings("tham soft", "link generator");
    if(!settings.contains("dir")){
        settings.setValue("dir", "/home");
    }
    if(settings.contains("mainwindow/geometry")){
        restoreGeometry(settings.value("mainwindow/geometry").toByteArray());
    }
    if(settings.contains("save_at")){
        ui->lineEditSaveAt->setText(settings.value("save_at").toString());
    }
}

MainWindow::~MainWindow()
{
    QSettings settings("tham soft", "link generator");
    settings.setValue("mainwindow/geometry", saveGeometry());
    settings.setValue("save_at", ui->lineEditSaveAt->text());

    delete ui;
}

void MainWindow::generate_link(int sharp_size, QString const &link_text)
{
    result_.clear();
    static QRegularExpression reg("#+");
    int from = ui->lineEditFrom->text().toInt();
    int to = ui->lineEditTo->text().toInt();
    if(from > to){
        std::swap(to, from);
    }
    for(int i = from; i <= to; ++i){
        auto const num =
                QString("%1").arg(i, std::max(sharp_size, ui->spinBoxFiledWidth->value()), 10, QChar('0'));
        auto link_text_cpy = link_text;
        qDebug()<<link_text_cpy;
        result_.push_back(link_text_cpy.replace(reg, num));
    }
    ui->plainTextEditResult->setPlainText(result_.join("\n"));
}

void MainWindow::on_pushButtonAddDownloadLink_clicked()
{    
    auto download_func = [this](){
        if(QDir().mkpath(ui->lineEditSaveAt->text())){
            auto const &links = ui->plainTextEditResult->toPlainText().split("\n");
            downloader_->add_files(links, ui->lineEditSaveAt->text());
            downloader_->show();
            downloader_->raise();
        }else{
            QMessageBox::warning(this, tr("Link generator"), tr("Cannot create folder, please try a new one"));
        }
    };

    QSettings settings("tham soft", "link generator");
    if(QDir(ui->lineEditSaveAt->text()).exists()){
        int const ret =
                QMessageBox::warning(this, tr("Link generator"),
                                     tr("The folder exist already, do you want to download files into the same folder?"),
                                     QMessageBox::Yes | QMessageBox::No);
        if(ret == QMessageBox::Yes){
            download_func();
        }
    }else{
        download_func();
    }
}

void MainWindow::on_pushButtonGenLinks_clicked()
{
    qDebug()<<__func__;
    if(ui->spinBoxNumPos->value() != -1){
        auto link_text = ui->lineEditLink->text();
        static QRegularExpression reg("(\\d+)");
        auto iter = reg.globalMatch(link_text);
        int index = 0;
        while(iter.hasNext()){
            QRegularExpressionMatch const match = iter.next();
            if(index == ui->spinBoxNumPos->value()){
                qDebug()<<__func__<<": match bbegin = "<<match.capturedStart();
                link_text.replace(match.capturedStart(), match.capturedEnd() - match.capturedStart(), QChar('#'));
                generate_link(1, link_text);
                break;
            }
            ++index;
        }
    }else{
        auto link_text = ui->lineEditLink->text();
        static QRegularExpression reg("(\\d+)(?!.*\\d)");
        auto match = reg.match(link_text);
        if(match.hasMatch()){
            link_text.replace(reg, QChar('#'));
        }
        generate_link(1, link_text);
    }
}

void MainWindow::on_pushButtonOpenSaveAt_clicked()
{
    QSettings settings("tham soft", "link generator");
    auto const dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                       ui->lineEditSaveAt->text(),
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
    ui->lineEditSaveAt->setText(dir);
}

