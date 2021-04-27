#ifndef CUSTOMGRAPHPOINT_H
#define CUSTOMGRAPHPOINT_H

#include "customgraphbase.h"
#include <QPainter>
#include "customgraphitemtype.h"
#include "../common/Qjson.h"
#include "../common/utils.h"

class CustomGraphPoint : public CustomGraphBase
{
	Q_OBJECT
	enum {
		Type = POINTTYPE
	};
public:
	CustomGraphPoint(QPointF localtion);
	CustomGraphPoint(double lat = 0.0, double lon = 0.0);
	CustomGraphPoint(QPointF localtion, Qjson options);
	CustomGraphPoint(double lat, double lon, Qjson options);
	CustomGraphPoint(const CustomGraphPoint& origin);
	CustomGraphPoint& operator = (const CustomGraphPoint& origin);
	~CustomGraphPoint();

public:
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
	void relocate();
	int type() const;
	Qjson getFactors();

private:
	QPointF localtion;
	QPoint coordinate;
	Qjson options;
	int pointRadius;
	
	int lineWidth;
	int colorValue;
	int fillStyle;
};

#endif // CUSTOMGRAPHPOINT_H
