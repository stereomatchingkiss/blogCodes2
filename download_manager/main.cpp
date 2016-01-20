#include "download_manager.hpp"
#include "download_manager_ui.hpp"
#include <QApplication>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    download_manager dm;

    dm.append(QUrl("http://i.nhentai.net/galleries/894523/1.jpg"),
              "manga", "1.jpg");

    download_manager_ui ui;
    ui.show();

    return a.exec();
}
