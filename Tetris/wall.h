#ifndef WALL_H
#define WALL_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsItem>

class Wall : public QGraphicsItem
{
public:
    Wall(qreal x, qreal y, qreal width, qreal height, qreal thick = 10, Qt::GlobalColor color = Qt::lightGray);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    QPainterPath shape() const;
    void advance(int step);

private:
    qreal x;
    qreal y;
    qreal w;
    qreal h;
    qreal thick;

    Qt::GlobalColor c;
};

#endif // WALL_H
