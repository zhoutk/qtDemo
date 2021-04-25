#include "customgraphpoint.h"

CustomGraphPoint::CustomGraphPoint(QPointF localtion) {
	this->localtion = localtion;
	this->options = Rjson();
	this->pointRadius = 4;
	this->lineWidth = 1;
	this->colorValue = 0;
	this->fillStyle = 0;
	this->relocate();
	setFlag(ItemIsMovable);
}

CustomGraphPoint::CustomGraphPoint(double lon, double lat)
{
	new (this)CustomGraphPoint(QPointF(lon, lat));		
}

CustomGraphPoint::CustomGraphPoint(QPointF localtion, Rjson options){
	new (this)CustomGraphPoint(localtion);
	this->options = options;
	if (options.IsObject()) {
		QString lwidth = QString::fromStdString(options["lineWidth"]);
		if (!lwidth.isEmpty()) {
			this->lineWidth = lwidth.toInt();
		}

		QString color = QString::fromStdString(options["colorValue"]);
		if (!color.isEmpty()) {
			this->colorValue = color.toInt();
		}

		QString fstyle = QString::fromStdString(options["fillStyle"]);
		if (!fstyle.isEmpty()) {
			this->fillStyle = fstyle.toInt();
		}

		QString idstr = QString::fromStdString(options["id"]);
		if (!idstr.isEmpty()) {
			id = idstr;
		}
	}
}

CustomGraphPoint::CustomGraphPoint(double lat, double lon, Rjson options)
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

Rjson CustomGraphPoint::getFactors()
{
	Rjson factor;
	factor.AddValueFloat("lon", localtion.x());
	factor.AddValueFloat("lat", localtion.y());
	factor.AddValueFloat("coordx", coordinate.x());
	factor.AddValueFloat("coordy", coordinate.y());
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

void CustomGraphPoint::relocate()
{
}

CustomGraphPoint::~CustomGraphPoint()
{

}