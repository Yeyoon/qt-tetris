#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>

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

public slots:
    void pause();
    void resume();
    void gameOver();

protected:
     bool eventFilter(QObject *object, QEvent *event);

private:
    void handleKeyPressed(QKeyEvent *event);
    void addNewFood();

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
};

#endif // GAMECONTROLLER_H
