#include "tetrisview.h"

TetrisView::TetrisView(QWidget*parent) : QGraphicsView(parent)
{
	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	setWindowFlags(Qt::FramelessWindowHint);
	setStyleSheet("padding: 0px; border: 0px;");
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setCacheMode(CacheBackground);
}
