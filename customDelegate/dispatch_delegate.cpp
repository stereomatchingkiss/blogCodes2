#include "dispatch_delegate.hpp"

#include "user_model.hpp"

#include <QApplication>
#include <QDebug>
#include <QMouseEvent>

dispatch_delegate::dispatch_delegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

bool dispatch_delegate::editorEvent(QEvent *event,
                                    QAbstractItemModel *model,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index)
{
    //qDebug()<<"editor event";
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *evt = static_cast<QMouseEvent*>(event);
        int const ClickX = evt->x();
        int const ClickY = evt->y();

        QRect const Rect = option.rect;

        int X1 = Rect.left();
        int X2 = Rect.left() + (Rect.width() / 2);
        int Y = Rect.top() + 5;
        int W = 20;
        int H = 20;

        if ((ClickX > X1 && ClickX < X1 + W) && (ClickY > Y && ClickY < Y + H)){
            bool const Value = index.model()->data(index, user_model::SMSId).toBool();

            model->setData(index,  QColor(Qt::yellow), Qt::BackgroundColorRole);

            return model->setData(index, !Value, user_model::SMSId);
        }else if((ClickX > X2 && ClickX < X2 + W) && (ClickY > Y && ClickY < Y + H)) {
            bool const Value = index.model()->data(index, user_model::EmailId).toBool();

            model->setData(index,  QColor(Qt::yellow), Qt::BackgroundColorRole);

            return model->setData(index, !Value, user_model::EmailId);
        }
    }

    return true;
}

void dispatch_delegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    //qDebug()<<"paint event";
    QRect const Rect = option.rect;

    int const X1 = Rect.left() + 5;
    int const Y = Rect.top() + 5;
    int const W = 80;
    int const H = 20;

    QColor data = index.data(Qt::BackgroundColorRole).value<QColor>();
    QStyleOptionViewItem custom_option = option;
    custom_option.palette.setColor(QPalette::Base, data);
    QStyledItemDelegate::paint(painter, custom_option, index);

    paint_checkbox(painter, index, "SMS",
                   QRect(X1, Y, W, H), user_model::SMSId);

    int const X2 = Rect.left() + (Rect.width() / 2) + 5;
    paint_checkbox(painter, index, "EMAIL",
                   QRect(X2, Y, W, H), user_model::EmailId);
}

void dispatch_delegate::updateEditorGeometry(QWidget *editor,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

void dispatch_delegate::paint_checkbox(QPainter *painter,
                                       const QModelIndex &index,
                                       const QString &dispatch_type,
                                       const QRect &offset,
                                       int role) const
{
    QStyleOptionButton button;
    button.rect = offset;
    button.text = dispatch_type;
    button.state = QStyle::State_Enabled;

    bool const Checked = index.model()->data(index, role).toBool();
    //qDebug()<<"role = "<<role<<", "<<Checked;
    //button.state |= QStyle::State_On;

    if (Checked) {
        button.state |= QStyle::State_On;
    } else {
        button.state |= QStyle::State_Off;
    }

    QApplication::style()->drawControl(QStyle::CE_CheckBox, &button, painter);
}
