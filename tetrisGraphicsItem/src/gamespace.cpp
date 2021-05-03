#include "gamespace.h"
#include "mainwindow.h"
#include "customGraphItem/customgraphtetrisbit.h"

GameSpace::GameSpace() : scene(nullptr)
{
	scene = MainWindow::GetApp()->GetScene();

	scene->addItem(new CustomGraphTetrisBit());
}
