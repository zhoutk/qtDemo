#pragma once
#include "Idb.h"
#include "../Sqlit3Db.h"
#include <algorithm>

class DbBase
{
public:
	DbBase(QString connStr, QString dbType = "sqlit3") : connStr(connStr) {
		dbType.toLower();
		if (dbType.compare("sqlit3") == 0)
			db = new Sqlit3::Sqlit3Db(connStr);
		else {
			throw "Db Type error or not be supported. ";
		}
	};
	~DbBase() {
		if (db) {
			delete db;
		}
	};

	Qjson select(QString tablename, Qjson& params, QStringList fields = QStringList()) {
		return db->select(tablename, params, fields);
	};

	Qjson create(QString tablename, Qjson& params) {
		return db->create(tablename, params);
	};

	Qjson update(QString tablename, Qjson& params) {
		return db->update(tablename, params);
	};

	Qjson remove(QString tablename, Qjson& params) {
		return db->remove(tablename, params);
	};

    Qjson querySql(QString sql, Qjson params = Qjson(), QStringList filelds = QStringList()) {
		return db->querySql(sql, params, filelds);
	}

	Qjson execSql(QString sql) {
		return db->execSql(sql);
	}

	Qjson insertBatch(QString tablename, QVector<Qjson> elements, QString constraint = "id") {
		return db->insertBatch(tablename, elements, constraint);
	}

	Qjson transGo(QStringList sqls, bool isAsync = false) {
		return db->transGo(sqls);
	}

private:
	QString connStr;
	Idb * db;
};

