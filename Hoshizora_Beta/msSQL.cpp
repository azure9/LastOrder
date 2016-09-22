#include "msSQL.h"
#include "stdafx.h"

#ifdef Business
#pragma optimize( "g", off )
#endif // Business
CmsSQL::CmsSQL(void)
	:sqlenvhandle(NULL),sqlconnectionhandle(NULL)
{
#ifdef Business
	VMProtectBeginVirtualizationLockByKey("CmsSQL::CmsSQL");
	int status=0;
	status = VMProtectGetSerialNumberState();
	if (status)
	{
		MessageBox(NULL,_T("请不要耍小聪明！"),_T("LastOrder授权系统"),MB_ICONERROR);
		ExitProcess(-1);
	}
#endif // Business
	if(SQL_SUCCESS!=SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlenvhandle))
		throw -1;

	if(SQLSetEnvAttr(sqlenvhandle,SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)||
		SQLAllocHandle(SQL_HANDLE_DBC, sqlenvhandle, &sqlconnectionhandle)) 
	{
		SQLFreeHandle(SQL_HANDLE_DBC,sqlenvhandle);
		throw -1;
	}

	switch(SQLDriverConnect (sqlconnectionhandle, 
		NULL, 
		(SQLTCHAR*)_T(""), //nothing here haha ~
		SQL_NTS, 
		NULL, 
		0, 
		NULL,
		SQL_DRIVER_NOPROMPT)){
	case SQL_SUCCESS_WITH_INFO:
		break;
	case SQL_INVALID_HANDLE:
	case SQL_ERROR:
		SQLFreeHandle(SQL_HANDLE_DBC, sqlconnectionhandle);
		SQLFreeHandle(SQL_HANDLE_ENV, sqlenvhandle);
		throw -1;
		break;
	default:
		break;
	}
#ifdef Business
	VMProtectEnd();
#endif // Business
}
#ifdef Business
#pragma optimize( "g", on )
#endif // Business


CmsSQL::~CmsSQL(void)
{
	SQLDisconnect(sqlconnectionhandle);
	SQLFreeHandle(SQL_HANDLE_DBC, sqlconnectionhandle);
	SQLFreeHandle(SQL_HANDLE_ENV, sqlenvhandle);
}


vector<user_info> CmsSQL::GetUserInfo(const CHAR * conditionString)
{
	basic_string<CHAR> && queryStr = stringf(("select * from 用户基本信息 where %s"),conditionString);
	vector<user_info> v;
	SQLHANDLE sqlstatementhandle;
	if(SQL_SUCCESS==SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
	{
		if (!SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)queryStr.c_str(),SQL_NTS))
		{
			while(SQLFetch(sqlstatementhandle)==SQL_SUCCESS)
			{
				SQLINTEGER idx;
				char name[50] = {0};
				char id[50] = {0};
				char phone[50] = {0};
				char mac[50] = {0};
				char local[50] = {0};
				SQLGetData(sqlstatementhandle,1,SQL_INTEGER,&idx,sizeof(SQLINTEGER),NULL);
				SQLGetData(sqlstatementhandle,2, SQL_C_CHAR, id, 50, NULL);//card id
				SQLGetData(sqlstatementhandle,3, SQL_C_CHAR, name, 50, NULL);//name
				SQLGetData(sqlstatementhandle,8, SQL_C_CHAR, phone, 50, NULL);//phone
				SQLGetData(sqlstatementhandle,10, SQL_C_CHAR, local, 50, NULL);//location
				SQLGetData(sqlstatementhandle,19, SQL_C_CHAR, mac, 50, NULL);//mac
				v.push_back(user_info(id,name,phone,local,mac,RegedUser,idx));
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
	}
	return move(v);
}

vector<vector<DWORD>> CmsSQL::GetUserPayedInfo(vector<user_info>& usrInfo)
{
	vector<vector<DWORD>> result;
	for (size_t index = 0;index<usrInfo.size();++index)
	{
		if (usrInfo[index].userid.empty())
		{
			result.push_back(vector<DWORD>());
		}
		else
		{
			basic_string<CHAR> && queryStr = stringf(("select 包月 from 学生交费 where 用户名='%s'"),usrInfo[index].userid.c_str());
			vector<DWORD> v;
			SQLHANDLE sqlstatementhandle;
			if(SQL_SUCCESS==SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
			{
				if (!SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)queryStr.c_str(),SQL_NTS))
				{
					const size_t cbSize = 10;
					SQL_DATE_STRUCT date[cbSize];
					SQLSetStmtAttr(sqlstatementhandle, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)cbSize, SQL_IS_INTEGER);
					SQLBindCol(sqlstatementhandle,1, SQL_C_DATE, date, sizeof(date), NULL);//card id
					while (memset(date,0,cbSize*sizeof(SQL_DATE_STRUCT)),
						SQLFetch(sqlstatementhandle)==SQL_SUCCESS)
					{
						for (size_t i=0;i<cbSize;++i)
						{
							if (date[i].year)
							{
								v.push_back(date[i].year<<8|date[i].month);
							}
							else
								break;
						}
					}
				}
				SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
			}
			result.push_back(move(v));
		}
	}
	
	return move(result);
}


int CmsSQL::MarkPayInfo(vector<user_info> & info)
{
	for (size_t index = 0;index<info.size();++index)
	{
		if (info[index].userid.empty())
		{
		}
		else
		{
			basic_string<CHAR> && queryStr = 
				stringf(("select 包月 from 学生交费 where 用户名='%s' and 包月='%s'"),
				info[index].userid.c_str(),GetCurrentMonthString().c_str());
			vector<DWORD> v;
			SQLHANDLE sqlstatementhandle;
			if(SQL_SUCCESS==SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
			{
				if (!SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)queryStr.c_str(),SQL_NTS))
				{
					if (SQLFetch(sqlstatementhandle)==SQL_SUCCESS)
					{
						info[index].flag|=Payed;
					}
				}
				SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
			}
		}
	}
	return 0;
}



bool CmsSQL::UpdateUserMac(size_t idx, const char * phone, const char * mac)
{
	SQLHANDLE sqlstatementhandle;
	bool retVal = false;
	if(SQL_SUCCESS==SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
	{
		if (!SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)
			stringf("usp_User_Update2 %d,%s,'%s'",idx,phone,mac).c_str(),SQL_NTS))
		{
			retVal = true;
		}
		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
	}
	return retVal;
}


bool CmsSQL::UpdateUserRoom(size_t idx, const char * room)
{
	SQLHANDLE sqlstatementhandle;
	bool retVal = false;
	if(SQL_SUCCESS==SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
	{
		if (!SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)
			stringf("usp_User_Update3 %d,'%s'",idx,room).c_str(),SQL_NTS))
		{
			retVal = true;
		}
		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
	}
	return retVal;
}

int CmsSQL::GetDeviceData(vector<pair<string,string>> & v,const char *prefix)
{
	SQLHANDLE sqlstatementhandle;
	bool retVal = false;
	if(SQL_SUCCESS==SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
	{
		if (!SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)
			stringf("select * from 交换机端口 where 房间号 LIKE '%s%%' order by 房间号",prefix).c_str(),SQL_NTS))
		{
			char port[64];
			char room[64];
			while(SQLFetch(sqlstatementhandle)==SQL_SUCCESS)
			{
				SQLGetData(sqlstatementhandle,2, SQL_C_CHAR, room, 64, NULL);
				SQLGetData(sqlstatementhandle, 4, SQL_C_CHAR, port, 64, NULL);
				v.push_back(pair<string,string>(room,port));
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
	}
	return retVal;
}

bool CmsSQL::CheckTimeLegal(time_t tm0)
{
	SQLHANDLE sqlstatementhandle;
	bool retVal = false;
	if(SQL_SUCCESS==SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
	{
		if (!SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)"select GETDATE()",SQL_NTS))
		{
			if (SQLFetch(sqlstatementhandle)==SQL_SUCCESS)
			{
				tm* tm1 = localtime(&tm0);
				SQL_DATE_STRUCT date;
				SQLGetData(sqlstatementhandle,1,SQL_DATE,&date,sizeof(SQL_DATE_STRUCT),NULL);
				if (labs(date.day-tm1->tm_mday)<2&&tm1->tm_mon+1==date.month&&tm1->tm_year+1900==date.year)
				{
					retVal = true;
				}
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
	}
	return retVal;
}

size_t CmsSQL::GetNewPayedUser(vector<pair<string,MacAddress>> & v,size_t LastLineID,const char* schemeDescr)
{
	SQLHANDLE sqlstatementhandle;
	if(SQL_SUCCESS==SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
	{
		if (!SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)
			stringf(
			"select 接入地点,MAC地址 from 用户基本信息 where 用户名 in (select 用户名 from 学生交费 where (Id>'%d' and 包月='%s'))%s",
			LastLineID,GetCurrentMonthString().c_str(),
			schemeDescr?stringf(" and 接入地点 LIKE '%s%%'",schemeDescr).c_str():""
			).c_str(),SQL_NTS))
		{
			char room[64];
			char mac[64];
			while(SQLFetch(sqlstatementhandle)==SQL_SUCCESS)
			{
				memset(mac,0,64);
				SQLGetData(sqlstatementhandle,1, SQL_C_CHAR, room, 64, NULL);
				SQLGetData(sqlstatementhandle,2, SQL_C_CHAR, mac, 64, NULL);
				v.push_back(pair<string,MacAddress>(room,mac));
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
	}

	return GetLastPayedLineID();
}

string CmsSQL::GetCurrentMonthString()
{
	return stringf("%04d-%02d-01 00:00:00",time_magic>>8,time_magic&0xFF);
}

size_t CmsSQL::GetLastPayedLineID()
{
	size_t LastLineID;
	SQLHANDLE sqlstatementhandle;
	if(SQL_SUCCESS==SQLAllocHandle(SQL_HANDLE_STMT, sqlconnectionhandle, &sqlstatementhandle))
	{
		if (!SQLExecDirectA(sqlstatementhandle, (SQLCHAR*)"select max(Id) from 学生交费",SQL_NTS))
		{
			if (SQLFetch(sqlstatementhandle)==SQL_SUCCESS)
			{
				SQLGetData(sqlstatementhandle,1,SQL_INTEGER,&LastLineID,sizeof(SQL_INTEGER),NULL);
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, sqlstatementhandle);
	}
	return LastLineID;
}