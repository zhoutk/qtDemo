#ifndef CUSTOMGRAPHTETRIS_H
#define CUSTOMGRAPHTETRIS_H
#include <QPainter>
#include <QVector>
#include <QObject>
#include "customGraphItem/customgraphtetrisblock.h"

class Tetris : public QObject
{
	Q_OBJECT

public:
	Tetris(int blockType = 0);
	Tetris(QPoint pos);
	Tetris(QPoint pos, int blockType);

	void relocate();
	void relocate(QPoint);

	bool canContinue();
	bool rotate();
	bool moveLeft();
	bool moveRight();
	bool moveDown();
	void moveDownEnd();
	int cleanRow();
	CustomGraphTetrisBlock* hasTetrisBlock(int x, int y);
	void erase(int x, int y);

private:
	QPoint pos;
	int blockType;
	QVector<QVector<int>> data;
	int sideLen;
	QVector<CustomGraphTetrisBlock*> blocks;
};

#endif // CUSTOMGRAPHTETRIS_H
