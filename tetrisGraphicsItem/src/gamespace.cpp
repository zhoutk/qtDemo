#include "gamespace.h"
#include "mainwindow.h"
#include "customGraphItem/customgraphtetrisblock.h"

extern const int BLOCKSIDEWIDTH;

GameSpace::GameSpace() : scene(nullptr)
{
	scene = MainWindow::GetApp()->GetScene();

	for (int i = 0; i < 12; i++)
	{
		//scene->addItem(new CustomGraphTetrisBit(QPoint(i - 1, -1)));
		scene->addItem(new CustomGraphTetrisBlock(QPoint(i - 1, 20)));
	}
	for (int i = 0; i < 20; i++)
	{
		scene->addItem(new CustomGraphTetrisBlock(QPoint(-1, i)));
		scene->addItem(new CustomGraphTetrisBlock(QPoint(10, i)));
	}
}
