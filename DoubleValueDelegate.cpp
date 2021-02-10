#include "DoubleValueDelegate.h"
#include <QSpinBox>
#include <QLineEdit>
#include <limits>

DoubleValueDelegate::DoubleValueDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

QWidget* DoubleValueDelegate::createEditor(QWidget *parent,
                                           const QStyleOptionViewItem &/* option */,
                                           const QModelIndex &/* index */) const
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setFrame(false);
    QDoubleValidator *pdblValidator = new QDoubleValidator(editor);
    pdblValidator->setBottom(std::numeric_limits<double>::epsilon());
    editor->setValidator(pdblValidator);

    return editor;
}
void DoubleValueDelegate::setEditorData(QWidget *editor,
                                        const QModelIndex &index) const
{
    QString value = (index.model()->data(index, Qt::EditRole)).toString();
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(value);
}
void DoubleValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                       const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->update();
    QString value = lineEdit->text();

    model->setData(index, value, Qt::EditRole);
}
void DoubleValueDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                               const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
