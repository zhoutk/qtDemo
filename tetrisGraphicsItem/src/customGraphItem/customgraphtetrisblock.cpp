#include "customgraphtetrisblock.h"

CustomGraphTetrisBlock::CustomGraphTetrisBlock()
{

}

QRectF CustomGraphTetrisBlock::boundingRect() const
{
	throw std::logic_error("The method or operation is not implemented.");
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
	throw std::logic_error("The method or operation is not implemented.");
}
