#include <QEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

#include "gamecontroller.h"
#include "tetris.h"
#include "wall.h"

GameController::GameController(QGraphicsScene &scene, QObject *parent) :
    QObject(parent),
    scene(scene),
    isPause(false)
{
    timer.start( 1000/33 );

    wTop = new Wall(-100,-100,200,10);
    wLeft = new Wall(-100,-100,10,200);
    wRight = new Wall(90,-100,10,200);
    wBottom = new Wall(-100,90,200,10);

    wShadow = new Wall(-90,80,180,10,Qt::gray);

    scene.addItem(wTop);
    scene.addItem(wLeft);
    scene.addItem(wRight);
    scene.addItem(wBottom);
    scene.addItem(wShadow);

    Tetris_type ty = (Tetris_type)(TETRIS_TYPE_1 + (qrand() % TETRIS_TYPE_END));
    currentTetris = new tetris(ty,
                               this,
                               wTop,
                               wLeft,
                               wRight,
                               wShadow);
    scene.addItem(currentTetris);

    scene.installEventFilter(this);

    resume();
}

GameController::~GameController()
{
}


//void GameController::snakeHitWall(Snake *snake, Wall *wall)
//{
//}

void GameController::snakeAteItself()
{
    QTimer::singleShot(0, this, SLOT(gameOver()));
}

void GameController::handleKeyPressed(QKeyEvent *event)
{
    if (!isPause)
        switch (event->key()) {
            case Qt::Key_Left:
                currentTetris->setDirection(TETRIS_LEFT);
                break;
            case Qt::Key_Right:
                currentTetris->setDirection(TETRIS_RIGHT);
                break;
            case Qt::Key_Down:
                currentTetris->setDirection(TETRIS_DOWN);
                break;
            case Qt::Key_Space:
                pause();
                break;
        }
    else resume();
}

void GameController::addNewFood()
{
}

void GameController::gameOver()
{
    qDebug() << "Game over";
    pause();
}

void GameController::pause()
{
    disconnect(&timer, SIGNAL(timeout()),
               &scene, SLOT(advance()));
    isPause = true;
}

void GameController::resume()
{
    connect(&timer, SIGNAL(timeout()),
            &scene, SLOT(advance()));
    isPause = false;
}

bool GameController::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        handleKeyPressed((QKeyEvent *)event);
        return true;
    } else if (event->type() == QEvent::Timer){
        qDebug() << "timer event";
        currentTetris->moveDown();
        if (currentTetris->collidesWithItem(wBottom)) {
            currentTetris->moveUp();
            currentTetris->setStop(true);
            return true;
        }
        return QObject::eventFilter(object,event);
    } else {
        return QObject::eventFilter(object, event);
    }
}

void GameController::stopTetris(tetris* te)
{
    qDebug() << "stop tetris";

    newTetris();
}

void GameController::newTetris()
{
    tetrisList.push_back(currentTetris);
    Tetris_type ty = (Tetris_type)(TETRIS_TYPE_1 + (qrand() % TETRIS_TYPE_END));
    currentTetris = new tetris(ty,
                               this,
                               wTop,
                               wLeft,
                               wRight,
                               wShadow);

    scene.addItem(currentTetris);
}

bool GameController::handleColliding(tetris *te)
{
    qDebug() << "handle colliding";
    if (wTop->collidesWithItem(te)){
        gameOver();
        return true;
    }

    if (wBottom->collidesWithItem(te)) {
        return true;
    }

    for (int i = 0; i < tetrisList.size(); i++) {
        if (te->collidesWithItem(tetrisList[i])) {
            return true;
        }
    }

    return false;
}


