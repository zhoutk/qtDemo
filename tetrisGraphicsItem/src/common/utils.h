#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QCoreApplication>

class Utils
{
public:
	Utils() {};

	static Qt::PenStyle getLineStyleFromIndex(int index) {
		switch (index)
		{
		case 0:
			return Qt::SolidLine;
			break;
		case 1:
			return Qt::DashLine;
			break;
		case 2:
			return Qt::DotLine;
			break;
		case 3:
			return Qt::DashDotLine;
			break;
		case 4:
			return Qt::DashDotDotLine;
			break;
		case 5:
			return Qt::CustomDashLine;
			break;
		case 9:
			return Qt::NoPen;
			break;
		default:
			return Qt::SolidLine;
			break;
		}
	}

	static Qt::GlobalColor getColorValueFromIndex(int index) {
		switch (index)
		{
		case 0:
			return Qt::red;
			break;
		case 1:
			return Qt::blue;
			break;
		case 2:
			return Qt::green;
			break;
		case 3:
			return Qt::gray;
			break;
		case 4:
			return Qt::darkMagenta;
			break;
		case 5:
			return Qt::yellow;
			break;
		case 6:
			return Qt::black;
			break;
		case 9:
			return Qt::darkCyan;
			break;
		case 10:
			return Qt::darkYellow;
			break;
		default:
			return Qt::red;
			break;
		}
	}

	static Qt::BrushStyle getBrushStyleFromIndex(int index) {
		switch (index)
		{
		case 0:
			return Qt::NoBrush;
			break;
		case 1:
			return Qt::Dense7Pattern;
			break;
		case 2:
			return Qt::SolidPattern;
			break;
		case 3:
			return Qt::NoBrush;
			break;
		case 4:
			return Qt::VerPattern;
			break;
		case 5:
			return Qt::BDiagPattern;
			break;
		case 6:
			return Qt::FDiagPattern;
			break;
		case 7:
			return Qt::DiagCrossPattern;
			break;
		case 9:
			return Qt::Dense6Pattern;
			break;
		default:
			return Qt::NoBrush;
			break;
		}
	}

	static void PostCustomEvent(QObject* receiver, QEvent* event) {
		if (receiver && event)
			QCoreApplication::postEvent(receiver, event);
		else
			qDebug() << "Receiver or event is null! ";
	}

};


#endif //UTILS_H

