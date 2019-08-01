#include "tetris.h"
#include <QPainter>
#include <QDebug>

const int BOXSIZE = 10;
const int TETRIS_W = 4;
const int TETRIS_H_ = 4;

tetris::tetris(const Tetris_type &t):location(QPointF(0,0)),myType(t)
{
    Qt::GlobalColor colors[] = {Qt::red,Qt::yellow,Qt::blue,Qt::green,Qt::gray,Qt::cyan,Qt::magenta};
    myColor = colors[myType % TETRIS_TYPE_END];

    switch (myType) {
    case TETRIS_TYPE_1:
        locationBits = 0x2222;
        break;
    case TETRIS_TYPE_2:
        locationBits = 0x66;
        break;
    case TETRIS_TYPE_3:
        locationBits = 0x322;
        break;
    case TETRIS_TYPE_4:
        locationBits = 0xe4;
        break;
    case TETRIS_TYPE_5:
        locationBits = 0x262;
        break;
    case TETRIS_TYPE_6:
        locationBits = 0xc6;
        break;
    case TETRIS_TYPE_7:
        locationBits = 0xe2;
        break;
    default:
        locationBits = 0x2222;
    }

    speed = 5;

    setPos(0,0);
}

QRectF tetris::boundingRect() const
{
    QPointF temp = mapFromScene(location);
    qreal x = temp.x();
    qreal y = temp.y();

    QRectF bound = QRectF(x,y,4*BOXSIZE,4*BOXSIZE);

    qDebug() << "bound";
    qDebug() << bound;
    return bound;
}

void tetris::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    (void)option;
    (void)widget;
    painter->save();
    QPen pen(Qt::black);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(myColor);

    //painter->fillPath(shape(),myColor);
    painter->drawPath(shape());
    painter->restore();
}

QPainterPath tetris::shape() const
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);

    QPointF temp = mapFromScene(location);
    qreal x = temp.x();
    qreal y = temp.y();

    for (int i = 0; i < 0xf; i++){
        if (locationBits & (1 << i)) {
           int yl = i / TETRIS_W;
           int xl = i % TETRIS_W;

           qreal xt = x + xl * BOXSIZE;
           qreal yt = y + yl * BOXSIZE;

           path.addRect(QRectF(xt,yt,BOXSIZE,BOXSIZE));
        }
    }

    qDebug() << path;
    return path;
}

void tetris::advance(int step)
{
    if (!step)
        return;

    if (tickCnt++ % speed)
        return;

  // moveDown();

  // qDebug() << "Move down";
    setPos(location);
}

void tetris::moveLeft()
{
    location.rx() -= BOXSIZE;
}

void tetris::moveDown()
{
    location.ry() += BOXSIZE;

    qDebug() << "Key move down";
    qDebug() << location;
}

void tetris::moveRight()
{

    location.rx() += BOXSIZE;
}

