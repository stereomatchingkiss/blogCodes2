#ifndef RANGE_DELEGATE_H
#define RANGE_DELEGATE_H

#include <QStyledItemDelegate>

#include <utility>

class range_delegate : public QStyledItemDelegate
{
public:
    explicit range_delegate(std::pair<int, int> const &range,
                            QObject *parent = 0);
    ~range_delegate();

    void set_range(std::pair<int, int> const &range);

    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;

private:
    std::pair<int, int> range_;
};

#endif // RANGE_DELEGATE_H
