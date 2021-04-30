#include "customgraphline.h"

CustomGraphLine::CustomGraphLine(CustomGraphPoint prevNode, CustomGraphPoint nextNode)
{
	this->prevNode = prevNode;
	this->nextNode = nextNode;
	this->options = Qjson();
}

CustomGraphLine::CustomGraphLine(CustomGraphPoint prevNode, CustomGraphPoint nextNode, Qjson options) {
	new (this)CustomGraphLine(prevNode, nextNode);
	this->options = options;
	if (options.IsObject()) {
		QString lwidth = options["lineWidth"];
		if (!lwidth.isEmpty()) {
			lineWidth = lwidth.toInt();
		}

		QString color = options["colorValue"];
		if (!color.isEmpty()) {
			colorValue = color.toInt();
		}

		QString fstyle = options["fillStyle"];
		if (!fstyle.isEmpty()) {
			this->fillStyle = fstyle.toInt();
		}

		QString idstr = options["id"];
		if (!idstr.isEmpty()) {
			id = idstr;
		}
	}
}

CustomGraphLine::CustomGraphLine(const CustomGraphLine& origin)
{
	new (this)CustomGraphLine(origin.prevNode, origin.nextNode, origin.options);
}

CustomGraphLine& CustomGraphLine::operator=(const CustomGraphLine& origin)
{
	new (this)CustomGraphLine(origin);
	return(*this);
}

int CustomGraphLine::type() const
{
	return Type;
}

QRectF CustomGraphLine::boundingRect() const
{
	const double diffX = qAbs(nextNode.scenePos().x() - prevNode.scenePos().x());
	const double diffY = qAbs(nextNode.scenePos().y() - prevNode.scenePos().y());
	int edgeWidth = 2;
	return QRectF(
		-diffX / 2 - edgeWidth, -diffY / 2 - edgeWidth,
		diffX + static_cast<qint64>(edgeWidth) * 2, diffY + static_cast<qint64>(edgeWidth) * 2
	);
}

void CustomGraphLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QBrush brushRef(Utils::getColorValueFromIndex(colorValue));
	painter->setPen(QPen(brushRef, lineWidth));
	painter->drawLine(QLineF(QPoint(0, 0), nextNode.scenePos() - prevNode.scenePos()));
}

Qjson CustomGraphLine::getFactors()
{
	return Qjson();
}

CustomGraphLine::~CustomGraphLine()
{
}

void CustomGraphLine::relocate()
{
	nextNode.relocate();
	prevNode.relocate();
	if (scenePos() != prevNode.scenePos())
	{
		setPos(prevNode.scenePos());
	}
}
