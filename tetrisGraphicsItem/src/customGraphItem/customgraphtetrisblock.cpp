#include "customgraphtetrisblock.h"
#include "../mainwindow.h"
#include "QGraphicsScene"
#include "customgraphtetrisbit.h"

extern const int BLOCKSIDEWIDTH = 30;
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

CustomGraphTetrisBlock::CustomGraphTetrisBlock(int blockType) :
	pos(QPoint(0,0)), 
	sideLen(BLOCKSIDELENGTH), 
	blockType(blockType)
{
	for (int i = 0; i < sideLen; i++) 
		data.push_back(QVector<int>(sideLen));
	QVector<int> shape = SHAPES[blockType % SHAPES.size()];
	for (int i = 0; i < shape.size(); i++) {
		if (shape[i])
			data[1 + i / sideLen][i % sideLen] = true;
	}
	this->relocate();
}

CustomGraphTetrisBlock::CustomGraphTetrisBlock(QPoint pos)
{
	new (this)CustomGraphTetrisBlock();
	this->pos = pos;
	this->relocate();
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

bool CustomGraphTetrisBlock::moveLeft()
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (data[i][j] && (j == 0 || data[i][j - 1] == 0)) {
				if (!this->canSee(pos.x() + j - 1, pos.y() + i)) {
					return false;
				}
			}
		}
	}
	pos.setX(pos.x() - 1);
	this->relocate();
	return true;
}

bool CustomGraphTetrisBlock::moveRight()
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (data[i][j] && (j == 3 || data[i][j + 1] == 0)) {
				if (!this->canSee(pos.x() + j + 1, pos.y() + i)) {
					return false;
				}
			}
		}
	}
	pos.setX(pos.x() + 1);
	this->relocate();
	return true;
}

bool CustomGraphTetrisBlock::moveDown()
{
	bool canMove = true;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (data[i][j] && (i == 3 || data[i + 1][j] == 0)) {
				if (!this->canSee(pos.x() + j, pos.y() + i + 1)) {
					canMove = false;
					break;
				}
			}
		}
		if (!canMove) {
			break;
		}
	}
	if (canMove) {
		pos.setY(pos.y() + 1);
		this->relocate();
		return true;
	}
	else {
		return false;
	}
}

void CustomGraphTetrisBlock::moveDownEnd()
{
	while (this->moveDown());
}

int CustomGraphTetrisBlock::cleanRow()
{
	int h = 19, levelCount = 0;
	while (h >= 0) {
		int count = 0;
		for (int i = 0; i < 10; i++) {
			if (this->canSee(i, h)) {
				count++;
			}
		}
		if (count == 0) {
			int level = h;
			levelCount++;
			while (level >= 0) {
				int ct = 0;
				for (int j = 0; j < 10; j++) {
					this->erase(j, level);
					if (this->canSee(j, level - 1)) {
						ct++;
					}
					else {
						MainWindow::GetApp()->GetScene()->addItem(new CustomGraphTetrisBit(QPoint(j, level - 1), 1));
					}
				}
				if (ct == 10) {
					break;
				}
				else {
					level--;
				}
			}
		}
		else if (count == 10) {
			break;
		}
		else {
			h--;
		}
	}
	return levelCount;
}

bool CustomGraphTetrisBlock::canSee(int x, int y)
{
	auto items = MainWindow::GetApp()->GetScene()->items(QPointF((x + 0.5) * BLOCKSIDEWIDTH, (y + 0.5) * BLOCKSIDEWIDTH));
	foreach (auto al , items)
	{
		if ((((CustomGraphBase*)al)->type()) == TETRISBITTYPE) {
			return false;
		}
	}
	return true;
}

void CustomGraphTetrisBlock::erase(int x, int y)
{
	auto items = MainWindow::GetApp()->GetScene()->items(QPointF((x + 0.5) * BLOCKSIDEWIDTH, (y + 0.5) * BLOCKSIDEWIDTH));
	foreach(auto al, items)
	{
		if ((((CustomGraphBase*)al)->type()) == TETRISBITTYPE) {
			MainWindow::GetApp()->GetScene()->removeItem(al);
			return;
		}
	}
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
