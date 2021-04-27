#include "customgraphpoint.h"

CustomGraphPoint::CustomGraphPoint(QPointF localtion) {
	this->localtion = localtion;
	this->options = Qjson();
	this->pointRadius = 4;
}

CustomGraphPoint::CustomGraphPoint(double lon, double lat)
{
	new (this)CustomGraphPoint(QPointF(lon, lat));		
}

CustomGraphPoint::CustomGraphPoint(QPointF localtion, Qjson options){
	new (this)CustomGraphPoint(localtion);
	this->options = options;
	if (options.IsObject()) {
		QString lwidth = options["lineWidth"];
		if (!lwidth.isEmpty()) {
			this->lineWidth = lwidth.toInt();
		}

		QString color = options["colorValue"];
		if (!color.isEmpty()) {
			this->colorValue = color.toInt();
		}

		QString fstyle = options["fillStyle"];
		if (!fstyle.isEmpty()) {
			this->fillStyle = fstyle.toInt();
		}

		QString idstr = options["id"];
		if (!idstr.isEmpty()) {
			id = idstr;
		}
	}
}

CustomGraphPoint::CustomGraphPoint(double lat, double lon, Qjson options)
{
	new (this)CustomGraphPoint(QPointF(lon, lat), options);
}

CustomGraphPoint::CustomGraphPoint(const CustomGraphPoint& origin) {
	new (this)CustomGraphPoint(origin.localtion, origin.options);
}

CustomGraphPoint& CustomGraphPoint::operator=(const CustomGraphPoint& origin)
{
	new (this)CustomGraphPoint(origin);
	return(*this);
}

int CustomGraphPoint::type() const
{
	return Type;
}

Qjson CustomGraphPoint::getFactors()
{
	Qjson factor;
	factor.AddValueBase("lon", localtion.x());
	factor.AddValueBase("lat", localtion.y());
	factor.AddValueBase("coordx", coordinate.x());
	factor.AddValueBase("coordy", coordinate.y());
	return factor;
}

QRectF CustomGraphPoint::boundingRect() const
{
	return QRectF(-pointRadius, -pointRadius, static_cast<qint64>(pointRadius) *2, static_cast<qint64>(pointRadius) *2);
}

void CustomGraphPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
	QBrush brushRef(Utils::getColorValueFromIndex(colorValue));
	painter->setPen(QPen(brushRef, lineWidth));
	painter->drawEllipse(-pointRadius, -pointRadius, static_cast<qint64>(pointRadius) * 2, static_cast<qint64>(pointRadius) * 2);
}

CustomGraphPoint::~CustomGraphPoint()
{

}