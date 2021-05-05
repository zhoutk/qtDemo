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

Tetris::Tetris(int blockType) :
	pos(QPoint(0,0)), 
	sideLen(BLOCKSIDELENGTH), 
	blockType(blockType)
{
	for (int i = 0; i < sideLen; i++) {
		data.push_back(QVector<int>(sideLen));
	}
	QVector<int> shape = SHAPES[blockType % SHAPES.size()];
	for (int i = 0; i < shape.size(); i++) {
		if (shape[i]) {
			data[1 + i / sideLen][i % sideLen] = true;
			CustomGraphTetrisBlock* block = new CustomGraphTetrisBlock(pos + QPoint(i % sideLen, 1 + i / sideLen), 2);
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

Tetris::Tetris(QPoint pos, int blockType)
{
	this->blockType = blockType;
	new (this)Tetris(blockType);
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
			if (data[i][j] && !this->canSee(pos.x() + lenJ, pos.y() + j) ||
				data[lenI][i] && !this->canSee(pos.x() + j, pos.y() + i) ||
				data[lenJ][lenI] && !this->canSee(pos.x() + i, pos.y() + lenI) ||
				data[j][lenJ] && !this->canSee(pos.x() + lenI, pos.y() + lenJ)){
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
	this->relocate();
	return true;
}

bool Tetris::canContinue()
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (data[i][j]) {
				if (!this->canSee(pos.x() + j, pos.y() + i)) {
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

bool Tetris::moveRight()
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

bool Tetris::moveDown()
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
		foreach(auto block, blocks)
			block->setNotActive();
		return false;
	}
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
						MainWindow::GetApp()->GetScene()->addItem(new CustomGraphTetrisBlock(QPoint(j, level - 1), 1));
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

bool Tetris::canSee(int x, int y)
{
	auto items = MainWindow::GetApp()->GetScene()->items(QPointF((x + 0.5) * BLOCKSIDEWIDTH, (y + 0.5) * BLOCKSIDEWIDTH));
	foreach (auto al , items)
	{
		if (!(((CustomGraphBase*)al)->isActive()) && (((CustomGraphBase*)al)->type()) == TETRISBLOCKTYPE) {
			return false;
		}
	}
	return true;
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

