#include "gamespace.h"
#include "mainwindow.h"
#include "customGraphItem/customgraphtetrisbit.h"

extern const int BLOCKSIDEWIDTH;

GameSpace::GameSpace() : scene(nullptr)
{
	scene = MainWindow::GetApp()->GetScene();

	for (int i = 0; i < 12; i++)
	{
		//scene->addItem(new CustomGraphTetrisBit(QPoint(i - 1, -1)));
		scene->addItem(new CustomGraphTetrisBit(QPoint(i - 1, 20)));
	}
	for (int i = 0; i < 20; i++)
	{
		scene->addItem(new CustomGraphTetrisBit(QPoint(-1, i)));
		scene->addItem(new CustomGraphTetrisBit(QPoint(10, i)));
	}
}
