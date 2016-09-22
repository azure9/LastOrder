#include "stdafx.h"
#include "common.h"

string stringf(CHAR* format,...)
{
	va_list vArgList;
	va_start(vArgList, format);
	basic_string<CHAR> str(_vsnprintf(NULL, 0, format, vArgList),0);
	_vsnprintf((CHAR *)str.c_str(), str.size(), format, vArgList);
	va_end(vArgList);
	return move(str);
}

wstring stringf(WCHAR* format,...)
{
	va_list vArgList;
	va_start(vArgList, format);
	basic_string<WCHAR> str(_vsnwprintf(NULL, 0, format, vArgList),0);
	_vsnwprintf((WCHAR *)str.c_str(), str.size(), format, vArgList);
	va_end(vArgList);
	return move(str);
}

bool operator == (const user_info &a,const MacAddress &b)
{
	return a.macAddress==b;
}

bool operator == (const user_info &a,char *routerIP)
{
	return a.tel==routerIP;
}

bool operator == (const user_info &a,size_t idx)
{
	return a.idx==idx;
}