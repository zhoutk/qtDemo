#ifndef CUSTOMGRAPHBASE_H
#define CUSTOMGRAPHBASE_H

#include <QObject>
#include <QGraphicsObject>
#include "../common/Qjson.h"
#include "../common/snowflake.h"

extern SnowFlake snowFlakeId;

class CustomGraphBase : public QGraphicsObject
{
    Q_OBJECT
public:
	CustomGraphBase();
	CustomGraphBase(QString id);
	~CustomGraphBase();

	QString getID() {
		return id;
	}

protected:
	QString id;
	int lineStyle;
	int lineWidth;
	int colorValue;
	int fillStyle;

public:
	virtual QRectF boundingRect() const = 0;
	virtual int type() const = 0;
	virtual void relocate() = 0;
	virtual Qjson getFactors() = 0;
	virtual bool isActive() { return false; };
};

#endif // CUSTOMGRAPHBASE_H
