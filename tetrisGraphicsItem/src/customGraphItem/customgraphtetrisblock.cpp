#include "customgraphtetrisblock.h"

const int BLOCKSIDEWIDTH = 30;
const int BLOCKSIDELENGTH = 4;
const int BLOCKDATASIZE = BLOCKSIDELENGTH * BLOCKSIDELENGTH;
QVector<QVector<int>> SHAPES = {
	{1, 1, 1, 1},
	{1, 1, 1, 0, 1},
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
			data[i % sideLen][1 + i / sideLen] = true;
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
				i * BLOCKSIDEWIDTH, 
				j * BLOCKSIDEWIDTH, 
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
	//int t;
	//for (int i = 0; i < BLOCKSIDELENGTH / 2; i++)
	//{
	//	for (int j = i; j < BLOCKSIDELENGTH - i ; j++)
	//	{
	//		t = data[i * BLOCKSIDELENGTH + j];
	//		data[i * BLOCKSIDELENGTH + j] = data[(BLOCKSIDELENGTH - j - 1) * BLOCKSIDELENGTH +i];
	//		data[(BLOCKSIDELENGTH - j - 1) * BLOCKSIDELENGTH + i] = data[(BLOCKSIDELENGTH - i - 1) * BLOCKSIDELENGTH + BLOCKSIDELENGTH - j - 1];
	//		data[(BLOCKSIDELENGTH - i - 1) * BLOCKSIDELENGTH + BLOCKSIDELENGTH - j - 1] = data[j * BLOCKSIDELENGTH + BLOCKSIDELENGTH - i - 1];
	//		data[j * BLOCKSIDELENGTH + BLOCKSIDELENGTH - i - 1] = t;
	//	}
	//}
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
