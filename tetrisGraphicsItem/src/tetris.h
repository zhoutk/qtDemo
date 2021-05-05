#ifndef CUSTOMGRAPHTETRISBLOCK_H
#define CUSTOMGRAPHTETRISBLOCK_H
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

	bool rotate();
	bool moveLeft();
	bool moveRight();
	bool moveDown();
	void moveDownEnd();
	int cleanRow();
	bool canSee(int x, int y);
	void erase(int x, int y);

private:
	QPoint pos;
	int blockType;
	QVector<QVector<int>> data;
	int sideLen;
	QVector<CustomGraphTetrisBlock*> blocks;
};

#endif // CUSTOMGRAPHTETRISBLOCK_H