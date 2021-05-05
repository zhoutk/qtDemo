#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include "gamespace.h"
#include "tetris.h"

class Game : public QObject
{
    Q_OBJECT
public:
    Game();
    ~Game();
    void start();
    void rotateBlock();
    void moveBlockLeft();
    void moveBlockRight();
    void moveBlockDown();
    void moveBlockDownEnd();
    void generateNextBlock();
    int getSpeed();
    int getLevels();
    int getScores();

public slots:
    void slotAutoTick();

signals:
    void signalGameOver();
    void signalUpdateScore();

private:
	QPoint curPos;
	Tetris* block;
    Tetris* blockNext;
	QTimer* autoTick;
	int tickVal;
	int tickInterval;
    int speed;
    int levels;
    int scores;
    int upScore;
};

#endif // GAME_H
