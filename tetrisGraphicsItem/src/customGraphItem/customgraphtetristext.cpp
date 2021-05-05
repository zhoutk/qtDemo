#include "customgraphtetristext.h"

CustomGraphTetrisText::CustomGraphTetrisText(QString text) : pos(QPoint(0,0)), blockType(5), text(text)
{
	this->relocate();
}

CustomGraphTetrisText::CustomGraphTetrisText(QPoint pos, QString text)
{
	new (this)CustomGraphTetrisText(text);
	this->pos = pos;
	this->relocate();
}

QRectF CustomGraphTetrisText::boundingRect() const
{
	return QRectF(
		-100,
		-50,
		400,
		100
	);
}

int CustomGraphTetrisText::type() const
{
	return Type;
}

Qjson CustomGraphTetrisText::getFactors()
{
	return Qjson();
}

void CustomGraphTetrisText::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget /*= nullptr*/)
{
	painter->setFont(QFont("Microsoft YaHei", 32));
	painter->setPen(QPen(Qt::red));
	painter->drawText(0, 0, text);
	prepareGeometryChange();
}

int CustomGraphTetrisText::getBlockType()
{
	return blockType;
}

void CustomGraphTetrisText::relocate()
{
	this->setPos(pos * 30);
}

void CustomGraphTetrisText::relocate(QPoint p)
{
	this->pos = p;
	this->relocate();
}
