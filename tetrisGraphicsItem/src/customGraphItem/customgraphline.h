#ifndef CUSTOMGRAPHLINE_H
#define CUSTOMGRAPHLINE_H

#include "customgraphbase.h"
#include <QPainter>
#include "customgraphitemtype.h"
#include "../common/Qjson.h"
#include "../common/utils.h"
#include "customgraphpoint.h"

class CustomGraphLine : public CustomGraphBase
{
	enum {
		Type = LINETYPE
	};
public:
	CustomGraphLine(CustomGraphPoint prevNode, CustomGraphPoint nextNode);
	CustomGraphLine(CustomGraphPoint prevNode, CustomGraphPoint nextNode, Qjson options);

	CustomGraphLine(const CustomGraphLine& origin);
	CustomGraphLine& operator = (const CustomGraphLine& origin);

	~CustomGraphLine();

public:
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
	int type() const;
	Qjson getFactors();

private:
	CustomGraphPoint prevNode;
	CustomGraphPoint nextNode;
	Qjson options;

	int lineWidth;
	int colorValue;
	int fillStyle;
};

#endif // CUSTOMGRAPHLINE_H
