#pragma once
#include "mssql.h"


typedef enum _QueryType
{
	ROOM_INFO,
	CUSTOM,
	SEARCH,
	NO_PAYED_DATA = 0x80000000
}QueryType;

extern DWORD time_magic;

class CUserInfo :
	protected CmsSQL
{
public:
	CUserInfo(void);
	~CUserInfo(void);
	vector<user_info> GetUserInfo(const char * conditionString,QueryType type = ROOM_INFO);
	vector<user_info> GetUserInfo(const char * conditionString,vector<vector<DWORD>>& pay_info,QueryType type = ROOM_INFO);
	int FillUserInfo(vector<user_info> & v, vector<vector<DWORD>>& pay_info, QueryType type = static_cast<QueryType>(NULL));
	size_t MarkPayInfo(vector<user_info> & v, vector<vector<DWORD>> & pay_info);
};

