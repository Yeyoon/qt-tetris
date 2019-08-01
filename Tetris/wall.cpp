#include "wall.h"

const int BOXSIZE = 10;

Wall::Wall(qreal x, qreal y, qreal width, qreal height, Qt::GlobalColor color) :
    x(x),
    y(y),
    w(width),
    h(height),
    c(color)
{
}


QRectF Wall::boundingRect() const
{
    QPointF p = mapFromScene(x,y);

    return QRectF(p.rx(),p.ry(),w,h);
}

QPainterPath Wall::shape() const
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);

    QPointF left = mapFromScene(x,y);
    path.addRect(x,y,w,h);
    return path;
}

void Wall::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    painter->save();
    painter->setPen(Qt::black);
    painter->setBrush(c);
    painter->drawPath(shape());
    painter->restore();
}

void Wall::advance(int step)
{
    (void)step;
    // do nothing
}

