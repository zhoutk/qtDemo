#ifndef CUSTOMGRAPHTETRISTEXT_H
#define CUSTOMGRAPHTETRISTEXT_H
#include "customgraphbase.h"
#include <QPainter>
#include <QVector>
#include "customgraphitemtype.h"
#include "../common/Qjson.h"
#include "../common/utils.h"

class CustomGraphTetrisText : public CustomGraphBase
{
	Q_OBJECT
		enum {
		Type = TETRISTEXTTYPE
	};
public:
	CustomGraphTetrisText(QString text = "Game Over!");
	CustomGraphTetrisText(QPoint pos, QString text = "Game Over!");

	QRectF boundingRect() const override;
	int type() const override;
	void relocate() override;
	void relocate(QPoint);
	Qjson getFactors() override;
	int getBlockType() override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QPoint pos;
	QString text;
	int blockType;		//0-border; 1-fix block; 2-active; 5-text;
};

#endif // CUSTOMGRAPHTETRISTEXT_H
