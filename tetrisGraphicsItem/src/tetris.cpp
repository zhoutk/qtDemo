#include "tetris.h"
#include "mainwindow.h"
#include "QGraphicsScene"

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

Tetris::Tetris(int shape) :
	pos(QPoint(0,0)), 
	sideLen(BLOCKSIDELENGTH), 
	shape(shape),
	rotateNum(0)
{
	for (int i = 0; i < sideLen; i++) {
		data.push_back(QVector<int>(sideLen));
	}
	QVector<int> curShape = SHAPES[shape % SHAPES.size()];
	for (int i = 0; i < curShape.size(); i++) {
		if (curShape[i]) {
			data[1 + i / sideLen][i % sideLen] = true;
			CustomGraphTetrisBlock* block = new CustomGraphTetrisBlock(pos + QPoint(i % sideLen, 1 + i / sideLen), 2, shape);
			blocks.push_back(block);
			MainWindow::GetApp()->GetScene()->addItem(block);
		}
	}
	//this->relocate();
}

Tetris::Tetris(QPoint pos)
{
	new (this)Tetris();
	this->pos = pos;
	this->relocate();
}

Tetris::Tetris(QPoint pos, int shape)
{
	this->shape = shape;
	new (this)Tetris(shape);
	this->pos = pos;
	this->relocate();
}

bool Tetris::rotate()
{
    int i, j, t, lenHalf = sideLen / 2, lenJ;
    for (i = 0; i < lenHalf; i++)
    {
        lenJ = sideLen - i - 1;
		for (j = i; j < lenJ; j++)
		{
			int lenI = sideLen - j - 1;
			if (data[i][j] && this->hasTetrisBlock(pos.x() + lenJ, pos.y() + j) ||
				data[lenI][i] && this->hasTetrisBlock(pos.x() + j, pos.y() + i) ||
				data[lenJ][lenI] && this->hasTetrisBlock(pos.x() + i, pos.y() + lenI) ||
				data[j][lenJ] && this->hasTetrisBlock(pos.x() + lenI, pos.y() + lenJ)){
                return false;
            }
        }
    }
    for (i = 0; i < lenHalf; i++)
	{
		lenJ = sideLen - i - 1;
		for (j = i; j < lenJ; j++)
		{
            int lenI = sideLen - j - 1;
			t = data[i][j];
            data[i][j] = data[lenI][i];
            data[lenI][i] = data[lenJ][lenI];
            data[lenJ][lenI] = data[j][lenJ];
            data[j][lenJ] = t;
		}
	}
	this->rotateNum++;
	this->relocate();
	return true;
}

bool Tetris::canContinue()
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (data[i][j]) {
				if (this->hasTetrisBlock(pos.x() + j, pos.y() + i)) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Tetris::moveLeft()
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (data[i][j] && (j == 0 || data[i][j - 1] == 0)) {
				if (this->hasTetrisBlock(pos.x() + j - 1, pos.y() + i)) {
					return false;
				}
			}
		}
	}
	pos.setX(pos.x() - 1);
	this->relocate();
	return true;
}

bool Tetris::moveRight()
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (data[i][j] && (j == 3 || data[i][j + 1] == 0)) {
				if (this->hasTetrisBlock(pos.x() + j + 1, pos.y() + i)) {
					return false;
				}
			}
		}
	}
	pos.setX(pos.x() + 1);
	this->relocate();
	return true;
}

bool Tetris::moveDown()
{
	bool canMove = true;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (data[i][j] && (i == 3 || data[i + 1][j] == 0)) {
				if (this->hasTetrisBlock(pos.x() + j, pos.y() + i + 1)) {
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
		foreach(auto block, blocks)
			block->setNotActive();
		return false;
	}
}



void Tetris::setBlockNotActive()
{
	foreach(auto block, blocks)
		block->setNotActive();
}

void Tetris::moveDownEnd()
{
	while (this->moveDown());
}

int Tetris::cleanRow()
{
	int h = 19, levelCount = 0;
	while (h >= 0) {
		int count = 0;
		for (int i = 0; i < 10; i++) {
			if (!this->hasTetrisBlock(i, h)) {
				count++;
			}
		}
		if (count == 0) {
			int level = h;
			levelCount++;
			bool first = true;
			while (level >= 0) {
				int ct = 0;
				for (int j = 0; j < 10; j++) {
					if(first)
						this->erase(j, level);
					CustomGraphTetrisBlock* block = this->hasTetrisBlock(j, level - 1);
					if (!block) {
						ct++;
					}
					else {
						block->relocate(QPoint(j, level));
					}
				}
				first = false;
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

CustomGraphTetrisBlock* Tetris::hasTetrisBlock(int x, int y)
{
	auto items = MainWindow::GetApp()->GetScene()->items(QPointF((x + 0.5) * BLOCKSIDEWIDTH, (y + 0.5) * BLOCKSIDEWIDTH));
	foreach (auto al , items)
	{
		if (!(((CustomGraphBase*)al)->isActive()) && (((CustomGraphBase*)al)->type()) == TETRISBLOCKTYPE) {
			return (CustomGraphTetrisBlock*)al;
		}
	}
	return nullptr;
}

void Tetris::erase(int x, int y)
{
	auto items = MainWindow::GetApp()->GetScene()->items(QPointF((x + 0.5) * BLOCKSIDEWIDTH, (y + 0.5) * BLOCKSIDEWIDTH));
	foreach(auto al, items)
	{
		if ((((CustomGraphBase*)al)->getBlockType() == 1) && (((CustomGraphBase*)al)->type()) == TETRISBLOCKTYPE) {
			MainWindow::GetApp()->GetScene()->removeItem(al);
			return;
		}
	}
}

QPoint Tetris::getPos()
{
	return this->pos;
}

int Tetris::getRotateNum()
{
	return this->rotateNum;
}

int Tetris::getBlockType()
{
	return this->shape;
}

void Tetris::relocate()
{
	int index = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (data[i][j]) {
				blocks[index++]->relocate(pos + QPoint(j, i));
			}
		}
	}
}

void Tetris::relocate(QPoint p)
{
	this->pos = p;
	this->relocate();
}

