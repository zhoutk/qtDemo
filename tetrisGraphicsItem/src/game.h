#ifndef GAME_H
#define GAME_H

#include <QObject>
#include "gamespace.h"
#include "tetris.h"

class Game : public QObject
{
    Q_OBJECT
public:
    Game();
    void start();
    void rotateBlock();
    void moveBlockLeft();
    void moveBlockRight();
    void moveBlockDown();
    void moveBlockDownEnd();

private:
	QPoint curPos;
	Tetris* block;
    Tetris* blockNext;
};

#endif // GAME_H
