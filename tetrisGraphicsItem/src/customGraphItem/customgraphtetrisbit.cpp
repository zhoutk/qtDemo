#include "customgraphtetrisbit.h"

extern const int BLOCKSIDEWIDTH;

CustomGraphTetrisBit::CustomGraphTetrisBit(int biType) : pos(QPoint(0,0)), biType(biType)
{
	this->relocate();
}

CustomGraphTetrisBit::CustomGraphTetrisBit(QPoint pos, int biType)
{
	new (this)CustomGraphTetrisBit();
	this->pos = pos;
	this->biType = biType;
	this->relocate();
}

QRectF CustomGraphTetrisBit::boundingRect() const
{
	return QRectF(
		0,
		0,
		BLOCKSIDEWIDTH,
		BLOCKSIDEWIDTH
	);
}

int CustomGraphTetrisBit::type() const
{
	return Type;
}

Qjson CustomGraphTetrisBit::getFactors()
{
	return Qjson();
}

void CustomGraphTetrisBit::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*= nullptr*/)
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

void CustomGraphTetrisBit::relocate()
{
	this->setPos(pos * BLOCKSIDEWIDTH);
}

void CustomGraphTetrisBit::relocate(QPoint p)
{
	this->pos = p;
	this->relocate();
}
