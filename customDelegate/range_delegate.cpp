#include "range_delegate.hpp"

#include <QSpinBox>

range_delegate::range_delegate(std::pair<int, int> const &range,
                               QObject *parent) :
    QStyledItemDelegate(parent),
    range_(range)
{

}

range_delegate::~range_delegate()
{

}

void range_delegate::set_range(const std::pair<int, int> &range)
{
    range_ = range;
}

QWidget *range_delegate::createEditor(QWidget *parent,
                                      const QStyleOptionViewItem&,
                                      const QModelIndex&) const
{
    QSpinBox *spinbox = new QSpinBox(parent);
    spinbox->setRange(range_.first, range_.second);

    return spinbox;
}

void range_delegate::setEditorData(QWidget *editor,
                                   const QModelIndex &index) const
{
    int const Value = index.model()->data(index, Qt::DisplayRole).toInt();
    static_cast<QSpinBox*>(editor)->setValue(Value);
}

void range_delegate::setModelData(QWidget *editor,
                                  QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
    model->setData(index, static_cast<QSpinBox*>(editor)->value());
}

