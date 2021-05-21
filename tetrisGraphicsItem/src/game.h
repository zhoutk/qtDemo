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
    void pause();
    void resume();
    void rotateBlock();
    void moveBlockLeft();
    void moveBlockRight();
    void moveBlockDown();
    void moveBlockDownEnd();
    void generateNextBlock();
    int getSpeed();
    int getLevels();
    int getScores();
    QString getGameID();
    void playback(QVector<Qjson> arr);

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
	int stepNum;
    QString gameID;

    int playbackIndex;
    QVector<Qjson> records;
};

#endif // GAME_H
