#pragma once
#include "stdafx.h"
#include "Snmp.h"
#include <tchar.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

basic_string<CHAR> stringf(CHAR* format,...);
basic_string<WCHAR> stringf(WCHAR* format,...);

template <typename T>
vector<T> GetDelta(vector<T> &src0,vector<T> &src1)
{
	vector<T> delta;
	vector<T>::iterator it;
	it=src1.begin();
	while (it!=src1.end())
	{
		if (find(src0.begin(),src0.end(),*it)==src0.end())
		{
			delta.push_back(*it);
		}
		it++;
	}
	return move(delta);
}

typedef enum _USER_STATUS_FLAG
{
	Payed=1,
	Online,//OnSwitcher ²»´ú±íOnline
	RegedUser = 4,
	OnSwitcher = 8,
}USER_STATUS_FLAG;

typedef struct user_info{
	string userid;
	string name;
	string tel;
	string local;
	MacAddress macAddress;
	DWORD flag;
	size_t idx;
	user_info(string userid,const char *name,const char *tel,const char *local,MacAddress macAddress,DWORD flag,size_t idx)
		:userid(userid),name(name),tel(tel),local(local),macAddress(macAddress),flag(flag),idx(idx){};
};

bool operator == (const user_info &a,const MacAddress &b);
bool operator == (const user_info &a,char *routerIP);
bool operator == (const user_info &a,size_t idx);

#define SEL_MESSAGE WM_USER+0x426