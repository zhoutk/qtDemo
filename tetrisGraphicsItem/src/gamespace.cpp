#include "gamespace.h"
#include "mainwindow.h"
#include "customGraphItem/customgraphtetrisbit.h"

extern const int BLOCKSIDEWIDTH;

GameSpace::GameSpace() : scene(nullptr)
{
	scene = MainWindow::GetApp()->GetScene();

	for (int i = 0; i < 12; i++)
	{
		scene->addItem(new CustomGraphTetrisBit(QPoint((i - 1) * BLOCKSIDEWIDTH, -BLOCKSIDEWIDTH)));
		scene->addItem(new CustomGraphTetrisBit(QPoint((i - 1) * BLOCKSIDEWIDTH, 20 * BLOCKSIDEWIDTH)));
	}
	for (int i = 0; i < 20; i++)
	{
		scene->addItem(new CustomGraphTetrisBit(QPoint(-BLOCKSIDEWIDTH, i * BLOCKSIDEWIDTH)));
		scene->addItem(new CustomGraphTetrisBit(QPoint(10 * BLOCKSIDEWIDTH, i * BLOCKSIDEWIDTH)));
	}
}
