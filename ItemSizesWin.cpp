#include "ItemSizesWin.h"
#include <QGridLayout>
#include <QTableView>
#include <QPushButton>

ItemSizesWin::ItemSizesWin(QWidget *parent, int itemQuantity)
    : QWidget(parent), m_nItemQuantity(itemQuantity)
{
    setWindowTitle("Размеры изделий");
    setMinimumWidth(250);
    QGridLayout *pgrdLayout = new QGridLayout(this);

    QTableView *pTableView = new QTableView;
    pTableView->setItemDelegate(&m_delegate);

    m_model.setColumnCount(2);
    m_model.setHeaderData(0, Qt::Horizontal, "Ширина, см");
    m_model.setHeaderData(1, Qt::Horizontal, "Длина, см");
    m_model.setRowCount(m_nItemQuantity);

    pTableView->setModel(&m_model);

    pgrdLayout->addWidget(pTableView,0,0,1,2);

    QPushButton *pOk = new QPushButton("Ok");
    pOk->setMaximumWidth(100);
    connect(pOk, SIGNAL(clicked(bool)), this, SLOT(s_ok()));
    connect(pOk, SIGNAL(clicked(bool)), this, SLOT(close()));

    QPushButton *pCancel = new QPushButton("Отмена");
    pCancel->setMaximumWidth(100);
    connect(pCancel, SIGNAL(clicked(bool)), this, SLOT(close()));

    pgrdLayout->addWidget(pOk,1,0,1,1);
    pgrdLayout->addWidget(pCancel,1,1,1,1);

    setLayout(pgrdLayout);
}
void ItemSizesWin::s_ok()
{
    emit ok_clicked(m_model);
}
