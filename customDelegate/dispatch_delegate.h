#ifndef DISPATCH_DELEGATE_H
#define DISPATCH_DELEGATE_H

#include <QStyledItemDelegate>

class dispatch_delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    dispatch_delegate(QObject *parent);

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &) const;

private:
    void paint_checkbox(QPainter *painter,
                        const QModelIndex &index,
                        const QString &dispatch_type,
                        const QRect &offset,
                        int role) const;
};

#endif // DISPATCH_DELEGATE_H

