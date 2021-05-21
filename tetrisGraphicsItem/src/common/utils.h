#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QCoreApplication>
#include "GlobalConstants.h"

class Utils
{
public:
	Utils() {};

	static bool FindStartsStringFromVector(vector<string> strs, string value) {
		bool rs = false;
		size_t len = strs.size();
		for (size_t i = 0; i < len; i++) {
			size_t vlen = value.size();
			string key = strs[i];
			size_t klen = key.size();
			if (vlen <= klen)
				continue;
			else {
				if (value.substr(0, klen).compare(key) == 0)
				{
					rs = true;
					break;
				}
			}
		}
		return rs;
	}

	static bool FindStartsCharArray(char** strings, char* value) {
		char* string;
		char* parValue;
		while ((string = *strings++) != NULL)
		{
			parValue = value;
			while (*string != '\0' || *parValue != '\0')
			{
				if (*string == *parValue++)
				{
					string++;
					continue;
				}
				else if (*string == '\0') {
					return true;
				}
				else {
					break;
				}
				parValue++;
			}
		}
		return false;
	}

	static bool FindStringFromVector(vector<string> strs, string value) {
		auto iter = std::find(strs.begin(), strs.end(), value);
		if (iter == strs.end()) {
			return false;
		}
		else {
			return true;
		}
	}

	static bool FindCharArray(char** strings, char* value) {
		char* string;
		char* parValue;
		while ((string = *strings++) != NULL)
		{
			parValue = value;
			while (*string != '\0' && *parValue != '\0')
			{
				if (*string++ == *parValue++)
				{
					if ((*parValue == '\0'))
						return true;
				}
				else {
					break;
				}
			}
		}
		return false;
	}

	static vector<string> MakeVectorInitFromString(string str, char flag = ',') {
		vector<string> rs;
		istringstream iss(str);
		string temp;

		while (getline(iss, temp, flag)) {
			rs.push_back(temp);
		}
		return rs;
	}

//	static string GetLocalNowTime() {
//		time_t timep;
//		time(&timep);
//		char tmp[64];
//		struct tm nowTime;
//		localtime_s(&nowTime, &timep);
//		strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", &nowTime);
//		return std::string(tmp);
//	}

	static string IntTransToString(int val) {
		std::stringstream ss;
		ss << val;
		return ss.str();
	}

	static string GetVectorJoinStr(vector<string> v) {
		std::stringstream ss;
		for (size_t i = 0; i < v.size(); ++i)
		{
			if (i != 0)
				ss << ",";
			ss << v[i];
		}
		return ss.str();
	}

	static Qjson MakeJsonObjectForFuncReturn(StatusCodes code, QString info = "") {
		Qjson rs;
		rs.AddValueBase("code", (int)code);
		if (!info.isEmpty()) {
			info.insert(0, "\r\ndetails, ");
		}
		info.insert(0, QString::fromStdString(STCODEMESSAGES[(int)code]));
		rs.AddValueBase("msg", info);
		return rs;
	}

	static char* U8ToUnicode(char* szU8)
	{
		QString sz = QString::fromUtf8(szU8);
		QByteArray tt = sz.toLocal8Bit();
		int len = tt.size();
		char* dd = new char[len + 1];
		memset(dd, 0, len + 1);
		memcpy(dd, tt.data(), len);
		return dd;
	}

	static char* UnicodeToU8(QString str)
	{
		string tt = str.toStdString();
		int len = tt.size();
		char* dd = new char[len + 1];
		memset(dd, 0, len + 1);
		memcpy(dd, tt.c_str(), len);
		return dd;
	}


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
			return Qt::magenta;
			break;
		case 2:
			return Qt::darkMagenta;
			break;
		case 3:
			return Qt::gray;
			break;
		case 4:
			return Qt::darkGreen;
			break;
		case 5:
			return Qt::darkCyan;
			break;
		case 6:
			return Qt::darkBlue;
			break;
		case 9:
			return Qt::blue;
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

