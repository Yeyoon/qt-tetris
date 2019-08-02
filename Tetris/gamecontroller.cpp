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
                               this);
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
            case Qt::Key_Up:
                stopTetris(currentTetris);
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
    } else {
        return QObject::eventFilter(object, event);
    }
}

void GameController::stopTetris(tetris* te)
{
    qDebug() << "stop tetris";
    //te->setStop(TETRIS_DOWN);
    newTetris();
}

void GameController::newTetris()
{
    qDebug() << "newTetris";

    tetrisList.push_back(currentTetris);
    Tetris_type ty = (Tetris_type)(TETRIS_TYPE_1 + (qrand() % TETRIS_TYPE_END));
    currentTetris = new tetris(ty,
                               this);

    scene.addItem(currentTetris);
}

Tetris_Collid GameController::handleColliding(tetris *te)
{
    Tetris_Collid collid = TETRIS_COLLIDING_NONE;
    if (wTop->collidesWithItem(te)){
        return TETRIS_COLLIDING_TOP;
    }

    QRectF b = QRectF(wBottom->boundingRect());
    b.moveTo(b.x(),b.y()-1);
    qDebug() << "b is : " << b;
    if (te->collidingWithQRectF(b)) {
        collid = TETRIS_COLLIDING_DOWN;
    }else if (wLeft->collidesWithItem(te)){
        collid = TETRIS_COLLIDING_LEFT;
    }else if (wRight->collidesWithItem(te)){
        collid = TETRIS_COLLIDING_RIGHT;
    }

    for (int i = 0;collid != TETRIS_COLLIDING_DOWN &&  i < tetrisList.size(); i++) {
        Tetris_Direction d = te->willCollidingWithTetris(tetrisList[i]);
       // bool iscoll = te->collidesWithItem(tetrisList[i]);
        if (d == TETRIS_NONE)
            continue;

        if (d == TETRIS_DOWN) {
            qDebug() << tetrisList.size();
            qDebug() << "te : " << te->pos();
            qDebug() << "tetriList[i] : " << tetrisList[i]->pos();
            collid =  TETRIS_COLLIDING_DOWN;
            break;
        }else {
            collid = TETRIS_LEFT == d ? TETRIS_COLLIDING_LEFT : TETRIS_COLLIDING_RIGHT;
        }
    }

    // check all values are online
    qreal x = -90;
    qreal y = 80;
    qreal w = 180;
    qreal h = 10;
    qreal stepw = 10;
    qreal i;

    tetrisList.push_back(te);
    for ( i = x; i < x + w; i += stepw){
        QPointF p(x+stepw/2,y+h/2);
        for (int i = 0; i < tetrisList.size(); i++){
            if (!tetrisList[i]->contains(p))
                break;
        }

        if (i < tetrisList.size()) break;
    }

    tetrisList.pop_back();
    if (i >= x + w) {
        qDebug() << "line complete";

        for (int i = 0; i < tetrisList.size(); i++) {
            tetrisList[i]->moveDown();
        }
    }
    return collid;
}

bool GameController::handleColliding()
{
    if (wTop->collidesWithItem(currentTetris)) {
        gameOver();
        return true;
    }

    if (wBottom->collidesWithItem(currentTetris)) {
        currentTetris->setStop(TETRIS_DOWN);
        return true;
    }

    for (int i = 0; i < tetrisList.size(); i++){
        if (currentTetris->collidesWithItem(tetrisList[i])) {
            qDebug() << currentTetris->pos();
            qDebug() << tetrisList[i]->pos();
            qDebug() << currentTetris->collidesWithPath(tetrisList[i]->shape());
            qDebug() << currentTetris->mapFromScene(currentTetris->shape());
            qDebug() << tetrisList[i]->mapFromScene(tetrisList[i]->shape());
            return true;
        }
    }

    return false;
}


