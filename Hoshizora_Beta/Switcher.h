#pragma once
#include "snmp.h"
#include "Router.h"
#include "PortSet.h"

#define  PORT_MACSECURITY_ON 1
#define  PORT_ADMINSTATUS_ON 2
#define  PORT_OPERSTATUS_ON  4

typedef struct _PortStatus
{
	BYTE bStatus;
	string Name;
	size_t cntOnline;
	_PortStatus()
		:bStatus(0),cntOnline(0),Name(""){};
}PortStatus;

class CSwitcher :
	public CSnmp
{
private:
	int BrdIndx;
	int portCnt;
	int globalMACSecu;
	int safemode;
	IpAddress ipGateway;
	IpAddress localIP;
	multimap<int,MacAddress> bridge;
	map<MacAddress,int> secuList;
public:
	CSwitcher(const char *ipAddr,LPCSTR ro,LPCSTR rw);
	~CSwitcher(void);
	vector<PortStatus> GetPortStatus(void);
	multimap<int,MacAddress> GetBridge(void);
	multimap<int,MacAddress> GetPortSecurityList(void);
	vector<MacAddress> GetPortSecurityList(int idxPort);
	bool SetPortAdminStatus(int idxPort,const int status);
	bool SetPortSecurityStatus(int idxPort, const int status);
	bool GetGlobalMAC(void);
	bool SetGlobalMAC(const int status);
	size_t DeleteMacFromPort(int idxPort, vector<MacAddress> & v);
	size_t DeleteMacFromPort(vector<pair<size_t,MacAddress>> & v);
	size_t AddMacToPort(int idxPort, vector<MacAddress>& v);
	int GetSafeMode(void);
	int SetSafeMode(int safemode);
	IpAddress GetGateway(void);
	int PartitionUser(MacAddress & mac,int idxPort,bool partition);
	int IsMacOnline(MacAddress & mac);
	IpAddress GetLocalIP(void);
	int GetTrapStatus(void);
	int GetPortCnt(void);
	bool operator == (CSwitcher &x)
	{
		return x.GetDeviceIPAddress()==GetDeviceIPAddress();
	}
	bool IsMacSecuOnPort(MacAddress & mac, int idxPort);
	int GetMACSecuPort(MacAddress & mac);
};

