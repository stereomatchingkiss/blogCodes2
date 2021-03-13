#include "font_manager.hpp"

#include <QDebug>
#include <QFile>
#include <QFontDatabase>

int get_font_id()
{
    static auto const font_id = QFontDatabase::addApplicationFont(":/assets/OPPOSans-H.ttf");
    qDebug()<<__func__<<"font id:"<<font_id;
    return font_id;
}

font_manager::font_manager() :
    QObject()
{
    QFile ifile(":/assets/OPPOSans-H.zip");
    if(ifile.open(QIODevice::ReadOnly)){
        auto const font_id = QFontDatabase::addApplicationFontFromData(qUncompress(ifile.readAll()));
        qDebug()<<__func__<<"font id:"<<font_id;

        font_family_ = QFontDatabase::applicationFontFamilies(font_id).at(0);
    }
}

QFont font_manager::get_font() const
{
    return QFont(get_font_family());
}

QString font_manager::get_font_family() const noexcept
{
    return font_family_;
}
