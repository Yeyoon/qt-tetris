#include "tetris.h"
#include "gamecontroller.h"
#include "wall.h"
#include <QPainter>
#include <QDebug>

const int BOXSIZE = 10;
const int TETRIS_W = 4;

tetris::tetris(const Tetris_type &t, GameController *game, int unit_w):
    location(QPointF(0,-90)),
    myType(t),
    isStop(TETRIS_UP),
    game(game),
    tetrisState(TETRIS_STATE_RUN),
    collidType(0),
    direction(TETRIS_DOWN),
    unit_w(unit_w),
    shrink_size(0)
{
    Qt::GlobalColor colors[] = {Qt::red,Qt::yellow,Qt::blue,Qt::green,Qt::darkYellow,Qt::cyan,Qt::magenta};
    myColor = colors[myType % TETRIS_TYPE_END];

    switch (myType) {
    case TETRIS_TYPE_1:
        locationBits = 0x1111;
        break;
    case TETRIS_TYPE_2:
        locationBits = 0x33;
        break;
    case TETRIS_TYPE_3:
        locationBits = 0x322;
        break;
    case TETRIS_TYPE_4:
        locationBits = 0x72;
        break;
    case TETRIS_TYPE_5:
        locationBits = 0x131;
        break;
    case TETRIS_TYPE_6:
        locationBits = 0x63;
        break;
    case TETRIS_TYPE_7:
        locationBits = 0x71;
        break;
    default:
        locationBits = 0x1111;
    }

    speed = 10;

    setPos(0,-100);

    for (int i = 0; i < 0xf; i++){
        if (locationBits & (1 << i)) {
           int yl = i / TETRIS_W;
           int xl = i % TETRIS_W;

           qreal xt =  xl * unit_w;
           qreal yt =  yl * unit_w;

           shapV.push_back(QRectF(xt,yt,unit_w,unit_w));
        }
    }
}

tetris::~tetris()
{

}

QRectF tetris::boundingRect() const
{
    QPointF temp = mapFromScene(location);
    qreal x = temp.x();
    qreal y = temp.y();
    if (myType == TETRIS_TYPE_1)
        return QRectF(x,y,4*unit_w,4*unit_w);
    else {
        return QRectF(x,y,4*unit_w,4*unit_w);
   }
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

    int bsize = unit_w ;

    for (int i = 0; i < 0xf; i++) {
        if (locationBits & (1 << i)) {
            int yl = i / TETRIS_W;
            int xl = i % TETRIS_W;
            qreal xt = x + xl * bsize;
            qreal yt = y + yl * bsize;

            path.addRect(QRectF(xt,yt,bsize,bsize));
        }
    }

    //path.translate(x+shrink_size,y+shrink_size);
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

    direction = TETRIS_DOWN;
}

void tetris::updatePosition1()
{
    QPointF old_location = pos();

    qDebug() << "updatePosition1 : " << direction;
    switch (direction) {
    case TETRIS_LEFT:
        moveLeft();
        break;
    case TETRIS_RIGHT:
        moveRight();
        break;
    case TETRIS_DOWN:
        moveDown();
        break;
    case TETRIS_CHANGE:
        change();
        direction = TETRIS_DOWN;
        return;
    case TETRIS_NONE:
        return;
    default:
        moveDown();
    }

    // first set location for handlecolliding
    setPos(location);

    if (handleColliding()) {
        // real colliding
        setPos(old_location);
        location = old_location;

        qDebug() << "real colliding" << tetrisState;
        qDebug() << "collid type : " << collidType;
        qDebug() << "pos : " << pos();
        qDebug() << "location : " << location;
    }else {
        setPos(location);
    }

    qDebug() << "after updatePosition1 : " << this;
}

void tetris::advance(int step)
{
    if (!step) return;
    if (tickCnt++ % speed)
        return;

    if (tetrisState == TETRIS_STATE_DESTROY) {
        qDebug() << "delete : " << this;
        game->destroyTetris(this);
        return;
    }

    if (tetrisState == TETRIS_STATE_PAUSE)
        return;

    if (collidType & TETRIS_COLLIDING_DOWN) {
        qDebug() << "advance before isLineComplete : " << this;
        game->isLineComplete(this,unit_w);
        game->stopTetris(this);
        tetrisState = TETRIS_STATE_PAUSE;
    }


    updatePosition1();
    direction = TETRIS_DOWN;
}

void tetris::setStop(Tetris_Direction s)
{
    isStop = s;
}

void tetris::moveLeft()
{
    location.rx() -= BOXSIZE;
}

void tetris::moveDown()
{
    location.ry() += BOXSIZE;
}

void tetris::moveRight()
{
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

    for (int i = 0; i < 0xf; i++) {
        if (locationBits & (1 << i)) {
            int yl = i / TETRIS_W;
            int xl = i % TETRIS_W;
            qreal xt = x + xl * unit_w;
            qreal yt = y + yl * unit_w;

            q.push_back(QRectF(xt,yt,unit_w,unit_w));
        }
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

void tetris::setCollidType(Tetris_Collid type)
{
    collidType = type;
}

void tetris::setTetrisState(Tetris_State s)
{
    tetrisState = s;
}

bool tetris::handleColliding()
{
    bool ret = false;

    qDebug() << "this is : " << this;
    qDebug() << "handleColliding";
    if (game->handleColliding(this)){
        if (1/*game->handleColliding1(this)*/) {
            switch (direction) {
            case TETRIS_DOWN:
                collidType = TETRIS_COLLIDING_DOWN;
                break;
            case TETRIS_LEFT:
                collidType = TETRIS_COLLIDING_LEFT;
                break;
            case TETRIS_RIGHT:
                collidType = TETRIS_COLLIDING_RIGHT;
                break;
            default:
                collidType = TETRIS_COLLIDING_DOWN;

            }
            ret = true;
        }
    }

    qDebug() << "after handle colliding : " << this;
    return ret;
}

void tetris::shrink(int size)
{
   // shrink_size += size;
}

void tetris::clearRectF(QRectF &r)
{
    QPointF p = pos();
    for (int i = 0; i < 0xf; i++){
        if (locationBits & (1 << i)){
                int yl = i / TETRIS_W;
                int xl = i % TETRIS_W;
                qreal xt = p.rx() + xl * unit_w;
                qreal yt = p.ry() + yl * unit_w;

                QRectF rt = QRectF(xt,yt,unit_w,unit_w);
                QRectF inter = r.intersected(rt);
                if (inter.isValid()){
                    if ((locationBits == 113 && i == 1) || myType == TETRIS_TYPE_7) {
                        qDebug() << "rt is : " << rt;
                        qDebug() << "r is : " << r;
                        qDebug() << "inter is : " << inter;
                        qDebug() << "locationbits : " << locationBits;
                        qDebug() << "i is : " << i;
                    }
                    updatePositionBits(locationBits & (~(1 << i)));
                    return;
                }
            }
    }
}

void tetris::updatePositionBits(unsigned int lbits)
{
    qDebug() << "updatePositionBits: my type is :" << myType;
    qDebug() << "old locationBits : " << locationBits;
    qDebug() << "update to " << lbits;

    unsigned int new_x = 0;
    unsigned int na[4] = {0};

    for (int i = 0; i < 4; i++) {
        na[i] = (lbits >> (i * 4)) & 0xf;
    }

    int j = 0;
    for (int i = 0; i < 4; i++) {
        if (na[i]) {
            new_x |= na[i] << (j * 4);
            j++;
        }
    }

    qDebug() << "locationBits : " << locationBits;
    qDebug() << "libits : " << lbits;
    qDebug() << "new x : " << new_x;

    locationBits = new_x;
}

bool tetris::isEmpty()
{
    qDebug() << "isEmpty : " << (locationBits & 0xff);
    return !(locationBits & 0xff);
}

void tetris::manualMoveWithHandleCollid(Tetris_Direction d)
{
    QPointF old_location = QPointF(location);
    switch (d) {
    case TETRIS_LEFT:
        moveLeft();
        break;
    case TETRIS_RIGHT:
        moveRight();
        break;
    default:
        moveDown();
        break;
    }

    setPos(location);
    if (game->handleColliding(this) /*&& collidType != TETRIS_COLLIDING_BOTTOM*/) {
        qDebug() << "manualMoveWithHandleCollid : " << true;
        qDebug() << "old_location : " << old_location;
        qDebug() << "new_location : " << location;
        location = old_location;
        setPos(location);
    }
}

void tetris::change()
{
    unsigned int old_pbits = locationBits;
    unsigned int new_x = 0;
    unsigned int na[4] = {0};
    unsigned int newa[4] = {0};

    for (int i = 0; i < 4; i++) {
        na[i] = old_pbits & 0xf;
        old_pbits >>= 4;
    }
 qDebug() << "change old lbits: " << locationBits;
 qDebug() << "na : " << na[0] << " " << na[1] << " " << na[2] << " " << na[3];
 newa[0] = !!(na[3] & 0x1) | (!!(na[2] & 0x1) << 1) | (!!(na[1] & 0x1) << 2) | (!!(na[0] & 0x1) << 3);
    newa[1] = !!(na[3] & 0x2) | (!!(na[2] & 0x2) << 1) | (!!(na[1] & 0x2) << 2) | (!!(na[0] & 0x2) << 3);
    newa[2] = !!(na[3] & 0x4) | (!!(na[2] & 0x4) << 1) | (!!(na[1] & 0x4) << 2) | (!!(na[0] & 0x4) << 3);
    newa[3] = !!(na[3] & 0x8) | (!!(na[2] & 0x8) << 1) | (!!(na[1] & 0x8) << 2) | (!!(na[0] & 0x8) << 3);
    qDebug() << "newa : " << newa[0] << "\t" << newa[1] << "\t" << newa[2] << "\t" << newa[3];

    for (int i = 3; i >=0; --i) {
        if (na[i] == 0) {
            newa[0] >>= 1;
            newa[1] >>= 1;
            newa[2] >>= 1;
            newa[3] >>= 1;
        }
    }
    new_x = newa[0] | (newa[1] << 4) | (newa[2] << 8) | (newa[3] << 12);

    updatePositionBits(new_x);


    qDebug() << "change new lbits: " << new_x;
}
