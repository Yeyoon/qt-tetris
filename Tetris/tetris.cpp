#include "tetris.h"
#include "gamecontroller.h"
#include "wall.h"
#include <QPainter>
#include <QDebug>

const int BOXSIZE = 10;
const int TETRIS_W = 4;

tetris::tetris(const Tetris_type &t, GameController *game, int unit_w, QPointF location):
    location(location),
    myType(t),
    game(game),
    tetrisState(TETRIS_STATE_RUN),
    collidType(0),
    direction(TETRIS_DOWN),
    unit_w(unit_w)
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

    setPos(location);
}

tetris::~tetris()
{

}

QRectF tetris::boundingRect() const
{
    QPointF temp = mapFromScene(pos());
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

    QPointF temp = mapFromScene(pos());
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
    unsigned int old_pbits = locationBits;

    qDebug() << "updatePosition : " << direction;
    qDebug() << "locationBits : " << locationBits;
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
        break;
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
        locationBits = old_pbits;
    }else {
        setPos(location);
    }

    direction = TETRIS_DOWN;
    qDebug() << "after updatePosition : " << this;
    qDebug() << "locationBits : " << locationBits;
}

void tetris::advance(int step)
{
    if (!step) return;
    if (tickCnt++ % speed)
        return;

    if (tetrisState == TETRIS_STATE_PAUSE)
        return;

    if (collidType & TETRIS_COLLIDING_DOWN) {
        qDebug() << "advance before isLineComplete : " << this;
       // if (game->isLineComplete(this,unit_w))
       //     return;

        game->stopTetris(this);
        //tetrisState = TETRIS_STATE_PAUSE;
    }


    updatePosition();
    direction = TETRIS_DOWN;
}

void tetris::moveLeft()
{
    location.rx() -= unit_w;
}

void tetris::moveDown()
{
    location.ry() += unit_w;
}

void tetris::moveRight()
{
    location.rx() += unit_w;
}

void tetris::moveUp()
{
    location.ry() -= unit_w;
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

    if (game->handleColliding(this)){
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
        case TETRIS_CHANGE:
            collidType = TETRIS_COLLIDING_CHANGE;
            break;
        default:
            collidType = TETRIS_COLLIDING_DOWN;

        }
        ret = true;

    }

    return ret;
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
    if (game->handleColliding(this)) {
        location = old_location;
        setPos(location);
    }
}

/**
 * @brief tetris::change
 * this function will do 90'c change
 */
void tetris::change()
{
    unsigned int old_pbits = locationBits;
    unsigned int new_pbits = 0;

    /* As a[i,j] will changed to a[j,N-(i+1)].
     * Here N is the square Array SIZE 4
     */
    for (unsigned int i = 0; i < 4; i++)
        for (unsigned int j = 0; j < 4; j++) {
            unsigned int old_position = 4 * i + j;
            unsigned int new_position = j * 4 + 4 - (i + 1);

            if (old_pbits & (1 << old_position)) {
                new_pbits |= (1 << new_position);
            }
        }

    /**
     * for matrix like this
     * [ 0 1 1 1              [ 0 0 0 0
     *   0 0 0 1    ----->      0 0 0 1
     *   0 0 0 0                0 0 0 1
     *   0 0 0 0 ]              0 0 1 1 ]
     * will be translated to
     * [ 0 0 0 0
     *   0 1 0 0
     *   0 1 0 0
     *   1 1 0 0 ]
     * for the first line empty bits. It will be deleted in func updatePositionBits.
     * */
    for (int i = 0; i < 4 ; i++){
        int p = (new_pbits & 0x1) | (new_pbits & 0x10) | (new_pbits & 0x100) | (new_pbits & 0x1000);
        if (p)
            break;

        new_pbits = ((new_pbits & 0xf) >> 1) | ((new_pbits & 0xf0) >> 1) | ((new_pbits & 0xf00) >> 1) | ((new_pbits & 0xf000) >> 1);
    }

    updatePositionBits(new_pbits);
}

void tetris::setlocation(QPointF loc)
{
    location = loc;
    setPos(location);
}

