#include "Router.h"
#include "stdafx.h"


CRouter::CRouter(const char *ipAddr,LPCSTR ro,LPCSTR rw,bool bBuffer):
	CSnmp(ipAddr,ro,rw),bBuffer(bBuffer)
{
	IpAddress mainIP;
	GetNext("1.3.6.1.4.1.25506.8.35.2.1.2.1.3").get_value(mainIP);

	if (mainIP != ipAddr)
	{
		string && sOid = stringf("1.3.6.1.2.1.4.20.1.2.%s",ipAddr);
		Get((Oid)sOid.c_str()).get_value(vlanId);
	}
	else
	{
		vlanId = -1;
	}
}


CRouter::~CRouter(void)
{
}


multimap<MacAddress,IpAddress> CRouter::GetVlanIPMACList(void)
{
	if (bBuffer&&m.size())
	{
		return m;
	}
	Oid query = "1.3.6.1.2.1.3.1.1.2";
	if (vlanId!=-1)//无关联交换机、在整个IP表中查询
	{
		query+=vlanId;
		query+=1;
	}
	map<Oid,Vb>&& resList = WalkEntry(query,50);
	multimap<MacAddress,IpAddress> MAC_IPTable;
	for (auto iter = resList.begin();iter!=resList.end();++iter)
	{
		MacAddress addr;
		iter->second.get_value(addr);
		MAC_IPTable.insert(pair<MacAddress,IpAddress>(addr,iter->first.get_printable()));
	}

	m = MAC_IPTable;
	return move(MAC_IPTable);
}


vector<IpAddress> CRouter::GetIPFromMac(vector<MacAddress> & macList)
{
	multimap<MacAddress,IpAddress> && resList = GetVlanIPMACList();
	vector<IpAddress> v;//储存结果

	IpAddress invaildIP("0.0.0.0");
	for (auto iter = macList.begin();iter!=macList.end();++iter)
	{
		auto it = resList.find(*iter);
		if (it!=resList.end())
		{
			v.push_back(it->second);
		}
		else
		{
			v.push_back(invaildIP);
		}
	}

	return move(v);
}

vector<IpAddress> CRouter::GetIPFromMac(MacAddress & mac)
{
	multimap<MacAddress,IpAddress> && resList = GetVlanIPMACList();
	vector<IpAddress> v;//储存结果
	
	auto iter = resList.lower_bound(mac);
	if (iter!=resList.end())
	{
		for (;iter!=resList.upper_bound(mac);++iter)
		{
			v.push_back(iter->second);
		}
	}
	if (!v.size())
	{
		v.push_back("0.0.0.0");
	}

	return move(v);
}

MacAddress CRouter::GetMacFromIP(IpAddress & ip)
{
	auto proc = [&](int ifIndex,MacAddress &mac){
		Oid oid("1.3.6.1.2.1.3.1.1.2");
		oid+=ifIndex;
		oid+=1;
		for (int i=0;i<4;++i)
		{
			oid+=ip[i];
		}

		Get(oid).get_value(mac);
		if (mac.valid())
		{
			ip = oid;
			return true;
		}
		return false;
	};


	if (vlanId != -1)
	{
		MacAddress mac;
		if (proc(vlanId,mac))
		{
			return move(mac);
		}
		return MacAddress();
	}

	map<Oid,Vb> && resList = WalkEntry("1.3.6.1.2.1.4.20.1.2",50);
	for (auto iter = resList.begin();iter!=resList.end();++iter)
	{
		int ifIndex;
		iter->second.get_value(ifIndex);

		MacAddress mac;
		if (proc(ifIndex,mac))
		{
			return move(mac);
		}
	}
	return MacAddress();
}