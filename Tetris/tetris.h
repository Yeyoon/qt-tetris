#ifndef TETRIS_H
#define TETRIS_H

#include <QGraphicsItem>

class GameController;
class Wall;

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
}Tetris_Direction;

class tetris : public QGraphicsItem
{
public:
    tetris(const Tetris_type &t, GameController *game, Wall *top, Wall *l, Wall *r, Wall *b);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    QPainterPath shape() const;
    void advance(int step);

    void moveLeft();
    void moveRight();
    void moveDown();
    void moveUp();

    void setStop(bool s);
    void setDirection(Tetris_Direction d);

    bool collidingWithTetris(tetris *other);
    QList<QRectF> collectingRects();
private:
    QPointF location;
    Tetris_type myType;
    Qt::GlobalColor myColor;
    unsigned int locationBits;
    unsigned int tickCnt;
    unsigned int speed;
    bool isStop;
    GameController *game;

    Wall *wTop;
    Wall *wLeft;
    Wall *wRight;
    Wall *wBottom;

    Tetris_Direction direction;
};

#endif // TETRIS_H
