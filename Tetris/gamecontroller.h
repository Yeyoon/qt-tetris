#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include "tetris.h"

class QGraphicsScene;
class QKeyEvent;

class tetris;
class Wall;


class GameController : public QObject
{
    Q_OBJECT
public:
    GameController(QGraphicsScene &scene, QObject *parent = nullptr);
    ~GameController();

//    void snakeHitWall(Snake *snake, Wall *wall);
    void snakeAteItself();

    void stopTetris(tetris *te);
    void newTetris();

    bool handleColliding(tetris *te);
    bool isLineComplete(tetris *te, int unit_w);

    void destroyTetris(tetris *destroy);

public slots:
    void pause();
    void resume();
    void gameOver();

protected:
     bool eventFilter(QObject *object, QEvent *event);

private:
    void handleKeyPressed(QKeyEvent *event);
    void addNewFood();

    bool checkLineCompleted(qreal y,int unit_w,tetris *te, QMap<tetris*,QList<QRectF>> &m);
    void removeTetris(QList<tetris*> &removeList);

    QTimer timer;
    QGraphicsScene &scene;

    tetris *currentTetris;
    bool isPause;

    Wall *wTop;
    Wall *wLeft;
    Wall *wRight;
    Wall *wBottom;

    Wall *wShadow;

    QList<tetris*> tetrisList;
    QList<tetris*> removeList;
};

#endif // GAMECONTROLLER_H
