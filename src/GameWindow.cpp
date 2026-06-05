#include "GameWindow.h"
#include <QEvent>
#include <QKeyEvent>

GameWindow::GameWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    setupConnections();
    m_view->installEventFilter(this);
}

GameWindow::~GameWindow()
{
    delete m_controller;
    delete m_scene;
    delete m_view;
}

void GameWindow::initUI()
{
    setWindowTitle("像素勇者 - Pixel Hero Adventure");
    setFixedSize(800, 600);
    setStyleSheet("background-color: #1a1a2e;");

    m_scene = new GameScene(this);
    m_view = new QGraphicsView(m_scene, this);
    m_view->setGeometry(0, 0, 800, 600);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform, false);
    m_view->setRenderHint(QPainter::Antialiasing, false);

    m_controller = new GameController(m_scene, this);

    setCentralWidget(m_view);
}

void GameWindow::setupConnections()
{
    connect(m_controller, &GameController::gameStarted, this, [this]() {
        m_scene->startGame();
    });

    connect(m_controller, &GameController::gamePaused, this, [this]() {
        m_scene->pauseGame();
    });

    connect(m_controller, &GameController::gameResumed, this, [this]() {
        m_scene->resumeGame();
    });

    connect(m_controller, &GameController::gameOver, this, [this]() {
        m_scene->gameOver();
    });
}

bool GameWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_view) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            m_controller->handleKeyPress(keyEvent);
            return true;
        } else if (event->type() == QEvent::KeyRelease) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            m_controller->handleKeyRelease(keyEvent);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}