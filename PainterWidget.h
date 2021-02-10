#ifndef PAINTERWIDGET_H
#define PAINTERWIDGET_H

#include <QWidget>
#include <QMultiMap>
class QStandardItemModel;

class PainterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PainterWidget(QWidget *parent = nullptr);
    void draw();
    void setSheetSizes(double sheetWidth, double sheetHeight);
    void setItemSizesModel(const QStandardItemModel& model);
protected:
    void resizeEvent(QResizeEvent *pEvent) override;
    void paintEvent(QPaintEvent *pEvent) override;
signals:
    void executedItemQuantity(int);
private:
    void imageClear();
    void drawItem(QPainter &painter, double x, double y, double width, double height);
    void drawMetalSheet_and_Items(QPainter &painter);

    QPixmap m_pixmap;
    double eps;
    double m_sheetWidth;
    double m_sheetHeight;
    QMultiMap<double, QPair<double,double>> m_mmap_Square_and_ItemSizes;
};

#endif // PAINTERWIDGET_H
