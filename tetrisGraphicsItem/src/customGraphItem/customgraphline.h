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
	void relocate() override;
	Qjson getFactors();

private:
	CustomGraphPoint prevNode;
	CustomGraphPoint nextNode;
	Qjson options;
};

#endif // CUSTOMGRAPHLINE_H
