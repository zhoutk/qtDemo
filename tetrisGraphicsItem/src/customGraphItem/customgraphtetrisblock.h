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
	CustomGraphTetrisBlock(QPoint pos, int blockType = 0, int colorValue = 2);

	QRectF boundingRect() const override;
	int type() const override;
	void relocate() override;
	void relocate(QPoint);
	Qjson getFactors() override;
	bool isActive() override;
	void setNotActive();
	int getBlockType() override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QPoint pos;
	int blockType;		//0-border; 1-fix block; 2-active; 5-text;
};

#endif // CUSTOMGRAPHTETRISBLOCK_H
