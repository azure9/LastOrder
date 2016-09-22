#pragma once
#include "SnmpBase.h"
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <functional>
#include <ppl.h>
#ifndef _SNMP_PUBLIC_INCLUDE
#define _SNMP_PUBLIC_INCLUDE
#include <winsock2.h>
#include "snmp_pp/snmp_pp.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"libdes.lib")
#pragma comment(lib,"snmp++.lib")

using namespace Snmp_pp;
#endif
using namespace std;

class CSnmp
{
public:
	CSnmp(LPCSTR szAddress,LPCSTR ro,LPCSTR rw);
	~CSnmp(void);
private:
	UdpAddress ipClient;
	CTarget conClient;
	Concurrency::critical_section cs;
public:
	Vb GetNext(char *oid);
	Vb GetNext(Oid &oid);
	vector<Vb> GetList(vector<string> & oidList);
	vector<Vb> GetList(vector<Oid> & oidList);
	Vb Get(char * oid);
	Vb Get(Oid & oid);
	map<Oid,Vb> WalkEntry(const char * oidEntry,int count = 9,int roffset = 0);
	map<Oid,Vb> WalkEntry(Oid & oidEntry,int count = 9,int roffset = 0);
	vector<Vb> WalkTable(char * oidEntry, int count = 9);
	vector<Vb> WalkTable(Oid &oidEntry, int count = 9);
	bool Set(Vb setValue);
	bool SetList(Pdu & pdu);
	MacAddress GetDeviceMAC(void);
	IpAddress GetDeviceIPAddress(void);
	virtual bool operator == (const string &x)
	{
		return GetDeviceIPAddress().get_printable()==x;
	}
	friend bool operator == (CSnmp *pThis,const string &x);
};

