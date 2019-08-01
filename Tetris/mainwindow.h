#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QGraphicsScene;
class QGraphicsView;
class GameController;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void adjustViewSize();
    void newGame();
    void gameHelp();
    void about();

private:
    void createActions();
    void createMenus();

    void initScene();
    void initSceneBackground();

    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsView *view;


    GameController *game;

    QAction *newGameAction;
    QAction *pauseAction;
    QAction *resumeAction;
    QAction *exitAction;
    QAction *gameHelpAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
};

#endif // MAINWINDOW_H
