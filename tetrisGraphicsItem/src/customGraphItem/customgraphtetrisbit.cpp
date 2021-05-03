#include "customgraphtetrisbit.h"

extern const int BLOCKSIDEWIDTH;

CustomGraphTetrisBit::CustomGraphTetrisBit() : pos(QPoint(0,0))
{
	this->relocate();
}

CustomGraphTetrisBit::CustomGraphTetrisBit(QPoint pos)
{
	new (this)CustomGraphTetrisBit();
	this->pos = pos;
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
	painter->drawRoundedRect(
		0,
		0,
		BLOCKSIDEWIDTH,
		BLOCKSIDEWIDTH,
		2, 2
	);
	prepareGeometryChange();
}

void CustomGraphTetrisBit::relocate()
{
	this->setPos(pos);
}

void CustomGraphTetrisBit::relocate(QPoint p)
{
	this->pos = p;
	this->relocate();
}
