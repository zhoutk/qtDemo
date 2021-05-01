#include "customgraphtetrisblock.h"
#include <QList>

const int BLOCKSIDEWIDTH = 30;
const int BLOCKSIDELENGTH = 4;
const int BLOCKDATASIZE = BLOCKSIDELENGTH * BLOCKSIDELENGTH;
QVector<QVector<int>> SHAPES = {
	{1, 1, 1, 1},
	{1, 1, 1, 0, 1},
	{1, 1, 1, 0, 0, 0, 1},
	{0, 1, 1, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 1, 1},
	{0, 1, 1, 0, 1, 1},
	{0, 1, 0, 0, 1, 1, 1} 
};

CustomGraphTetrisBlock::CustomGraphTetrisBlock() :
	pos(QPoint(0,0)), 
	sideLen(BLOCKSIDELENGTH), 
	data(QVector<bool>(BLOCKDATASIZE)),
	blockType(0)
{
	this->relocate();
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos)
{
	new (this)CustomGraphTetrisBlock();
	this->pos = pos;
	this->relocate();
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(int blockType)
{
	new (this)CustomGraphTetrisBlock();
	this->blockType = blockType;
	QVector<int> shape = SHAPES[blockType % SHAPES.size()];
	for (int i = 0; i < shape.size(); i++) {
		if (shape[i])
			data[i] = true;
	}
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos, int blockType)
{
	new (this)CustomGraphTetrisBlock(blockType);
	this->pos = pos;
	this->relocate();
	this->blockType = blockType;
}

QRectF CustomGraphTetrisBlock::boundingRect() const
{
	return QRectF(
		0,
		0,
		BLOCKSIDELENGTH * BLOCKSIDEWIDTH,
		BLOCKSIDELENGTH * BLOCKSIDEWIDTH
	);
}

int CustomGraphTetrisBlock::type() const
{
	return Type;
}

Qjson CustomGraphTetrisBlock::getFactors()
{
	return Qjson();
}

void CustomGraphTetrisBlock::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*= nullptr*/)
{
	for (int i = 0; i < BLOCKDATASIZE; i++) {
		if (data[i])
		{
			painter->drawRoundedRect(
				(i % sideLen) * BLOCKSIDEWIDTH, 
				(int)(i / sideLen) * BLOCKSIDEWIDTH, 
				BLOCKSIDEWIDTH, 
				BLOCKSIDEWIDTH, 
				2, 2
			);
		}
	}
	prepareGeometryChange();
}

void CustomGraphTetrisBlock::relocate()
{
	this->setPos(pos);
}

void CustomGraphTetrisBlock::relocate(QPoint p)
{
	this->pos = p;
	this->relocate();
}
