#pragma once
#include "../Qjson.h"
#include <QStringList>

class Idb
{
public:
	virtual Qjson select(QString tablename, Qjson& params, QStringList fields = QStringList(), int queryType = 1) = 0;
	virtual Qjson create(QString tablename, Qjson& params) = 0;
	virtual Qjson update(QString tablename, Qjson& params) = 0;
	virtual Qjson remove(QString tablename, Qjson& params) = 0;
    virtual Qjson querySql(QString sql, Qjson params = Qjson(), QStringList filelds = QStringList()) = 0;
	virtual Qjson execSql(QString sql) = 0;
	virtual Qjson insertBatch(QString tablename, QVector<Qjson> elements, QString constraint = "id") = 0;
	virtual Qjson transGo(QStringList sqls, bool isAsync = false) = 0;
};

