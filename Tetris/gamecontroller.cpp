#include <QEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

#include "gamecontroller.h"
#include "tetris.h"
#include "wall.h"


GameController::GameController(QGraphicsScene &scene, QObject *parent,QPointF p, int unit_w) :
    QObject(parent),
    scene(scene),
    wallLeftPoint(p),
    unit_w(unit_w),
    currentTetris(nullptr),
    isPause(false),
    totalTetrisCnt(0),
    totalLineCompleteNum(0)
{
    timer.start( 1000/33 );

    text = scene.addText(tr("SCORE BOARD:\nTotal Tetris:\t0\nScored:\t0\n"));
    text->setPos(40,40);
    text->setScale(0.6);


    QPen pen(Qt::LinearGradientPattern);
    pen.setColor(Qt::blue);
    pen.setWidth(1);
    pen.setStyle(Qt::DashDotDotLine);
    scene.addLine(20,0,220,0,pen);

    wall = new Wall(p.x(),p.y(),120,220,unit_w);
    scene.addItem(wall);

    nextTetris = getRandomTetris();
    stopTetris(nullptr);
    scene.installEventFilter(this);

    resume();
}

GameController::~GameController()
{
}

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
                currentTetris->setDirection(TETRIS_CHANGE);
                break;
            case Qt::Key_Space:
                pause();
                break;
        }
    else resume();
}

void GameController::removeTetris(QList<tetris*> &removeList)
{
    for (int i = removeList.size(); i >= 0; i--){
        scene.removeItem(removeList[i]);
        delete removeList[i];
        removeList.removeAt(i);
    }
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
    qDebug() << "pause";
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
    (void)te;
    qDebug() << "stop tetris";
    //te->setStop(TETRIS_DOWN);
    pause();
    if (te) {
        if (isLineComplete(te,unit_w)) {
            ++totalLineCompleteNum;
            resume();
            return;
        }else {
            te->setTetrisState(TETRIS_STATE_PAUSE);
        }
    }

    newTetris();

    text->setPlainText(generScoreText());
    qDebug() << "stop tetris after new";
    qDebug() << currentTetris;
    if (!currentTetris)
        return;

    if (handleColliding(currentTetris)){
        QTimer::singleShot(0,this,SLOT(gameOver()));
    }else {
        currentTetris->setDirection(TETRIS_DOWN);
        resume();
    }

    qDebug() << "end stop tetris";
}

tetris* GameController::getRandomTetris()
{
    Tetris_type ty = (Tetris_type)(TETRIS_TYPE_1 + (qrand() % TETRIS_TYPE_END));

    tetris* new_te = new tetris(ty,
                               this,
                               unit_w,
                               QPointF(60,-60));
    if (new_te == nullptr)
        return nullptr;

    new_te->setDirection(TETRIS_NONE);
    new_te->setTetrisState(TETRIS_STATE_PAUSE);
    scene.addItem(new_te);

    return new_te;
}

void GameController::newTetris()
{
    qDebug() << "newTetris";

    if (currentTetris != nullptr)
        tetrisList.push_back(currentTetris);

    currentTetris = nextTetris;
    nextTetris = getRandomTetris();

    if (currentTetris) {
        currentTetris->setlocation(QPointF(-50,-90));
        currentTetris->setDirection(TETRIS_DOWN);
        currentTetris->setTetrisState(TETRIS_STATE_RUN);
        scene.addItem(currentTetris);

        ++totalTetrisCnt;
    }
}

bool GameController::handleColliding(tetris *cu)
{
    if (wall->collidesWithItem(cu)) {
        QPainterPath p1,p2, pin;
        p1 = cu->mapToScene(cu->shape());
        p2 = wall->mapToScene(wall->shape());

        pin = p1.intersected(p2);
        if (pin.elementCount() > 0) {
            qDebug() << "hit the wall intersected : " << pin;
            return true;
        }
    }

    for (int i = 0; i < tetrisList.size(); i++){
        if (cu == tetrisList[i])
            continue;

        if (cu->collidesWithItem(tetrisList[i])) {
            QPainterPath p1, p2;
            p1 = cu->mapToScene(cu->shape());
            p2 = tetrisList[i]->mapToScene(tetrisList[i]->shape());
            QPainterPath pin = p1.intersected(p2);

            if (pin.elementCount() == 0)
                continue;
            return true;
        }
    }

    return false;
}

bool GameController::checkLineCompleted(qreal y, int unit_w,tetris *te, QMap<tetris*,QList<QRectF>> &m)
{
    qreal x = -90;
    qreal xend = x + unit_w * 10;
    int i;
    QRectF r;

    m.clear();

    if (y > 110) {
        // here is the bottom;
        return false;
    }
    for (; x < xend; x+= unit_w){
        r = QRectF(x,y,unit_w,unit_w);
        for (i = 0; i < tetrisList.size(); i++) {
            if (tetrisList[i]->collidingWithQRectF(r)){
                m[tetrisList[i]].push_back(r);
                break;
            }
        }

        if (i == tetrisList.size()) {
            if (te->collidingWithQRectF(r)){
                m[te].push_back(r);
                continue;
            }

            return false;
        }
    }

    return true;
}

bool GameController::isLineComplete(tetris* te, int unit_w)
{
    QRectF b = te->mapRectToScene(te->boundingRect());
    qreal y = b.y();
    qreal x = -90;
    QRectF r;
    QMap<tetris*,QList<QRectF> > m;
    qreal yx;

    for (yx = y; yx < y + b.height(); yx += unit_w) {
        if (checkLineCompleted(yx,unit_w,te,m))
            break;
    }

    if ((int)yx == (int)(y + b.height()))
        return false;

    qDebug() << "line completed for point : " << b;
    qDebug() << "line completed for line :" << yx;

    QMapIterator<tetris*,QList<QRectF>> qiter(m);
    while (qiter.hasNext()) {
        tetris* key = qiter.next().key();
        QList<QRectF> ra = qiter.value();
        for (int i = 0; i < ra.size(); i++) {
            key->clearRectF(ra[i]);
        }
    }

    // do clear
    pause();
    for (int i =  tetrisList.size()-1; i >=0;  --i) {
        if (tetrisList[i]->isEmpty()) {

            qDebug() << "remove item : " << tetrisList[i]->pos();
            qDebug() << "remove item is : " << tetrisList[i] << "\t" << " scene is : " << tetrisList[i]->scene();
            tetris *xx = tetrisList[i];
            xx->hide();
            tetrisList.removeAt(i);

            removeList.push_back(xx);
        }
    }
    resume();

    for (int i = 0; i < tetrisList.size(); ++i) {
        tetrisList[i]->manualMoveWithHandleCollid(TETRIS_DOWN);
    }

    QTimer::singleShot(0,this,SLOT(removeTetris));
    return true;
}

QString GameController::generScoreText()
{
    QString text_str = QString(tr("SCORE BOARD:\nTotal Tetris:\t")) +
            QString::number(totalTetrisCnt) +
            QString("\nScore:\t") +
            QString::number(totalLineCompleteNum) +
            QString("\n");

    return text_str;
}

