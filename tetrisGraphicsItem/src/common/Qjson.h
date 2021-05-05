#ifndef Qjson_H
#define Qjson_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QDebug>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

class Qjson {
private:
	QJsonObject* json;
	bool _IsObject_;

public:
	Qjson() {
		json = new QJsonObject();
		_IsObject_ = true;
	}

	Qjson(QByteArray& data) {
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

	Qjson(QString jstr) {
		new (this)Qjson(jstr.toUtf8());
	}

	Qjson(const Qjson& origin) {
		new (this)Qjson(QJsonDocument(*(origin.json)).toJson(QJsonDocument::Compact));
	}

	QString operator[](QString key) {
		QString rs = "";
		if (json->contains(key)) {
			int vType;
			GetValueAndTypeByKey(key, &rs, &vType);
		}
		return rs;
	}

	Qjson& operator = (const Qjson& origin) {
		new (this)Qjson(origin);
		return(*this);
	}

	bool HasMember(QString key) {
		return json->contains(key);
	}

	QString GetStringValueAndRemove(QString key) {
		QString rs = (*this)[key];
		if (HasMember(key)) {
			json->remove(key);
		}
		return rs;
	}

	vector<QString> GetStringArrayByKey(QString k) {
		vector<QString> rs;
		if (json->contains(k) && (*json)[k].isArray()) {
			QJsonArray v = (*json)[k].toArray();
			size_t len = v.size();
			for (size_t i = 0; i < len; i++) {
				rs.push_back(v[i].toString());
			}
		}
		return rs;
	}

	vector<Qjson> GetObjectsArrayByKey(QString k) {
		vector<Qjson> rs;
		if (json->contains(k) && (*json)[k].isArray()) {
			QJsonArray v = (*json)[k].toArray();
			size_t len = v.size();
			for (size_t i = 0; i < len; i++) {
				rs.push_back(Qjson(QJsonDocument(v[i].toObject()).toJson(QJsonDocument::Compact)));
			}
		}
		return rs;
	}

	Qjson ExtendObject(Qjson& obj) {
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

	template<typename T> void AddValueBase(QString k, T v)
	{
		json->insert(k, v);
	}

	/*void AddValueObject(string k, Qjson v) {
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

	void AddValueObjectsArray(string k, vector<Qjson>& arr) {
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
	}*/

	QString GetJsonString() {
		return QString(QJsonDocument(*json).toJson(QJsonDocument::Compact));
	}

	QByteArray GetJsonQByteArray() {
		return QJsonDocument(*json).toJson(QJsonDocument::Compact);
	}

	void GetValueAndTypeByKey(QString key, QString* v, int* vType) {
		QJsonObject::iterator iter = json->find(key);
		if (iter != json->end()) {
			*vType = (int)(iter->type());
            *v = iter->toString();
		}
	}

	vector<QString> GetAllKeys() {
		vector<QString> keys;
		for (auto iter = json->begin(); iter != json->end(); ++iter)
		{
			keys.push_back(iter.key());
		}
		return keys;
	}

	bool IsObject() {
		return _IsObject_;
	}

	~Qjson() {
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

#endif //Qjson_H
