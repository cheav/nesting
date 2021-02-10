#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <memory>
class PainterWidget;
class QScrollArea;
class ItemSizesWin;
class QStandardItemModel;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void closeEvent(QCloseEvent *pEvent) override;
    void resizeEvent(QResizeEvent *pEvent) override;
private slots:
    void sSetSizesButtonEnabled(const QString &itemQuantity);
    void sSetItemSizes();
    void sImageDisplay(const QStandardItemModel &itemSizesModel);
    void sExecutedItemQuantity(int nItems);
signals:
    void closed();
private:
    Ui::Widget *ui;
    double m_tan_angle;
    double m_scaleFactor;
    QScrollArea *m_pScrollArea;
    PainterWidget *m_pPainterWidget;
    std::unique_ptr<ItemSizesWin> m_upItemSizesWin;

};

#endif // WIDGET_H
