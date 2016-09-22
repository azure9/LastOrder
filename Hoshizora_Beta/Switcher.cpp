#include "stdafx.h"
#include "Switcher.h"
/*
 *s5SbsSecurityAction
 *
 *1.3.6.1.4.1.45.1.6.5.3.5.0
 *noAction(1),trap(2),partitionPort(3),partitionPortAndsendTrap(4),daFiltering(5),
 *daFilteringAndsendTrap(6),partitionPortAnddaFiltering(7),partitionPortdaFilteringAndsendTrap(8)
 */
CSwitcher::CSwitcher(const char *ipAddr,LPCSTR ro,LPCSTR rw)
	:CSnmp(ipAddr,ro,rw)
{
	vector<Oid> OidList;
	OidList.push_back("1.3.6.1.2.1.2.1.0");//端口数
	OidList.push_back("1.3.6.1.4.1.45.1.6.5.3.3.0");//全局MAC安全
	OidList.push_back("1.3.6.1.4.1.45.1.6.4.2.1.13.0");//s5AgInfoCurDfltGwAddr
	OidList.push_back("1.3.6.1.4.1.45.1.6.5.3.5.0");//Mac safe action
	OidList.push_back("1.3.6.1.4.1.45.1.6.4.3.5.1.0");//远程登录IP（本机IP
	vector<Vb> && resList = GetList(OidList);
	if (resList.size()==5)
	{
		resList[0].get_value(portCnt);
		resList[1].get_value(globalMACSecu);
		resList[2].get_value(ipGateway);
		resList[3].get_value(safemode);
		resList[4].get_value(localIP);
		GetNext("1.3.6.1.4.1.45.1.6.5.3.10.1.1").get_value(BrdIndx);// Support for Avaya
	}
	else
	{
		throw static_cast<int>(-1);
	}
}


CSwitcher::~CSwitcher(void)
{
}


vector<PortStatus> CSwitcher::GetPortStatus(void)
{
	vector<Vb> && portName = WalkTable("1.3.6.1.2.1.2.2.1.2",portCnt);//port name
	vector<Vb> && portAdminS = WalkTable("1.3.6.1.2.1.2.2.1.7",portCnt);//port admin status
	vector<Vb> && portOperS = WalkTable("1.3.6.1.2.1.2.2.1.8",portCnt);//port operate status/ online status
	bridge = GetBridge();
	CPortSet macPort(Get("1.3.6.1.4.1.45.1.6.5.3.15.0"));//Port mac security
	vector<PortStatus> portSt;
	for (int i=0;i<portCnt;++i)
	{
		PortStatus port;
		int iStatus=0;
		if (i<portName.size())
		{
			const char * pPortName = portName[i].get_printable_value();
			const char * pName = strstr(pPortName,"Port");

			port.Name = pName?pName:pPortName;
		}
		//////////////////////////////////////////////////////////////////////////
		if (i<portAdminS.size())
		{
			portAdminS[i].get_value(iStatus);
			if (iStatus==1)//Up
			{
				port.bStatus|=PORT_ADMINSTATUS_ON;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if (i<portOperS.size())
		{
			portOperS[i].get_value(iStatus);//online status
			if (iStatus==1)//Up
			{
				port.bStatus|=PORT_OPERSTATUS_ON;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if (macPort.GetPos(i+1))//port MAC security
		{
			port.bStatus|=PORT_MACSECURITY_ON;
		}

		port.cntOnline = bridge.count(i+1);
		portSt.push_back(port);
	}
	return move(portSt);
}

multimap<int,MacAddress> CSwitcher::GetBridge(void)
{
	if (bridge.size())
	{
		return bridge;
	}
	map<Oid,Vb> && resTable = WalkEntry("1.3.6.1.2.1.17.4.3.1.2",portCnt);//bridge index(MAC dec) - port
	multimap<int,MacAddress> portOnlineMac;
	for (auto iter = resTable.begin();iter!=resTable.end();++iter)
	{
		int port;
		iter->second.get_value(port);
		MacAddress mac;
		for (int i=0;i<6;++i)
		{
			mac[i] = iter->first[i];
		}
		portOnlineMac.insert(pair<int,MacAddress>(port,move(mac)));
	}
	return move(portOnlineMac);
}


multimap<int,MacAddress> CSwitcher::GetPortSecurityList(void)
{
	Oid oid("1.3.6.1.4.1.45.1.6.5.3.10.1.5");

	oid+=BrdIndx;

	map<Oid,Vb> && resTable = WalkEntry(oid,50);//bridge index(MAC dec) - port
	multimap<int,MacAddress> portMacSecu;
	for (auto iter = resTable.begin();iter!=resTable.end();++iter)
	{
		int port = iter->first[0];
		MacAddress mac;
		for (int i=0;i<6;++i)
		{
			mac[i] = iter->first[i+1];
		}
		secuList[mac] = port;//map Security list mac-port
		portMacSecu.insert(pair<int,MacAddress>(port,move(mac)));
	}
	return move(portMacSecu);
}


vector<MacAddress> CSwitcher::GetPortSecurityList(int idxPort)
{
	Oid oid("1.3.6.1.4.1.45.1.6.5.3.10.1.5");

	oid+=BrdIndx;
	oid+=idxPort;

	map<Oid,Vb> && resTable = WalkEntry(oid,10);//bridge index(MAC dec) - port
	vector<MacAddress> portMacSecu;

	for (auto iter = resTable.begin();iter!=resTable.end();++iter)
	{
		MacAddress mac;
		for (int i=0;i<6;++i)
		{
			mac[i] = iter->first[i];
		}
		portMacSecu.push_back(move(mac));
	}
	return move(portMacSecu);
}


bool CSwitcher::SetPortAdminStatus(int idxPort,const int status)
{
	Oid oid="1.3.6.1.2.1.2.2.1.7";
	oid+=idxPort;
	Vb setValue;
	setValue.set_oid(oid);
	setValue.set_value(status?1:2);
	return Set(setValue);
}

bool CSwitcher::SetPortSecurityStatus(int idxPort, const int status)
{
	CPortSet macPort(Get("1.3.6.1.4.1.45.1.6.5.3.15.0"));//Port mac security
	macPort.SetPos(idxPort,status);
	Vb setValue;
	setValue.set_oid((Oid)"1.3.6.1.4.1.45.1.6.5.3.15.0");
	setValue.set_value(macPort.GetDesc(),64);
	return Set(setValue);
}

bool CSwitcher::GetGlobalMAC(void)
{
	return globalMACSecu==1?true:false;
}

bool CSwitcher::SetGlobalMAC(const int status)
{
	Vb setValue;
	setValue.set_oid((Oid)"1.3.6.1.4.1.45.1.6.5.3.3.0");
	setValue.set_value(status?1:2);
	return Set(setValue);
}

/*
 *s5SbsAuthCfgStatus 
 *
 *1.3.6.1.4.1.45.1.6.5.3.10.1.5.1.port.mac:access flag
 *
 *valid (1), create (2), delete (3), modify (4)
 *=.=Modify Flag真的有用么、这玩意咋用呢
"The status of the AuthCfg entry. The primary use of
this object is for modifying the AuthCfg table. Values
that can be written create(2), delete(3), modify(4).
Values that can be read: valid(1). Setting this entry
to delete(3) causes the entry to be deleted from the
table. Setting a new entry with create(2) causes the
entry to be created in the table. Setting an entry with
modify(4) causes the entry to be modified. The response
to a get request or get-next request will always indicate
a status of valid (1), since invalid entries are removed
from the table. "*/

size_t CSwitcher::DeleteMacFromPort(int idxPort, vector<MacAddress> & v)
{
	vector<MacAddress> fV;
	for (auto iter = v.begin();iter!=v.end();++iter)
	{
		Vb setValue;
		Oid oid("1.3.6.1.4.1.45.1.6.5.3.10.1.5");

		oid+=BrdIndx;
		oid+=idxPort;

		for (int i=0;i<6;++i)
		{
			oid+=(unsigned int)(*iter)[i];
		}

		setValue.set_oid(oid);
		setValue.set_value(3);
		if (!Set(setValue))
		{
			fV.push_back(move(*iter));//已经存在的，其实这里没必要考虑这么多的
		}
	}
	v = fV;
	return v.size();//failed count
}

size_t CSwitcher::DeleteMacFromPort(vector<pair<size_t,MacAddress>> & v)
{
	vector<pair<size_t,MacAddress>> fV;
	const size_t cntPair = 0;
	for (size_t idx = 0;idx<cntPair;++idx)
	{
		Vb setValue;
		Oid oid("1.3.6.1.4.1.45.1.6.5.3.10.1.5");

		oid+=BrdIndx;
		oid+=v[idx].first;

		for (int i=0;i<6;++i)
		{
			oid+=(unsigned int)v[idx].second[i];
		}

		setValue.set_oid(oid);
		setValue.set_value(3);
		if (!Set(setValue))
		{
			fV.push_back(move(v[idx]));//已经存在的，其实这里没必要考虑这么多的
		}
	}
	v = fV;
	return v.size();//failed count
}


size_t CSwitcher::AddMacToPort(int idxPort, vector<MacAddress>& v)
{
	vector<MacAddress> fV;

	if (!secuList.size())
	{
		GetPortSecurityList();
	}

	const size_t cnt = v.size();
	for (size_t idx = 0;idx<cnt;++idx)
	{
		auto iter = secuList.find(v[idx]);
		if (iter!=secuList.end())
		{
			if (iter->second==idxPort)
			{
				continue;// do nothing, already exist in security list
			}
			DeleteMacFromPort(iter->second,vector<MacAddress>(1,v[idx]));
		}

		Vb setValue;
		Oid oid("1.3.6.1.4.1.45.1.6.5.3.10.1.5");

		oid+=BrdIndx;
		oid+=idxPort;

		for (int i=0;i<6;++i)
		{
			oid+=(unsigned int)v[idx][i];
		}

		setValue.set_oid(oid);
		setValue.set_value(2);
		if (!Set(setValue))
		{
			//failed process
			fV.push_back(move(v[idx]));
		}
	}
	v = fV;
	return v.size();//failed count
}

bool CSwitcher::IsMacSecuOnPort(MacAddress & mac, int idxPort)
{
	Oid oid("1.3.6.1.4.1.45.1.6.5.3.10.1.5");

	oid+=BrdIndx;
	oid+=idxPort;

	for (int i=0;i<6;++i)
	{
		oid+=(unsigned int)mac[i];
	}

	int result;
	Get(oid).get_value(result);
	if (result==1)
		return true;
	return false;
}

int CSwitcher::GetMACSecuPort(MacAddress & mac)
{
	vector<Oid> OidList;

	for (int port = 0;port<portCnt;++port)
	{
		Oid oid("1.3.6.1.4.1.45.1.6.5.3.10.1.5");

		oid+=BrdIndx;
		oid+=(port+1);//base on port 1

		for (int i=0;i<6;++i)
		{
			oid+=(unsigned int)mac[i];
		}

		OidList.push_back(move(oid));
	}

	vector<Vb> && resList = GetList(OidList);

	const size_t cntRes = resList.size();
	for (int i = 0;i<cntRes;++i)
	{
		int result;
		resList[i].get_value(result);
		if (result==1)
		{
			return i+1;
		}
	}

	return 0;
}

//block port : set 	1.3.6.1.4.1.45.1.6.5.3.10.1.4.1.port.mac:access type[allow 	(1),block 	(2)]
//s5SbsAuthCfgAccessCtrlType
// 
// (╯‵□′)╯︵┻━┻北电我草你妈！写了那么多代码才发现你这根本就没法设置、靠！
int CSwitcher::PartitionUser(MacAddress & mac,int idxPort,bool partition)
{
	stringstream ss;string oidStr;Vb setValue;

	ss<<"1.3.6.1.4.1.45.1.6.5.3.10.1.4.1."<<idxPort;
	for (int i=0;i<6;++i)
	{
		ss<<"."<<(unsigned int)mac[i];
	}
	ss>>oidStr;

	setValue.set_oid((Oid)oidStr.c_str());
	setValue.set_value(partition?2:1);
	return Set(setValue);
}


/*
 *s5SbsAuthCfgSecureList
 *
 *1.3.6.1.4.1.45.1.6.5.3.10.1.6
 *
"The index of the security list. This value is meaningful
only if s5SbsAuthCfgBrdIndx and s5SbsAuthCfgPortIndx values
are zero. For other board and port index values
it should have the value of zero. This value is used
as an index into s5SbsSecurityListTable.
The corresponding MAC Address of this entry is allowed or blocked
on all the ports of that port list. "*/

/*
 * s5SbsViolationStatusMACAddress
 * 
 *1.3.6.1.4.1.45.1.6.5.3.12.1.3.1.port:MAC
 *
 "The MAC address of the device attempting unauthorized
 network access. (MAC addrees-based security)"*/


/*s5SbsSecurityMode
 *1.3.6.1.4.1.45.1.6.5.3.4.0
 *Clear all port security data
 *set to 3 then set to 2
 */

int CSwitcher::GetSafeMode(void)
{
	return safemode==6?0:safemode==3||safemode==4?1:-1;
}


int CSwitcher::SetSafeMode(int safemode)
{
	Vb setValue;
	setValue.set_oid((Oid)"1.3.6.1.4.1.45.1.6.5.3.5.0");
	setValue.set_value(safemode==0?this->safemode=6:this->safemode==1?
		this->safemode=4:this->safemode=0);
	return Set(setValue);
}


IpAddress CSwitcher::GetGateway(void)
{
	return ipGateway;
}


int CSwitcher::IsMacOnline(MacAddress & mac)
{
	Oid oid("1.3.6.1.2.1.17.4.3.1.2");
	int port = 0;

	for (int i=0;i<6;++i)
	{
		oid+=(unsigned int)mac[i];
	}
	
	Get(oid).get_value(port);
	
	if (port)
	{
		return port;
	}
	
	return 0;
}


IpAddress CSwitcher::GetLocalIP(void)
{
	return localIP;
}


int CSwitcher::GetTrapStatus(void)
{
	IpAddress ip;
	Get("1.3.6.1.4.1.45.1.6.4.3.4.1.4.1").get_value(ip);
	if (ip==localIP)
	{
		return true;
	}
	return false;
}


int CSwitcher::GetPortCnt(void)
{
	return portCnt;
}