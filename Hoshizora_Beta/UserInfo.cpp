#include "stdafx.h"
#include "UserInfo.h"


CUserInfo::CUserInfo(void)
{
}


CUserInfo::~CUserInfo(void)
{
}


vector<user_info> CUserInfo::GetUserInfo(const char * conditionString,QueryType type)
{
	return GetUserInfo(conditionString,vector<vector<DWORD>>(),static_cast<QueryType>(type|NO_PAYED_DATA));
}

vector<user_info> CUserInfo::GetUserInfo(const char * conditionString,vector<vector<DWORD>>& pay_info,QueryType type)
{
	basic_string<CHAR> && cond = string();

	switch (type&~NO_PAYED_DATA)
	{
	case ROOM_INFO:
		if (!strlen(conditionString))
		{
			return vector<user_info>();
		}
		cond = stringf("接入地点='%s'",conditionString);
		break;
	case CUSTOM:
		cond = string(conditionString);
		break;
	default:
		break;
	}
	vector<user_info> && v = CmsSQL::GetUserInfo(cond.c_str());
	
	if (type&NO_PAYED_DATA)
	{
		CmsSQL::MarkPayInfo(v);
	}
	else
	{
		pay_info = GetUserPayedInfo(v);
		MarkPayInfo(v,pay_info);
	}

	return v;
}

int CUserInfo::FillUserInfo(vector<user_info> & v, vector<vector<DWORD>>& pay_info, QueryType type)
{
	for (auto iter = v.begin();iter!=v.end();++iter)
	{
		if (iter->name.empty())
		{
			string && queryStr = 
				stringf("MAC地址='%.2X-%.2X-%.2X-%.2X-%.2X-%.2X'",
				iter->macAddress[0],iter->macAddress[1],iter->macAddress[2],
				iter->macAddress[3],iter->macAddress[4],iter->macAddress[5]);
			vector<user_info> && vSingle = CmsSQL::GetUserInfo(queryStr.c_str());
			if (vSingle.size())
			{
				DWORD flag = iter->flag;
				*iter = vSingle[0];
				if (type!=SEARCH)
				{
					iter->flag=flag&~RegedUser;
				}
				else
				{
					iter->flag=flag|RegedUser;
				}
			}
			else
			{
				iter->flag&=~RegedUser;
			}
		}
	}

	pay_info = GetUserPayedInfo(v);
	MarkPayInfo(v,pay_info);
	return 0;
}


size_t CUserInfo::MarkPayInfo(vector<user_info> & v, vector<vector<DWORD>> & pay_info)
{
	int pos = 0;

	for (auto iter = pay_info.begin();iter!=pay_info.end();++iter)
	{
		if (find(iter->begin(),iter->end(),time_magic)!=iter->end())
		{
			v[pos].flag|=Payed;
		}
		++pos;
	}
	return pay_info.size();
}
