#include "customgraphtetrisblock.h"

extern const int BLOCKSIDEWIDTH;

CustomGraphTetrisBlock::CustomGraphTetrisBlock(int blockType) : pos(QPoint(0,0)), blockType(blockType)
{
	this->relocate();
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos, int blockType)
{
	new (this)CustomGraphTetrisBlock();
	this->pos = pos;
	this->blockType = blockType;
	this->relocate();
}

QRectF CustomGraphTetrisBlock::boundingRect() const
{
	return QRectF(
		0,
		0,
		BLOCKSIDEWIDTH,
		BLOCKSIDEWIDTH
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
	if (blockType) {
		painter->drawRoundedRect(
			0,
			0,
			BLOCKSIDEWIDTH,
			BLOCKSIDEWIDTH,
			2, 2
		);
	}
	prepareGeometryChange();
}

bool CustomGraphTetrisBlock::isActive()
{
	return blockType == 2;
}

void CustomGraphTetrisBlock::relocate()
{
	this->setPos(pos * BLOCKSIDEWIDTH);
}

void CustomGraphTetrisBlock::relocate(QPoint p)
{
	this->pos = p;
	this->relocate();
}
