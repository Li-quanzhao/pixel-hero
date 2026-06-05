#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include "GameScene.h"
#include "GameController.h"

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

private:
    QGraphicsView *m_view;
    GameScene *m_scene;
    GameController *m_controller;

    void initUI();
    void setupConnections();
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // GAMEWINDOW_H