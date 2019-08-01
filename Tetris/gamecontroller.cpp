#include <QEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMessageBox>

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
    wRight = new Wall(80,-100,10,200);
    wBottom = new Wall(-100,90,200,10);

    scene.addItem(wTop);
    scene.addItem(wLeft);
    scene.addItem(wRight);
    scene.addItem(wBottom);
    Tetris_type ty = (Tetris_type)(TETRIS_TYPE_1 + (qrand() % TETRIS_TYPE_END));
    currentTetris = new tetris(ty);
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
                if (!currentTetris->collidesWithItem(wLeft))
                    currentTetris->moveLeft();
                break;
            case Qt::Key_Right:
                if (!currentTetris->collidesWithItem(wRight))
                    currentTetris->moveRight();
                break;
            case Qt::Key_Down:
                if (!currentTetris->collidesWithItem(wBottom))
                    currentTetris->moveDown();
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
    } else {
        return QObject::eventFilter(object, event);
    }
}
