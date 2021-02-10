#ifndef ITEMSIZESWIN_H
#define ITEMSIZESWIN_H

#include <QWidget>
#include <QStandardItemModel>
#include "DoubleValueDelegate.h"

class ItemSizesWin : public QWidget
{
    Q_OBJECT
public:
    ItemSizesWin(QWidget *parent = nullptr, int itemQuantity = 0);
signals:
    void ok_clicked(const QStandardItemModel&);
private slots:
    void s_ok();
private:
    int m_nItemQuantity;
    QStandardItemModel m_model;
    DoubleValueDelegate m_delegate;
};

#endif // ITEMSIZESWIN_H
