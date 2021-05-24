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
	Tetris(int shape = 0);
	Tetris(QPoint pos);
	Tetris(QPoint pos, int shape);
	Tetris(QPoint pos, int shape, int blockType);
	~Tetris();

	void relocate();
	void relocate(QPoint);

	bool canContinue();
	bool rotate();
	bool moveLeft();
	bool moveRight();
	bool moveDown();
	void setBlockNotActive();
	void setBlockTest();
	void moveDownEnd();
	int cleanCount();
	int cleanRow();
	CustomGraphTetrisBlock* hasTetrisBlock(int x, int y);
	void erase(int x, int y);

	QPoint getPos();
	int getRotateNum();
	int getShape();

	QVector<QVector<int>> data;

private:
	int blockType;
	QPoint pos;
	int shape;
	int sideLen;
	QVector<CustomGraphTetrisBlock*> blocks;

	int rotateNum;
};

#endif // CUSTOMGRAPHTETRIS_H
