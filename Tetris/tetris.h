#ifndef TETRIS_H
#define TETRIS_H

#include <QGraphicsItem>


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

class tetris : public QGraphicsItem
{
public:
    tetris(const Tetris_type &t);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    QPainterPath shape() const;
    void advance(int step);

    void moveLeft();
    void moveRight();
    void moveDown();
private:
    QPointF location;
    Tetris_type myType;
    Qt::GlobalColor myColor;
    unsigned int locationBits;
    unsigned int tickCnt;
    unsigned int speed;

};

#endif // TETRIS_H
