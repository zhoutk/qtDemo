#ifndef CUSTOMGRAPHBASE_H
#define CUSTOMGRAPHBASE_H

#include <QObject>
#include <QGraphicsObject>
#include "../common/Rjson.h"
#include "../common/snowflake.h"

extern SnowFlake snowFlakeId;

class CustomGraphBase : public QGraphicsObject
{
    Q_OBJECT
public:
	CustomGraphBase();
	CustomGraphBase(QString id);
	~CustomGraphBase();

	QString getGraphItemId() {
		return id;
	}

protected:
	QString id;

public:
	virtual QRectF boundingRect() const = 0;
	virtual void relocate() = 0;
	virtual int type() const = 0;
	virtual Rjson getFactors() = 0;
};

#endif // CUSTOMGRAPHBASE_H
