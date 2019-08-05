#include "wall.h"

const int BOXSIZE = 10;

Wall::Wall(qreal x, qreal y, qreal width, qreal height, qreal thick, Qt::GlobalColor color) :
    x(x),
    y(y),
    w(width),
    h(height),
    thick(thick),
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
    qreal xx = left.rx(), yy = left.ry();
    path.addRect(xx,yy,w,thick);
    path.addRect(xx+w-thick,yy,thick,h);
    path.addRect(xx,yy,thick,h);
    path.addRect(xx,yy+h-thick,w,thick);

    return path;
}

void Wall::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    QPen pen(Qt::black);
    pen.setWidth(0);
    painter->save();
    painter->setPen(pen);
    painter->setBrush(c);
    painter->drawPath(shape());
    painter->restore();
}

void Wall::advance(int step)
{
    (void)step;
    // do nothing
}

