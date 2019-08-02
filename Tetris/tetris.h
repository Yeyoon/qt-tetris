#ifndef TETRIS_H
#define TETRIS_H

#include <QGraphicsItem>

class GameController;

typedef enum
{
    TETRIS_TYPE_1,
    TETRIS_TYPE_2,
    TETRIS_TYPE_3,
    TETRIS_TYPE_4,
    TETRIS_TYPE_5,
    TETRIS_TYPE_6,
    TETRIS_TYPE_7,
    TETRIS_TYPE_END,
}Tetris_type;

typedef enum {
    TETRIS_LEFT,
    TETRIS_RIGHT,
    TETRIS_DOWN,
    TETRIS_UP,
    TETRIS_NONE,
}Tetris_Direction;

typedef enum {
    TETRIS_COLLIDING_LEFT = (1 << TETRIS_LEFT),
    TETRIS_COLLIDING_RIGHT = (1 << TETRIS_RIGHT),
    TETRIS_COLLIDING_DOWN = (1 << TETRIS_DOWN),
    TETRIS_COLLIDING_TOP = (1 << TETRIS_UP),
    TETRIS_COLLIDING_NONE = 0,
}Tetris_Collid;

typedef enum {
    TETRIS_STATE_RUN,
    TETRIS_STATE_PAUSE,
    TETRIS_STATE_RUN_ONECE,
    TETRIS_STATE_NONE,
}Tetris_State;

class tetris : public QGraphicsItem
{
public:
    tetris(const Tetris_type &t, GameController *game);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    QPainterPath shape() const;
    void advance(int step);

    void moveLeft();
    void moveRight();
    void moveDown();
    void moveUp();

    void setStop(Tetris_Direction s);
    void setDirection(Tetris_Direction d);

    Tetris_Direction willCollidingWithTetris(tetris *other);
    QList<QRectF> collectingRects();
    bool collidingWithQRectF(const QRectF &r);

    bool isStopped();
private:
    void updatePosition();

    QPointF location;
    Tetris_type myType;
    Qt::GlobalColor myColor;
    unsigned int locationBits;
    unsigned int tickCnt;
    unsigned int speed;
    bool isStop;
    GameController *game;

    QList<QRectF> shapV;

    Tetris_State  tetrisState;
    int collidType;
    Tetris_Direction direction;
};

#endif // TETRIS_H
