#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP

#include <QObject>
#include <QString>

class font_manager : public QObject
{
    Q_OBJECT
public:
    font_manager();

    QFont get_font() const;
    QString get_font_family() const noexcept;

private:
    QString font_family_;
};


#endif // FONT_MANAGER_HPP
