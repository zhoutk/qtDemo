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
	CustomGraphTetrisBlock();
	CustomGraphTetrisBlock(QPoint pos);
	CustomGraphTetrisBlock(QPoint pos, int blockType);
	CustomGraphTetrisBlock(int blockType);

	QRectF boundingRect() const override;
	int type() const override;
	void relocate() override;
	Qjson getFactors() override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QPoint pos;
	int blockType;
	QVector<bool> data;
	int sideLen;
};

#endif // CUSTOMGRAPHTETRISBLOCK_H
