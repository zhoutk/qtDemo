#include "customgraphtetrisblock.h"
#include <QDebug>

extern const int BLOCKSIDEWIDTH;

CustomGraphTetrisBlock::CustomGraphTetrisBlock(int blockType) : pos(QPoint(0,0)), blockType(blockType)
{
	this->relocate();
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos, int blockType, int colorValue)
{
	new (this)CustomGraphTetrisBlock();
	this->pos = pos;
	this->blockType = blockType;
	this->colorValue = colorValue;
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
	if (blockType > 0) {
		QBrush brushRef(Utils::getColorValueFromIndex(colorValue));
		brushRef.setStyle(Qt::SolidPattern);
		painter->setBrush(brushRef);
		painter->setPen(QPen(Qt::yellow, lineWidth));
		painter->drawRoundedRect(
			0,
			0,
			BLOCKSIDEWIDTH,
			BLOCKSIDEWIDTH,
			2, 2
		);
	}
	//prepareGeometryChange();
	//qDebug() << "--------------------- block --------------------";
}

int CustomGraphTetrisBlock::getBlockType()
{
	return blockType;
}

bool CustomGraphTetrisBlock::isActive()
{
	return blockType == 2 || blockType < 0;
}

void CustomGraphTetrisBlock::setNotActive()
{
	this->blockType = 1;
}

void CustomGraphTetrisBlock::setTest()
{
	this->blockType = -1;
}

void CustomGraphTetrisBlock::relocate()
{
	this->setPos(pos * BLOCKSIDEWIDTH);
	prepareGeometryChange();
}

void CustomGraphTetrisBlock::relocate(QPoint p)
{
	this->pos = p;
	this->relocate();
}
