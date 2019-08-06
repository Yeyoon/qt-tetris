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
    GameController(QGraphicsScene &scene, QObject *parent = nullptr,QPointF p = QPointF(-100,-100), int unit_w = 10);
    ~GameController();

//    void snakeHitWall(Snake *snake, Wall *wall);
    void snakeAteItself();

    void stopTetris(tetris *te);
    void newTetris();

    bool handleColliding(tetris *te);
    bool isLineComplete(tetris *te, int unit_w);

public slots:
    void pause();
    void resume();
    void gameOver();

protected:
     bool eventFilter(QObject *object, QEvent *event);

 private slots:
         void removeTetris(QList<tetris*> &removeList);

private:
    void handleKeyPressed(QKeyEvent *event);
    void addNewFood();

    bool checkLineCompleted(qreal y,int unit_w,tetris *te, QMap<tetris*,QList<QRectF>> &m);
    QString generScoreText();
    tetris* getRandomTetris();

    QTimer timer;
    QGraphicsScene &scene;
    QPointF wallLeftPoint;
    QGraphicsTextItem *text;
    int unit_w;
    tetris *currentTetris;
    tetris *nextTetris;
    bool isPause;

    Wall *wall;

    int totalTetrisCnt;
    int totalLineCompleteNum;

    QList<tetris*> tetrisList;
    QList<tetris*> removeList;
};

#endif // GAMECONTROLLER_H
