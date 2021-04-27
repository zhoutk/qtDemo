#ifndef CUSTOMGRAPHTETRISBLOCK_H
#define CUSTOMGRAPHTETRISBLOCK_H
#include "customgraphbase.h"
#include <QPainter>
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

	QRectF boundingRect() const override;
	int type() const override;
	Qjson getFactors() override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

};

#endif // CUSTOMGRAPHTETRISBLOCK_H
