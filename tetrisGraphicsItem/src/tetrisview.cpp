#include "tetrisview.h"
#include "mainwindow.h"

extern bool isRunning;

TetrisView::TetrisView(QWidget*parent) : QGraphicsView(parent)
{
	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	setWindowFlags(Qt::FramelessWindowHint);
	setStyleSheet("padding: 0px; border: 0px;");
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setCacheMode(CacheBackground);
}

void TetrisView::keyPressEvent(QKeyEvent* event)
{
	if (isRunning) {
		if (event->key() == Qt::Key_Up) {
			MainWindow::GetApp()->GetGame()->rotateBlock();
		}
		else if (event->key() == Qt::Key_Left) {
			MainWindow::GetApp()->GetGame()->moveBlockLeft();
		}
		else if (event->key() == Qt::Key_Right) {
			MainWindow::GetApp()->GetGame()->moveBlockRight();
		}
		else if (event->key() == Qt::Key_Down) {
			MainWindow::GetApp()->GetGame()->moveBlockDown();
		}
		else if (event->key() == Qt::Key_Space) {
			MainWindow::GetApp()->GetGame()->moveBlockDownEnd();
		}
	}
	
}
