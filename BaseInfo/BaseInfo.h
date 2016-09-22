#pragma once
#include <windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>
#include <string>
#include <map>
#include <algorithm>
#include "sqlitepp/sqlitepp.hpp"
#pragma comment(lib,"sqlite.lib")
#pragma comment(lib,"sqlitepp.lib")

using namespace std;
using namespace sqlitepp;

#ifdef ELEMENT_EXPORTS
#define BASEINFO_API __declspec(dllexport)
#else
#define BASEINFO_API __declspec(dllimport)
#endif // ELEMENT_EXPORTS

typedef struct SchemeContent
{
	string name;
	string prefix;
	string custom_cmd;
	SchemeContent(string & name,string & prefix,string & custom_cmd)
		:name(move(name)),prefix(move(prefix)),custom_cmd(move(custom_cmd))
	{

	}
};


class BASEINFO_API CBaseInfo
{
private:
	vector<pair<string,string>> communitys;
	vector<SchemeContent> vSC;
	session db;
	int scheme;
public:
	CBaseInfo(LPCSTR dbPath);
	~CBaseInfo(void);
	int SetScheme(int scheme);
	vector<SchemeContent> LoadScheme(void);
	int GetDeviceScheme(string & ip);
	SchemeContent LoadScheme(int idxScheme);
	int UpdateAllSchemeDB(vector<vector<pair<string,string>>> & v);
	size_t GetAllPortDescr(map<pair<int,int>,string> & v);
	string GetLocationInfoS(string ip);
	int UpdateSelectDB(vector<pair<string,string>> &v);
	pair<string,string> GetCommunity(string ip);
	vector<string> RoomImagination(string room);
	vector<string> IPImagination(string & ip,int device_type);
	vector<string> IPImaginationS(string & ip);
	vector<string> IPImaginationR(string & ip);
	string GetIPFromDescrS(string descr,int &port);
	map<int,string> GetPortListDescrS(string ip);
	string GetPortDescrS(string ip,int port);
	vector<string> GetAllIPS(void);
	vector<string> GetAllIPR(void);
	vector<string> GetAllGateway(void);
	int GetScheme();
	size_t GetAllIP(map<string,int> & v);
};

#ifdef ELEMENT_EXPORTS
#else
extern CBaseInfo *bs;
#endif // ELEMENT_EXPORTS
