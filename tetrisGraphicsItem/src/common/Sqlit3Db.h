#pragma once
#include <assert.h>
#include "Qjson.h"
#include "Idb/Idb.h"
#include "sqlit3/sqlite3.h"
#include "utils.h"
#include "GlobalConstants.h"
#include <algorithm>
#include <QString>
#include <QStringList>
#include <memory>

using namespace std;

namespace Sqlit3 {

#define SQLITECPP_ASSERT(expression, message)   assert(expression && message)

	const int   OPEN_READONLY = SQLITE_OPEN_READONLY;
	const int   OPEN_READWRITE = SQLITE_OPEN_READWRITE;
	const int   OPEN_CREATE = SQLITE_OPEN_CREATE;
	const int   OPEN_URI = SQLITE_OPEN_URI;

	const int   OK = SQLITE_OK;

	

	class Sqlit3Db : public Idb
	{
	public:
		struct Deleter
		{
			void operator()(sqlite3* apSQLite) {
				const int ret = sqlite3_close(apSQLite);
				(void)ret;
				SQLITECPP_ASSERT(SQLITE_OK == ret, "database is locked");
			};
		};

	private:
		std::unique_ptr<sqlite3, Deleter> mSQLitePtr;
		QString mFilename;
		QStringList QUERY_EXTRA_KEYS;
		QStringList QUERY_UNEQ_OPERS;

	public:
		Sqlit3Db(const QString apFilename,
			const int   aFlags = OPEN_READWRITE | OPEN_CREATE,
			const int   aBusyTimeoutMs = 0,
			const QString* apVfs = nullptr) : mFilename(apFilename)
		{
			QUERY_EXTRA_KEYS = QString("ins,lks,ors").split(",");

			QUERY_UNEQ_OPERS.push_back(">,");
			QUERY_UNEQ_OPERS.push_back(">=,");
			QUERY_UNEQ_OPERS.push_back("<,");
			QUERY_UNEQ_OPERS.push_back("<=,");
			QUERY_UNEQ_OPERS.push_back("<>,");
			QUERY_UNEQ_OPERS.push_back("=,");

			sqlite3* handle;
			const int ret = sqlite3_open_v2(apFilename.toStdString().c_str(), &handle, aFlags, apVfs ? apVfs->toStdString().c_str() : nullptr);
			mSQLitePtr.reset(handle);
			if (SQLITE_OK != ret)
			{
				throw QString("DB Error, code: %1; message: %2").arg(ret).arg(sqlite3_errmsg(getHandle()));
			}
			if (aBusyTimeoutMs > 0)
			{
				const int ret = sqlite3_busy_timeout(getHandle(), aBusyTimeoutMs);
				if (OK != ret)
				{
					throw QString("DB Error, code: %1; message: %2").arg(ret).arg(sqlite3_errmsg(getHandle()));
				}
			}
		};

		Qjson remove(QString tablename, Qjson& params)
		{
			if (params.IsObject()) {
				QString execSql = "delete from ";
				execSql.append(tablename).append(" where id = ");

				QString v;
				int vType;
				params.GetValueAndTypeByKey("id", &v, &vType);

				if (vType == 6)
					execSql.append(v);
				else
					execSql.append("'").append(v).append("'");

				return ExecNoneQuerySql(execSql);
			}
			else {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
		}

		Qjson update(QString tablename, Qjson& params)
		{
			if (params.IsObject()) {
				QString execSql = "update ";
				execSql.append(tablename).append(" set ");

				QStringList allKeys = params.GetAllKeys();

				QStringList::iterator iter = find(allKeys.begin(), allKeys.end(), "id");
				if (iter == allKeys.end()) {
					return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
				}
				else {
					size_t len = allKeys.size();
					size_t conditionLen = len - 2;
					QString fields = "", where = " where id = ";
					for (size_t i = 0; i < len; i++) {
						QString key = allKeys[i];
						QString v;
						int vType;
						params.GetValueAndTypeByKey(key, &v, &vType);
						if (key.compare("id") == 0) {
							conditionLen++;
							if (vType == 6)
								where.append(v);
							else
								where.append("'").append(v).append("'");
						}
						else {
							fields.append(key).append(" = ");
							if (vType == 6)
								fields.append(v);
							else
								fields.append("'").append(v).append("'");
							if (i < conditionLen) {
								fields.append(",");
							}
						}
					}
					execSql.append(fields).append(where);
					return ExecNoneQuerySql(execSql);
				}
			}
			else {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
		}
		
		Qjson create(QString tablename, Qjson& params)
		{
			if (params.IsObject()) {
				QString execSql = "insert into ";
				execSql.append(tablename).append(" ");

				QStringList allKeys = params.GetAllKeys();
				size_t len = allKeys.size();
				QString fields = "", vs = "";
				for (size_t i = 0; i < len; i++) {
					QString key = allKeys[i];
					fields.append(key);
					QString v;
					int vType;
					params.GetValueAndTypeByKey(key, &v, &vType);
					if (vType == 2)
						vs.append(v);
					else
						vs.append("'").append(v).append("'");
					if (i < len - 1) {
						fields.append(",");
						vs.append(",");
					}
				}
				execSql.append("(").append(fields).append(") values (").append(vs).append(")");
				return ExecNoneQuerySql(execSql);
			}
			else {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
		}
		
		Qjson execSql(QString sql) {
			return ExecNoneQuerySql(sql);
		}
		
        Qjson querySql(QString sql, Qjson params = Qjson(), QStringList filelds = QStringList()) {
			return select(sql, params, filelds, 2);
		}

		Qjson insertBatch(QString tablename, QVector<Qjson> elements, QString constraint) {
			QString sql = "insert into ";
			if (elements.empty()) {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
			else {
				QString keyStr = " (";
				keyStr.append(elements[0].GetAllKeys().join(',')).append(" ) ");
				for (size_t i = 0; i < elements.size(); i++) {
					QStringList keys = elements[i].GetAllKeys();
					QString valueStr = " select ";
					for (size_t j = 0; j < keys.size(); j++) {
						valueStr.append("'").append(elements[i][keys[j]]).append("'");
						if (j < keys.size() - 1) {
							valueStr.append(",");
						}
					}
					if (i < elements.size() - 1) {
						valueStr.append(" union all ");
					}
					keyStr.append(valueStr);
				}
				sql.append(tablename).append(keyStr);
			}
			return ExecNoneQuerySql(sql);
		}

		Qjson transGo(QStringList sqls, bool isAsync = false) {
			if (sqls.empty()) {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
			else {
				char* zErrMsg = 0;
				bool isExecSuccess = true;
				sqlite3_exec(getHandle(), "begin;", 0, 0, &zErrMsg);
				for (size_t i = 0; i < sqls.size(); i++) {
					char * u8Query = Utils::UnicodeToU8(sqls[i]);
					int rc = sqlite3_exec(getHandle(), u8Query, 0, 0, &zErrMsg);
					if (rc != SQLITE_OK)
					{
						isExecSuccess = false;
						cout << "Transaction Fail, sql " << i + 1 << " is wrong. Error: " << zErrMsg << endl;
						sqlite3_free(zErrMsg);
						break;
					}
				}
				if (isExecSuccess)
				{
					sqlite3_exec(getHandle(), "commit;", 0, 0, 0);
					sqlite3_close(getHandle());
					cout << "Transaction Success: run " << sqls.size() << " sqls." << endl;
					return Utils::MakeJsonObjectForFuncReturn(STSUCCESS, "Transaction success.");
				}
				else
				{
					sqlite3_exec(getHandle(), "rollback;", 0, 0, 0);
					sqlite3_close(getHandle());
					return Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, zErrMsg);
				}
			}
		}
		
		Qjson select(QString tablename, Qjson& params, QStringList fields = QStringList(), int queryType = 1) {
			if (params.IsObject()) {
				QString querySql = "";
				QString where = "";
				const QString AndJoinStr = " and ";
				QString fieldsJoinStr = "*";

				if (!fields.empty()) {
					fieldsJoinStr = fields.join(',');
				}

				QString fuzzy = params.GetStringValueAndRemove("fuzzy");
				QString sort = params.GetStringValueAndRemove("sort");
				int page = params.GetStringValueAndRemove("page").toInt();
				int size = params.GetStringValueAndRemove("size").toInt();
				QString sum = params.GetStringValueAndRemove("sum");
				QString count = params.GetStringValueAndRemove("count");
				QString group = params.GetStringValueAndRemove("group");

				QStringList allKeys = params.GetAllKeys();
				size_t len = allKeys.size();
				for (size_t i = 0; i < len; i++) {
					QString k = allKeys[i];
					QString v;
					int vType;
					params.GetValueAndTypeByKey(k, &v, &vType);
					if (where.length() > 0) {
						where.append(AndJoinStr);
					}

					if (QUERY_EXTRA_KEYS.indexOf(k) > -1) {   // process key
						QString whereExtra = "";
						QStringList ele = params[k].split(',');
						if (ele.size() < 2 || ((k.compare("ors") == 0 || k.compare("lks") == 0) && ele.size() % 2 == 1)) {
							return Utils::MakeJsonObjectForFuncReturn(STPARAMERR, k + " is wrong.");
						}
						else {
							if (k.compare("ins") == 0) {
								QString c = ele.front();
								ele.pop_front();
								whereExtra.append(c).append(" in ( ").append(ele.join(',')).append(" )");
							}
							else if (k.compare("lks") == 0 || k.compare("ors") == 0) {
								whereExtra.append(" ( ");
								for (size_t j = 0; j < ele.size(); j += 2) {
									if (j > 0) {
										whereExtra.append(" or ");
									}
									whereExtra.append(ele.at(j)).append(" ");
									QString eqStr = k.compare("lks") == 0 ? " like '" : " = '";
									QString vsStr = ele.at(j + 1);
									if (k.compare("lks") == 0) {
										vsStr.insert(0, "%");
										vsStr.append("%");
									}
									vsStr.append("'");
									whereExtra.append(eqStr).append(vsStr);
								}
								whereExtra.append(" ) ");
							}
						}
						where.append(whereExtra);
					}
					else {				// process value
						if (QUERY_UNEQ_OPERS.indexOf(v) > -1) {
							QStringList vls = v.split(',');
							if (vls.size() == 2) {
								where.append(k).append(vls.at(0)).append("'").append(vls.at(1)).append("'");
							}
							else if (vls.size() == 4) {
								where.append(k).append(vls.at(0)).append("'").append(vls.at(1)).append("' and ");
								where.append(k).append(vls.at(2)).append("'").append(vls.at(3)).append("'");
							}
							else {
								return Utils::MakeJsonObjectForFuncReturn(STPARAMERR, "not equal value is wrong.");
							}
						}
						else if (!fuzzy.isEmpty() && vType == QJsonValue::String) {
							where.append(k).append(" like '%").append(v).append("%'");
						}
						else {
							if (vType == QJsonValue::Double)
								where.append(k).append(" = ").append(v);
							else
								where.append(k).append(" = '").append(v).append("'");
						}
					}
				}

				QString extra = "";
				if (!sum.isEmpty()) {
					QStringList ele = sum.split(',');
					if (ele.empty() || ele.size() % 2 == 1)
						return Utils::MakeJsonObjectForFuncReturn(STPARAMERR, "sum is wrong.");
					else {
						for (size_t i = 0; i < ele.size(); i += 2) {
							extra.append(",sum(").append(ele.at(i)).append(") as ").append(ele.at(i + 1)).append(" ");
						}
					}
				}
				if (!count.isEmpty()) {
					QStringList ele = count.split(',');
					if (ele.empty() || ele.size() % 2 == 1)
						return Utils::MakeJsonObjectForFuncReturn(STPARAMERR, "count is wrong.");
					else {
						for (size_t i = 0; i < ele.size(); i += 2) {
							extra.append(",count(").append(ele.at(i)).append(") as ").append(ele.at(i + 1)).append(" ");
						}
					}
				}

				if (queryType == 1) {
					querySql.append("select ").append(fieldsJoinStr).append(extra).append(" from ").append(tablename);
					if (where.length() > 0)
						querySql.append(" where ").append(where);
				}
				else {
					querySql.append(tablename);
					if (!fields.empty()) {
						int starIndex = querySql.indexOf('*');
						if (starIndex < 10) {
							querySql.replace(starIndex, 1, fieldsJoinStr);
						}
					}
					if (where.length() > 0) {
						int whereIndex = querySql.indexOf("where");
						if (whereIndex < 0) {
							querySql.append(" where ").append(where);
						}
						else {
							querySql.append(" and ").append(where);
						}
					}
				}

				if (!group.isEmpty()) {
					querySql.append(" group by ").append(group);
				}

				if (!sort.isEmpty()) {
					querySql.append(" order by ").append(sort);
				}

				if (page > 0) {
					page--;
					querySql.append(" limit ").append(QString::number(page * size)).append(",").append(QString::number(size));
				}

				return ExecQuerySql(querySql, fields);
			}
			else {
				return Utils::MakeJsonObjectForFuncReturn(STPARAMERR);
			}
		};
		
		sqlite3* getHandle()
		{
			return mSQLitePtr.get();
		}

	private:
		Qjson ExecQuerySql(QString aQuery, QStringList fields) {
			Qjson rs = Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
			sqlite3_stmt* stmt = NULL;
			sqlite3* handle = getHandle();
			char * u8Query = Utils::UnicodeToU8(aQuery);
			const int ret = sqlite3_prepare_v2(handle, u8Query, strlen(u8Query), &stmt, NULL);
			if (SQLITE_OK != ret)
			{
				QString errmsg = sqlite3_errmsg(getHandle());
				rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, errmsg));
			}
			else {
				int insertPot = aQuery.indexOf("where");
				if (insertPot < 0) {
					insertPot = aQuery.indexOf("limit");
					if (insertPot < 0) {
						insertPot = aQuery.length();
					}
				}
				QString aQueryLimit0 = aQuery.mid(0, insertPot).append(" limit 1");
				char** pRes = NULL;
				int nRow = 0, nCol = 0;
				char* pErr = NULL;
				sqlite3_get_table(handle, aQueryLimit0.toStdString().c_str(), &pRes, &nRow, &nCol, &pErr);
				for (int j = 0; j < nCol; j++)
				{
					QString fs = *(pRes + j);
					if (find(fields.begin(), fields.end(), fs) == fields.end()) {
						fields.push_back(fs);
					}
				}
				if (pErr != NULL)
				{
					sqlite3_free(pErr);
				}
				sqlite3_free_table(pRes);

				QVector<Qjson> arr;
				while (sqlite3_step(stmt) == SQLITE_ROW) {
					Qjson al;
					for (int j = 0; j < nCol; j++)
					{
						QString k = fields.at(j);
						int nType = sqlite3_column_type(stmt, j);
						if (nType == 1) {					//SQLITE_INTEGER
							al.AddValueBase(k, sqlite3_column_int64(stmt, j));
						}
						else if (nType == 2) {				//SQLITE_FLOAT
							al.AddValueBase(k, sqlite3_column_double(stmt, j));
						}
						else if (nType == 3) {				//SQLITE_TEXT
							al.AddValueBase(k, Utils::U8ToUnicode((char*)sqlite3_column_text(stmt, j)));
						}
						//else if (nType == 4) {				//SQLITE_BLOB

						//}
						//else if (nType == 5) {				//SQLITE_NULL

						//}
						else{
							al.AddValueBase(k, "");
						}
					}
					arr.push_back(al);
				}
				if (arr.empty())
					rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STQUERYEMPTY));
				rs.AddValueObjectsArray("data", arr);
			}
			sqlite3_finalize(stmt);
			qDebug() << "SQL: " << aQuery << endl;
			return rs;
		}

		Qjson ExecNoneQuerySql(QString aQuery) {
			Qjson rs = Utils::MakeJsonObjectForFuncReturn(STSUCCESS);
			sqlite3_stmt* stmt = NULL;
			sqlite3* handle = getHandle();
			char * u8Query = Utils::UnicodeToU8(aQuery);
			const int ret = sqlite3_prepare_v2(handle, u8Query, strlen(u8Query), &stmt, NULL);
			if (SQLITE_OK != ret)
			{
				QString errmsg = sqlite3_errmsg(getHandle());
				rs.ExtendObject(Utils::MakeJsonObjectForFuncReturn(STDBOPERATEERR, errmsg));
			}
			else {
				sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			qDebug() << "SQL: " << aQuery << endl;
			return rs;
		}
		
	};
}
