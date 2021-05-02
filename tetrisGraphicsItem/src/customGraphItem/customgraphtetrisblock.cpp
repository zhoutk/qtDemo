#include "customgraphtetrisblock.h"

const int BLOCKSIDEWIDTH = 30;
const int BLOCKSIDELENGTH = 4;
const int BLOCKDATASIZE = BLOCKSIDELENGTH * BLOCKSIDELENGTH;
QVector<QVector<int>> SHAPES = {
	{1, 1, 1, 1},
	{0, 1, 1, 1, 0 , 1},
	{1, 1, 1, 0, 0, 0, 1},
	{0, 1, 1, 0, 0, 1, 1},
	{1, 1, 0, 0, 0, 1, 1},
	{0, 1, 1, 0, 1, 1},
	{0, 1, 0, 0, 1, 1, 1} 
};

CustomGraphTetrisBlock::CustomGraphTetrisBlock() :
	pos(QPoint(0,0)), 
	sideLen(BLOCKSIDELENGTH), 
	blockType(0)
{
	for (int i = 0; i < sideLen; i++) 
		data.push_back(QVector<int>(sideLen));
	this->relocate();
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos)
{
	new (this)CustomGraphTetrisBlock();
	this->pos = pos;
	this->relocate();
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(int blockType)
{
	new (this)CustomGraphTetrisBlock();
	this->blockType = blockType;
	QVector<int> shape = SHAPES[blockType % SHAPES.size()];
	for (int i = 0; i < shape.size(); i++) {
		if (shape[i])
			data[1 + i / sideLen][i % sideLen] = true;
	}
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos, int blockType)
{
	new (this)CustomGraphTetrisBlock(blockType);
	this->pos = pos;
	this->relocate();
	this->blockType = blockType;
}

QRectF CustomGraphTetrisBlock::boundingRect() const
{
	return QRectF(
		0,
		0,
		BLOCKSIDELENGTH * BLOCKSIDEWIDTH,
		BLOCKSIDELENGTH * BLOCKSIDEWIDTH
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
	for (int i = 0; i < sideLen; i++)
	for(int j = 0; j < sideLen; j++){
		if (data[i][j])
		{
			painter->drawRoundedRect(
				j * BLOCKSIDEWIDTH, 
				i * BLOCKSIDEWIDTH, 
				BLOCKSIDEWIDTH, 
				BLOCKSIDEWIDTH, 
				2, 2
			);
		}
	}
	prepareGeometryChange();
}

bool CustomGraphTetrisBlock::rotate()
{
	int i, j, t, lenI = sideLen / 2, lenJ;
	for (i = 0; i < lenI; i++)
	{
		lenJ = sideLen - i - 1;
		for (j = i; j < lenJ; j++)
		{
			t = data[i][j];
			data[i][j] = data[sideLen - j - 1][i];
			data[sideLen - j - 1][i] = data[sideLen - i -1][sideLen - j - 1];
			data[sideLen - i - 1][sideLen - j - 1] = data[j][sideLen - i - 1];
			data[j][sideLen - i - 1] = t;
		}
	}
	return true;
}

void CustomGraphTetrisBlock::relocate()
{
	this->setPos(pos);
}

void CustomGraphTetrisBlock::relocate(QPoint p)
{
	this->pos = p;
	this->relocate();
}
