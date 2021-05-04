#include "customgraphtetrisblock.h"

extern const int BLOCKSIDEWIDTH;

CustomGraphTetrisBlock::CustomGraphTetrisBlock(int biType) : pos(QPoint(0,0)), biType(biType)
{
	this->relocate();
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos, int biType)
{
	new (this)CustomGraphTetrisBlock();
	this->pos = pos;
	this->biType = biType;
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
	if (biType) {
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

void CustomGraphTetrisBlock::relocate()
{
	this->setPos(pos * BLOCKSIDEWIDTH);
}

void CustomGraphTetrisBlock::relocate(QPoint p)
{
	this->pos = p;
	this->relocate();
}
