#ifndef RJSON_H
#define RJSON_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QDebug>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

class Rjson {
private:
	QJsonObject* json;
	bool _IsObject_;

public:
	Rjson() {
		json = new QJsonObject();
		_IsObject_ = true;
	}

	Rjson(const char* jstr) {
		QJsonParseError json_error;
		QJsonDocument jsonDocument = QJsonDocument::fromJson(QString::fromLocal8Bit(jstr).toUtf8(), &json_error);
		if (json_error.error == QJsonParseError::NoError) {
			_IsObject_ = true;
			json = new QJsonObject(jsonDocument.object());
		}
		else {
			_IsObject_ = false;
			json = nullptr;
		}
	}

	Rjson(string jstr) {
		new (this)Rjson(jstr.c_str());
	}

	Rjson(QString jstr) {
		new (this)Rjson(jstr.toLocal8Bit().toStdString());
	}

	Rjson(QByteArray& data) {
		QJsonParseError json_error;
		QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &json_error);
		if (json_error.error == QJsonParseError::NoError) {
			_IsObject_ = true;
			json = new QJsonObject(jsonDocument.object());
		}
		else {
			_IsObject_ = false;
			json = nullptr;
		}
	}

	Rjson(const Rjson& origin) {
		new (this)Rjson(QString(QJsonDocument(*(origin.json)).toJson(QJsonDocument::Compact)));
	}

	string operator[](string key) {
		string rs = "";
		if (json->contains(key.c_str())) {
			int vType;
			GetValueAndTypeByKey(key.c_str(), &rs, &vType);
		}
		return rs;
	}

	Rjson& operator = (const Rjson& origin) {
		new (this)Rjson(origin);
		return(*this);
	}

	bool HasMember(string key) {
		return json->contains(key.c_str());
	}

	string GetStringValueAndRemove(string key) {
		string rs = (*this)[key];
		if (HasMember(key)) {
			json->remove(key.c_str());
		}
		return rs;
	}

	vector<QString> GetQStringArrayByKey(string k) {
		vector<QString> rs;
		if (json->contains(k.c_str()) && (*json)[k.c_str()].isArray()) {
			QJsonArray v = (*json)[k.c_str()].toArray();
			size_t len = v.size();
			for (size_t i = 0; i < len; i++) {
				rs.push_back(v[i].toString());
			}
		}
		return rs;
	}

	vector<string> GetStringArrayByKey(string k) {
		vector<string> rs;
		if (json->contains(k.c_str()) && (*json)[k.c_str()].isArray()) {
			QJsonArray v = (*json)[k.c_str()].toArray();
			size_t len = v.size();
			for (size_t i = 0; i < len; i++) {
				rs.push_back(v[i].toString().toStdString());
			}
		}
		return rs;
	}

	vector<Rjson> GetObjectsArrayByKey(string k) {
		vector<Rjson> rs;
		if (json->contains(k.c_str()) && (*json)[k.c_str()].isArray()) {
			QJsonArray v = (*json)[k.c_str()].toArray();
			size_t len = v.size();
			for (size_t i = 0; i < len; i++) {
				rs.push_back(Rjson(QString(QJsonDocument(v[i].toObject()).toJson(QJsonDocument::Compact))));
			}
		}
		return rs;
	}

	Rjson ExtendObject(Rjson& obj) {
		QJsonObject* src = obj.GetOriginRapidJson();
		for (auto iter = src->begin(); iter != src->end(); ++iter)
		{
			if (json->contains(iter.key())) {
				json->remove(iter.key());
			}
			json->insert(iter.key(), iter.value());
		}
		return *(this);
	}

	void AddValueLong(string k, long long v) {
		json->insert(k.c_str(), v);
	}

	void AddValueInt(string k, int v) {
		json->insert(k.c_str(), v);
	}

	void AddValueFloat(string k, double v) {
		json->insert(k.c_str(), v);
	}

	void AddValueString(string k, string v) {
		json->insert(k.c_str(), QString::fromLocal8Bit(v.c_str()));
	}

	void AddValueObject(string k, Rjson v) {
		QJsonObject* al = v.GetOriginRapidJson();
		QJsonObject aObj;
		for (auto iter = al->begin(); iter != al->end(); ++iter)
		{
			aObj.insert(iter.key(), iter.value());
		}
		json->insert(k.c_str(), aObj);
	}

	void AddValueArray(string k, vector<string>& arr) {
		QJsonArray rows;
		int len = arr.size();
		for (int i = 0; i < len; i++) {
			rows.push_back(QJsonValue(arr[i].c_str()));
		}
		json->insert(k.c_str(), rows);
	}

	void AddValueQStringArray(string k, vector<QString>& arr) {
		int len = arr.size();
		QJsonArray rows;
		for (int i = 0; i < len; i++) {
			rows.push_back(arr.at(i));
		}
		json->insert(k.c_str(), rows);
	}

	void AddValueObjectsArray(string k, vector<Rjson>& arr) {
		int len = arr.size();
		QJsonArray rows;
		for (int i = 0; i < len; i++) {
			QJsonObject arow;
			QJsonObject* al = arr.at(i).GetOriginRapidJson();
			for (auto iter = al->begin(); iter != al->end(); ++iter)
			{
				arow.insert(iter.key(), iter.value());
			}
			rows.push_back(arow);
		}

		json->insert(k.c_str(), rows);
	}

	QString GetJsonQString() {
		return QString(QJsonDocument(*json).toJson(QJsonDocument::Compact));
	}

	string GetJsonString() {
		return QString(QJsonDocument(*json).toJson(QJsonDocument::Compact)).toLocal8Bit().toStdString();
	}

	QByteArray GetJsonQByteArray() {
		return QJsonDocument(*json).toJson(QJsonDocument::Compact);
	}

	void GetValueAndTypeByKey(string key, string* v, int* vType) {
		QJsonObject::iterator iter = json->find(key.c_str());
		if (iter != json->end()) {
			*vType = (int)(iter->type());
			if (iter->isNull()) {
				*v = "null";
			}
			else if (iter->isBool()) {
				*v = QString("%1").arg(iter->toBool()).toStdString();
			}
			else if (iter->isDouble()) {
				QString tmp = QString::number(iter->toDouble(), 'f', 6); //QString("%1").arg(iter->toDouble()).toStdString();
				QStringList tmps = tmp.split('.');
				if (tmps.size() == 2 && tmps[1].toInt() > 0) {
					*v = tmp.toStdString();
				}
				else {
					*v = tmps[0].toStdString();
				}
			}
			else if (iter->isString()) {
				*v = iter->toString().toLocal8Bit().toStdString();
			}
			else if (iter->isArray()) {
				*v =  (QJsonDocument(iter->toArray()).toJson(QJsonDocument::Compact)).data();
			}
			else if (iter->isObject()) {
				*v =  (QJsonDocument(iter->toObject()).toJson(QJsonDocument::Compact)).data();
			}
			else if (iter->isUndefined()) {
				*v = "undefined";
			}
			else {
				*v = "";
			}
		}
		else {
			*vType = QJsonValue::String;
			*v = "";
		}
	}

	vector<string> GetAllKeys() {
		vector<string> keys;
		for (auto iter = json->begin(); iter != json->end(); ++iter)
		{
			keys.push_back(iter.key().toStdString());
		}
		//std::reverse(keys.begin(), keys.end());
		return keys;
	}

	bool IsObject() {
		return _IsObject_;
	}

	~Rjson() {
		if (json) {
			delete json;
			json = nullptr;
		}
	}

private:
	QJsonObject* GetOriginRapidJson() {
		return json;
	}

};

#endif //RJSON_H