#include "PainterWidget.h"
#include <QPainter>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QStandardItemModel>
#include <QDebug>
#include <cmath>
#include <limits>

PainterWidget::PainterWidget(QWidget *parent) : QWidget(parent),
    m_pixmap(QPixmap(300,300)), eps(std::numeric_limits<double>::epsilon()),
    m_sheetWidth(m_pixmap.width()), m_sheetHeight(m_pixmap.height())
{
    setMinimumSize(m_pixmap.size());
}
void PainterWidget::resizeEvent(QResizeEvent *pEvent)
{
    m_pixmap = QPixmap(pEvent->size());
    m_pixmap.fill(Qt::white);
    draw();
}
void PainterWidget::paintEvent(QPaintEvent *pEvent)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_pixmap);
    pEvent->accept();
}
void PainterWidget::imageClear()
{
    m_pixmap.fill(Qt::white);
}
void PainterWidget::setSheetSizes(double sheetWidth, double sheetHeight)
{
    m_sheetWidth = sheetWidth;
    m_sheetHeight = sheetHeight;
}
void PainterWidget::setItemSizesModel(const QStandardItemModel& model)
{
    int rows = model.rowCount();
    int columns = model.columnCount();
    //qDebug() << "rows = " << rows << "; columns = " << columns;

    imageClear();
    m_mmap_Square_and_ItemSizes.clear();

    QLocale russian(QLocale::Russian);
    for (int row = 0; row < rows; ++row)
    {
        double square = 1.0;
        QPair<double,double> sizes;
        for (int column = 0; column < columns; ++column)
        {
            QModelIndex index = model.index(row, column, QModelIndex());
            QString str = (index.data()).toString();
            double size = russian.toDouble(str);
            if(column == 0)
                sizes.first = size;
            else
                sizes.second = size;
            square *= russian.toDouble(str);
        }
        m_mmap_Square_and_ItemSizes.insert(square, sizes);
    }
}

void PainterWidget::draw()
{
    QPainter painter(&m_pixmap);
    drawMetalSheet_and_Items(painter);
}
void PainterWidget::drawItem(QPainter &painter, double x, double y, double width, double height)
{
    QRectF itemRect(x, y, width, height);
    painter.drawRect(itemRect);
}
void PainterWidget::drawMetalSheet_and_Items(QPainter& painter)
{
    // если количество изделий = 0
    if(m_mmap_Square_and_ItemSizes.isEmpty()) return;

    QPen pen(QColor(0, 0, 0));
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    painter.setPen(pen);

    double tan_alpha = m_sheetWidth / m_sheetHeight;
    double scale = height() * 0.98;
    double sheet_h_scaled = scale;
    double sheet_w_scaled = sheet_h_scaled * tan_alpha;

    double x0 = 0.0;
    double y0 = 0.0;
    // рисование листа металла
    QRectF sheet(x0, y0, sheet_w_scaled, sheet_h_scaled);
    painter.drawRect(sheet);

    // АЛГОРИТМ РАСКРОЯ ЛИСТА МЕТАЛЛА

    // высота и ширина листа металла
    double hs = sheet_h_scaled;
    double ws = sheet_w_scaled;
    // координаты изделия (верхнего левого угла)
    double x = x0, y = y0;
    // смещение координат изделия
    double dx = 0.0, dy = 0.0;
    // оставшееся место на листе
    double hr = hs;
    double wr = ws;
    // высота максимальная из предыдущих высот изделий, размещенных на листе
    double h_max = 0.0;
    // минимальное оставшееся место по ширине листа
    double wr_min = ws, wr_min_temp;
    // смещение по X происходит, если лист заполнен по высоте и еще есть свободное место
    double shift_x = 0.0;
    // выполнимое количество изделий
    int nExecItemCount{};   

    auto it = m_mmap_Square_and_ItemSizes.end() - 1;
    auto it_before_begin = m_mmap_Square_and_ItemSizes.begin() - 1;
    while(it != it_before_begin)
    {
        QPair<double,double> value = it.value();
        // размеры изделий
        double w = std::min(value.first, value.second);
        double h = std::max(value.first, value.second);
        -- it;
        double tan_angle = w / h;
        // масштабированные размеры изделий
        h = h * scale / m_sheetHeight;
        w = h * tan_angle;

        // изделие умещается на листе металла
        bool bItemTooLarge = false;
        // второй проход алгоритма, если изделие не умещается по высоте, но умещается по ширине
        int N = 2;

        for(int i = 0; i < N; ++i)
        {
            if(h - hr <= eps && w - ws <= eps)
            {
                // размещение вдоль Y (вниз)
                x = x0 + shift_x;
                y += dy;
                drawItem(painter, x, y, w, h);
                hr -= h;
                wr = ws - w;
                dx = w; dy = h;
                h_max = h;

                wr_min_temp = std::min(wr, wr_min);
                wr_min = wr_min_temp;

                ++ nExecItemCount;
                break;
            }
            else if(h - hr > eps && h - (h_max + hr) <= eps && w - wr <= eps)
            {
                // размещение вдоль X (вправо)
                x += dx;
                drawItem(painter, x, y, w, h);
                dx = w;
                if(h - h_max > eps)
                {
                    hr = hr - (h - h_max);
                    h_max = h;
                    dy = h;
                }
                else
                {
                    dy = h_max;
                }
                wr -= w;

                wr_min_temp = std::min(wr, wr_min);
                wr_min = wr_min_temp;

                ++ nExecItemCount;
                break;
            }
            else if(h - hr > eps && w - hr > eps && (h - wr_min <= eps || w - wr_min <= eps)
                    && y + h - hs > eps)
            {
                // если лист заполнен по высоте, но изделия еще остались и есть место,
                // рисование продолжается сверху вниз со смещением по оси X
                double new_h = std::max(h, w);
                double new_w = std::min(h, w);
                if(new_h - hs <= eps)
                {
                    y = y0;
                    x = ws - wr_min;
                    drawItem(painter, x, y, new_w, new_h);
                    hr = hs - new_h;
                    wr = wr_min - w;
                    dy = new_h;
                    dx = new_w;
                    shift_x += x;

                    wr_min_temp = std::min(wr, wr_min);
                    wr_min = wr_min_temp;

                    ++ nExecItemCount;
                    break;
                }
            }
            else if(h - hr > eps && (w - ws <= eps ||
                                     (h - (h_max + hr) > eps && w - wr <= eps)) )
            {
                // если изделие не умещается по высоте, но умещается по ширине
                std::swap(h, w);
                continue;
            }
            else
            {
                bItemTooLarge = true;
                break;
            }

        }

        // изделие, не умещаемое на листе, не рисуется
        if(bItemTooLarge) continue;

    } // while()


    emit executedItemQuantity(nExecItemCount);
}


