#ifndef CUSTOMGRAPHTETRISBIT_H
#define CUSTOMGRAPHTETRISBIT_H
#include "customgraphbase.h"
#include <QPainter>
#include <QVector>
#include "customgraphitemtype.h"
#include "../common/Qjson.h"
#include "../common/utils.h"

class CustomGraphTetrisBit : public CustomGraphBase
{
	Q_OBJECT
		enum {
		Type = TETRISBLOCKTYPE
	};
public:
	CustomGraphTetrisBit();
	CustomGraphTetrisBit(QPoint pos);

	QRectF boundingRect() const override;
	int type() const override;
	void relocate() override;
	void relocate(QPoint);
	Qjson getFactors() override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	bool rotate();

private:
	QPoint pos;
	int blockType;
};

#endif // CUSTOMGRAPHTETRISBIT_H
