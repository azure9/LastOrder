#pragma once
#include "snmp.h"

class CRouter :
	public CSnmp
{
private:
	int vlanId;
	bool bBuffer;
	multimap<MacAddress,IpAddress> m;
public:
	CRouter(const char *ipAddr,LPCSTR ro,LPCSTR rw,bool bBuffer = true);
	~CRouter(void);
	multimap<MacAddress,IpAddress> GetVlanIPMACList(void);
	vector<IpAddress> GetIPFromMac(vector<MacAddress> & macList);
	vector<IpAddress> GetIPFromMac(MacAddress & mac);
	MacAddress GetMacFromIP(IpAddress & ip);
	bool operator == (CRouter &x)
	{
		return x.GetDeviceIPAddress()==GetDeviceIPAddress()&&x.vlanId==vlanId;
	}
};

