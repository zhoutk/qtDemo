#pragma once

#include <map>
#include<utility>

using namespace std;

enum StatusCodes {
	STSUCCESS = 200,              //�����ɹ�
	STQUERYEMPTY = 202,           //���޽��
	STPARAMERR = 301,             //�����������
	STNOTFOUNDERR = 404,          //��ȡ��Դ������
	STUPLOADERR = 411,            //�ļ��ϴ�ʧ��
	STJWTAUTHERR = 421,           //��Ȩ�����ʧЧ
	STPASSWORDERR = 422,          //�������
	STUSERNAMEERR = 423,          //�û�������򲻴���
	STAUTHORIZATIONERR = 431,     //Ȩ�޲���
	STUSERNOTFOUNDERR = 432,      //�û�������
	STEXCEPTIONERR = 500,         //�����쳣
	STDBCONNECTERR = 700,         //���ݿ�����ʧ��
	STDBOPERATEERR = 701,         //���ݿ����ʧ��
	STDBNEEDIDERR = 702,          //���ݿ��������ID�ֶ�
	STDBNEEDRESTARTERR = 703,     //���ݿ���޸ģ���Ҫ��������
	STPARENTNOTFOUNDERR = 801,    //����¼������
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
