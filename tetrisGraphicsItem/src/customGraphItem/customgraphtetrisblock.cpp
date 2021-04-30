#include "customgraphtetrisblock.h"

const int BLOCKSIDELENGTH = 4;
const int BLOCKDATASIZE = 16;

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

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos, int blockType)
{
	new (this)CustomGraphTetrisBlock(pos);
	this->blockType = blockType;
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(int blockType)
{
	new (this)CustomGraphTetrisBlock();
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
