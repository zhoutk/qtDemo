#include "customgraphtetrisblock.h"

const int BLOCKSIDELENGTH = 4;
const int BLOCKDATASIZE = BLOCKSIDELENGTH * BLOCKSIDELENGTH;

CustomGraphTetrisBlock::CustomGraphTetrisBlock() :
	pos(QPoint(0,0)), 
	sideLen(BLOCKSIDELENGTH), 
	data(QVector<bool>(BLOCKDATASIZE)),
	blockType(0)
{
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos)
{
	new (this)CustomGraphTetrisBlock();
	this->pos = pos;
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(int blockType)
{
	new (this)CustomGraphTetrisBlock();
	this->blockType = blockType;
	switch (blockType)
	{
	case 0: {
		for (int i = 0; i < BLOCKDATASIZE; i++) {
			if (i >= 2 * BLOCKSIDELENGTH && i < 3 * BLOCKSIDELENGTH)
				data[i] = true;
			else
				data[i] = false;
		}
		break;
	}
	default:
		break;
	}
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos, int blockType)
{
	new (this)CustomGraphTetrisBlock(blockType);
	this->pos = pos;
	this->blockType = blockType;
}

QRectF CustomGraphTetrisBlock::boundingRect() const
{
	return QRectF(0,0,0,0);
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
	;
}

void CustomGraphTetrisBlock::relocate()
{
	this->setPos(pos);
}
