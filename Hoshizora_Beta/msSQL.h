#pragma once
#include "common.h"
#include <tchar.h>
#include <vector>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>
#include <string>
#include <map>
#include <algorithm>
#include <ppl.h>

using namespace std;

extern DWORD time_magic;

class CmsSQL
{
private:
	SQLHANDLE sqlenvhandle;    
	SQLHANDLE sqlconnectionhandle;
	Concurrency::critical_section cs;
	string GetCurrentMonthString(void);
public:
	CmsSQL(void);
	~CmsSQL(void);
	vector<user_info> GetUserInfo(const CHAR * conditionString);
	vector<vector<DWORD>> GetUserPayedInfo(vector<user_info>& usrInfo);
	int MarkPayInfo(vector<user_info> & info);
	bool UpdateUserMac(size_t idx, const char * phone, const char * mac);
	bool UpdateUserRoom(size_t idx, const char * room);
	int GetDeviceData(vector<pair<string,string>> & v,const char *prefix);
	bool CheckTimeLegal(time_t tm0);
	size_t GetNewPayedUser(vector<pair<string,MacAddress>> & v,size_t LastLineID,const char* schemeDescr = NULL);
	size_t GetLastPayedLineID();
};

