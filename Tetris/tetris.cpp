#include "tetris.h"
#include "gamecontroller.h"
#include "wall.h"
#include <QPainter>
#include <QDebug>

const int BOXSIZE = 10;
const int TETRIS_W = 4;
const int TETRIS_H_ = 4;

tetris::tetris(const Tetris_type &t, GameController *game):
    location(QPointF(0,-90)),
    myType(t),
    isStop(TETRIS_UP),
    game(game),
    tetrisState(TETRIS_STATE_RUN),
    collidType(0),
    direction(TETRIS_DOWN)
{
    Qt::GlobalColor colors[] = {Qt::red,Qt::yellow,Qt::blue,Qt::green,Qt::darkYellow,Qt::cyan,Qt::magenta};
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

    setPos(0,-100);

    for (int i = 0; i < 0xf; i++){
        if (locationBits & (1 << i)) {
           int yl = i / TETRIS_W;
           int xl = i % TETRIS_W;

           qreal xt =  xl * BOXSIZE;
           qreal yt =  yl * BOXSIZE;

           shapV.push_back(QRectF(xt,yt,BOXSIZE,BOXSIZE));
        }
    }
}

QRectF tetris::boundingRect() const
{
    QPointF temp = mapFromScene(location);
    qreal x = temp.x();
    qreal y = temp.y();
    return QRectF(x,y,4*BOXSIZE,4*BOXSIZE);
}

void tetris::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    (void)option;
    (void)widget;
    painter->save();
    QPen pen(Qt::black);
    //pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(myColor);

   // painter->fillPath(shape(),myColor);
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

    for (int i = 0; i < shapV.size(); i++){
        QRectF r = QRectF(shapV[i]);
        r.setX(r.x() + x);
        r.setY(r.y() + y);
        r.setHeight(shapV[i].height());
        r.setWidth(shapV[i].width());
        path.addRect(r);
    }

    return path;
}

void tetris::updatePosition()
{
    QPointF old_location = pos();
    qDebug() << "updatePosition : direction " << direction;
    qDebug() << "pos is : " << old_location;
    qDebug() << "loc before move " << location;
    switch (direction) {
    case TETRIS_LEFT:
        if (!(collidType & TETRIS_COLLIDING_LEFT)){
            moveLeft();
            collidType = (Tetris_Collid)(collidType & (~TETRIS_COLLIDING_RIGHT));
        }
        break;
    case TETRIS_RIGHT:
        if (!(collidType & TETRIS_COLLIDING_RIGHT)){
            moveRight();
            collidType = (Tetris_Collid)(collidType & (~TETRIS_COLLIDING_LEFT));
        }
        break;
    case TETRIS_DOWN:
        if (!(collidType & TETRIS_COLLIDING_DOWN))
            moveDown();
        break;
    case TETRIS_NONE:
        return;
    default:
        break;
    }
    setPos(location);

    collidType = game->handleColliding(this);
}

void tetris::advance(int step)
{
    if (!step) return;
    if (tickCnt++ % speed)
        return;

    if (tetrisState == TETRIS_STATE_PAUSE)
        return;

    updatePosition();
}

void tetris::setStop(Tetris_Direction s)
{
    isStop = s;
}

void tetris::moveLeft()
{
    direction = TETRIS_DOWN;
    if (isStop == TETRIS_LEFT) {
        return;
    }

    location.rx() -= BOXSIZE;
}

void tetris::moveDown()
{
    location.ry() += BOXSIZE;
}

void tetris::moveRight()
{
    direction = TETRIS_DOWN;
    location.rx() += BOXSIZE;
}

void tetris::moveUp()
{
    location.ry() -= BOXSIZE;
}

void tetris::setDirection(Tetris_Direction d)
{
    direction = d;
}

QList<QRectF> tetris::collectingRects()
{
    QList<QRectF> q;
    qreal x = location.x();
    qreal y = location.y();

    for (int i = 0; i < shapV.size(); i++) {
        QRectF r = QRectF(shapV[i]);
        r.setX(x+r.x());
        r.setY(y+r.y());
        r.setHeight(shapV[i].height());
        r.setWidth(shapV[i].width());
        qDebug() << "shapV[i] : " << shapV[i];
        qDebug() << "r : " << r;
        q.push_back(r);
    }

    return q;
}

Tetris_Direction tetris::willCollidingWithTetris(tetris *other)
{
    QList<QRectF> q = collectingRects();
    QList<QRectF> p = other->collectingRects();

    //qDebug() << "q : " << q;
   // qDebug() << "p : " << p;
    for (int i = 0; i < q.size(); i++) {
        QRectF xp = QRectF(q[i]);
        QRectF lp = QRectF(q[i]);
        QRectF rp = QRectF(q[i]);
        xp.setHeight(xp.height() + 1);
        lp.moveTo(xp.x()-1,xp.y());
        rp.setWidth(rp.width() + 1);

        for (int j = 0; j < p.size(); j++) {
            QRectF yp = p[j];
            yp.setHeight(yp.height()+1);

            if (xp.intersects(yp))
                return TETRIS_DOWN;

            yp = p[j];

            if (lp.intersects(yp))
                return TETRIS_LEFT;

            if (rp.intersects(yp))
                return TETRIS_RIGHT;
        }
    }

    return TETRIS_NONE;
}

bool tetris::isStopped()
{
    return tetrisState == TETRIS_STATE_PAUSE;
}

bool tetris::collidingWithQRectF(const QRectF &r)
{
    QList<QRectF> head = this->collectingRects();
    for (int i = 0; i < head.size(); i++){
        if (r.intersects(head[i]))
            return true;
    }

    return false;
}
