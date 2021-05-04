#ifndef CUSTOMGRAPHTETRISBLOCK_H
#define CUSTOMGRAPHTETRISBLOCK_H
#include "customgraphbase.h"
#include <QPainter>
#include <QVector>
#include "customgraphitemtype.h"
#include "../common/Qjson.h"
#include "../common/utils.h"

class CustomGraphTetrisBlock : public CustomGraphBase
{
	Q_OBJECT
		enum {
		Type = TETRISBLOCKTYPE
	};
public:
	CustomGraphTetrisBlock(int blockType = 0);
	CustomGraphTetrisBlock(QPoint pos);
	CustomGraphTetrisBlock(QPoint pos, int blockType);

	QRectF boundingRect() const override;
	int type() const override;
	void relocate() override;
	void relocate(QPoint);
	Qjson getFactors() override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	bool rotate();
	bool moveLeft();
	bool moveRight();
	bool moveDown();
	void moveDownEnd();
	int cleanRow();
	bool canSee(int x, int y);
	void erase(int x, int y);

private:
	QPoint pos;
	int blockType;
	QVector<QVector<int>> data;
	int sideLen;
};

#endif // CUSTOMGRAPHTETRISBLOCK_H
