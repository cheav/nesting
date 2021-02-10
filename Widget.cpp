#include "Widget.h"
#include "ui_Widget.h"
#include "ItemSizesWin.h"
#include "PainterWidget.h"
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QDebug>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget),
    m_tan_angle(1.0), m_scaleFactor(0.9)
{
    setWindowTitle("Раскрой листа металла");

    QHBoxLayout *phbxLayout = new QHBoxLayout(this);

    m_pScrollArea = new QScrollArea;
    m_pPainterWidget = new PainterWidget(m_pScrollArea);
    m_pScrollArea->setWidget(m_pPainterWidget);
    m_pScrollArea->setMinimumSize(300,300);

    QWidget *pUserInputWidget = new QWidget;
    pUserInputWidget->setFixedWidth(225);
    ui->setupUi(pUserInputWidget);

    double bottomValue = std::numeric_limits<double>::epsilon();

    QDoubleValidator *pdblVal_MetalSheet_Width = new QDoubleValidator;
    pdblVal_MetalSheet_Width->setBottom(bottomValue);
    ui->lineEdit_MetalSheet_Width->setValidator(pdblVal_MetalSheet_Width);

    QDoubleValidator *pdblVal_MetalSheet_Length = new QDoubleValidator;
    pdblVal_MetalSheet_Length->setBottom(bottomValue);
    ui->lineEdit_MetalSheet_Length->setValidator(pdblVal_MetalSheet_Length);

    QIntValidator *pintVal_ItemQuantity = new QIntValidator;
    pintVal_ItemQuantity->setBottom(1);
    ui->lineEdit_ItemQuantity->setValidator(pintVal_ItemQuantity);
    connect(m_pPainterWidget, SIGNAL(executedItemQuantity(int)),
            this, SLOT(sExecutedItemQuantity(int)));

    ui->btn_ItemSizes->setEnabled(false);
    connect(ui->lineEdit_ItemQuantity, SIGNAL(textChanged(QString)),
            SLOT(sSetSizesButtonEnabled(QString)));
    connect(ui->btn_ItemSizes, SIGNAL(clicked(bool)), SLOT(sSetItemSizes()));
    connect(ui->btn_ItemSizes, SIGNAL(pressed()), SLOT(sSetItemSizes()));

    phbxLayout->addWidget(m_pScrollArea);
    phbxLayout->addWidget(pUserInputWidget);
    setLayout(phbxLayout);
}
Widget::~Widget()
{
    delete ui;
}

void Widget::sSetSizesButtonEnabled(const QString& itemQuantity)
{
    int nItemQuantity = itemQuantity.toInt();
    if(nItemQuantity > 0)
        ui->btn_ItemSizes->setEnabled(true);
    else
        ui->btn_ItemSizes->setEnabled(false);
}
void Widget::sSetItemSizes()
{
    int nItemQuantity = (ui->lineEdit_ItemQuantity->text()).toInt();
    m_upItemSizesWin.reset(new ItemSizesWin(nullptr, nItemQuantity));

    ItemSizesWin *pItemSizesWin = m_upItemSizesWin.get();
    connect(this, SIGNAL(closed()), pItemSizesWin, SLOT(close()));
    connect(pItemSizesWin, SIGNAL(ok_clicked(const QStandardItemModel&)),
            this, SLOT(sImageDisplay(const QStandardItemModel&)));

    pItemSizesWin->show();
}
void Widget::closeEvent(QCloseEvent *pEvent)
{
    emit closed();
    pEvent->accept();
}
void Widget::resizeEvent(QResizeEvent *pEvent)
{
    int nPainterWidgetHeight = m_pScrollArea->height() * m_scaleFactor;
    int nPainterWidgetWidth = nPainterWidgetHeight * (m_tan_angle + 0);
    m_pPainterWidget->resize(nPainterWidgetWidth, nPainterWidgetHeight);
    pEvent->accept();
}

void Widget::sImageDisplay(const QStandardItemModel& itemSizesModel)
{
    QLocale russian(QLocale::Russian);
    double sheetHeight = russian.toDouble(ui->lineEdit_MetalSheet_Width->text());
    double sheetWidth = russian.toDouble(ui->lineEdit_MetalSheet_Length->text());
    sheetWidth *= 100; // перевод длины в см

    double eps = std::numeric_limits<double>::epsilon();
    if(sheetHeight < eps || sheetWidth < eps) return;

    if(sheetHeight - sheetWidth > std::numeric_limits<double>::epsilon())
        std::swap(sheetHeight, sheetWidth);

    m_pPainterWidget->setSheetSizes(sheetWidth, sheetHeight);
    m_pPainterWidget->setItemSizesModel(itemSizesModel);

    m_tan_angle = sheetWidth / sheetHeight;

    int nPainterWidgetHeight = m_pScrollArea->height() * m_scaleFactor;
    int nPainterWidgetWidth = nPainterWidgetHeight * (m_tan_angle + 0);
    m_pPainterWidget->resize(nPainterWidgetWidth, nPainterWidgetHeight);

    m_pPainterWidget->draw();
}
void Widget::sExecutedItemQuantity(int nItems)
{
    QString strExecItemQuantity = QString::number(nItems);
    ui->lineEdit_ExecItemQuantity->setText(strExecItemQuantity);
}
