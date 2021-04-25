#ifndef CUSTOMGRAPHLINE_H
#define CUSTOMGRAPHLINE_H

#include "customgraphbase.h"
#include <QPainter>
#include "customgraphitemtype.h"
#include "../common/Rjson.h"
#include "../common/utils.h"
#include "customgraphpoint.h"

class CustomGraphLine : public CustomGraphBase
{
	enum {
		Type = LINETYPE
	};
public:
	CustomGraphLine(CustomGraphPoint prevNode, CustomGraphPoint nextNode);
	CustomGraphLine(CustomGraphPoint prevNode, CustomGraphPoint nextNode, Rjson options);

	CustomGraphLine(const CustomGraphLine& origin);
	CustomGraphLine& operator = (const CustomGraphLine& origin);

	~CustomGraphLine();

public:
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
	void relocate();
	int type() const;
	Rjson getFactors();

private:
	CustomGraphPoint prevNode;
	CustomGraphPoint nextNode;
	Rjson options;

	int lineWidth;
	int colorValue;
	int fillStyle;
};

#endif // CUSTOMGRAPHLINE_H
