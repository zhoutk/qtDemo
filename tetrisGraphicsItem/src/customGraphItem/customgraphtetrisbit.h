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
		Type = TETRISBITTYPE
	};
public:
	CustomGraphTetrisBit(int biType = 0);
	CustomGraphTetrisBit(QPoint pos, int biType = 0);

	QRectF boundingRect() const override;
	int type() const override;
	void relocate() override;
	void relocate(QPoint);
	Qjson getFactors() override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QPoint pos;
	int biType;		//0-border; 1-fix block;
};

#endif // CUSTOMGRAPHTETRISBIT_H
