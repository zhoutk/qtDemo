#pragma once

#include <map>
#include<utility>

using namespace std;

enum StatusCodes {
	STSUCCESS = 200,              //操作成功
	STQUERYEMPTY = 202,           //查无结果
	STPARAMERR = 301,             //输入参数错误
	STNOTFOUNDERR = 404,          //获取资源不存在
	STUPLOADERR = 411,            //文件上传失败
	STJWTAUTHERR = 421,           //授权错误或失效
	STPASSWORDERR = 422,          //密码错误
	STUSERNAMEERR = 423,          //用户名错误或不存在
	STAUTHORIZATIONERR = 431,     //权限不够
	STUSERNOTFOUNDERR = 432,      //用户不存在
	STEXCEPTIONERR = 500,         //发生异常
	STDBCONNECTERR = 700,         //数据库连接失败
	STDBOPERATEERR = 701,         //数据库操作失败
	STDBNEEDIDERR = 702,          //数据库表必须包含ID字段
	STDBNEEDRESTARTERR = 703,     //数据库表修改，需要服务重启
	STPARENTNOTFOUNDERR = 801,    //父记录不存在
};

const pair<int, string> pairs[] = {
	make_pair(200, "Operation succeeded. "),
	make_pair(202, "Query result is empty. "),
	make_pair(301, "Error: Param is wrong. "),
	make_pair(404, "Error: Request resource is not found. "),
	make_pair(411, "Error: Upload file fail. "),
	make_pair(421, "Error: Json web token authorize fail. "),
	make_pair(422, "Error: Password is wrong. "),
	make_pair(423, "Error: Username is wrong. "),
	make_pair(431, "Error: Authorization is less. "),
	make_pair(432, "Error: User is not found. "),
	make_pair(500, "Error: Exception is thrown. "),
	make_pair(700, "Error: Database connection is wrong. "),
	make_pair(701, "Error: Database operation is wrong. "),
	make_pair(702, "Error: Database table must have id field. "),
	make_pair(703, "Error: Database modify & serve need resart. "),
	make_pair(801, "Error: Parent record is not found. "),
};

static map<int, string> STCODEMESSAGES(pairs, pairs + sizeof(pairs)/sizeof(pairs[0]));
