#include "customgraphtetrisblock.h"

CustomGraphTetrisBlock::CustomGraphTetrisBlock():pos(QPoint(0,0)), sideLen(4), data(QVector<bool>(sideLen*sideLen)),blockType(0)
{
	foreach (auto al, data)
		al = false;
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos): pos(pos)
{
	new (this)CustomGraphTetrisBlock();
	this->pos = pos;
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos, int blockType) : pos(pos), blockType(blockType)
{
	new (this)CustomGraphTetrisBlock(pos);
	this->blockType = blockType;
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(int blockType): blockType(blockType)
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
