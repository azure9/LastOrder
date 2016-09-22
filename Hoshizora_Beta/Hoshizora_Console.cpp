// Hoshizora_Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Snmp.h"
#include "Switcher.h"
#include "Router.h"
//#include "BaseInfo.h"
#include "UserInfo.h"
#include <windows.h>

#ifndef _SNMP_PUBLIC_INCLUDE
#define _SNMP_PUBLIC_INCLUDE
#include <winsock2.h>
#include "snmp_pp/snmp_pp.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"libdes.lib")
#pragma comment(lib,"snmp++.lib")

using namespace Snmp_pp;
#endif
#include <iostream>
#include <windows.h>
#include <sqltypes.h>
#include <functional>
#include <sql.h>
#include <sqlext.h>
#include <algorithm>
#include <stdexcept>
#include <string>

using namespace std;

char *ipList[]={"10.200.101.252",
	"10.200.101.253",
	"10.200.102.253",
	"10.200.105.253",
	"10.200.106.251",
	"10.200.106.253",
	"10.200.107.253",
	"10.200.108.253",
	"10.200.109.252",
	"10.200.109.253",
	"10.200.111.253",
	"10.200.115.253",
	"10.200.117.253",
	"10.200.119.253",
	"10.200.121.253",
	"10.200.122.252",
	"10.200.122.253",
	"10.200.124.251",
	"10.200.124.252",
	"10.200.124.253",
	"10.200.126.253",
	"10.200.129.240",
	"10.200.129.241",
	"10.200.129.242",
	"10.200.129.243",
	"10.200.129.244",
	"10.200.129.245",
	"10.200.129.246",
	"10.200.129.247",
	"10.200.129.248",
	"10.200.129.249",
	"10.200.129.250",
	"10.200.129.251",
	"10.200.129.252",
	"10.200.129.253",
	"10.200.130.250",
	"10.200.130.251",
	"10.200.130.252",
	"10.200.130.253",
	"10.200.132.251",
	"10.200.132.252",
	"10.200.132.253",
	"10.200.138.253",
	"10.200.141.252",
	"10.200.141.253",
	"10.200.145.252",
	"10.200.145.253",
	"10.200.149.251",
	"10.200.149.252",
	"10.200.149.253",
	"10.200.155.251",
	"10.200.155.252",
	"10.200.155.253",
	"10.200.161.251",
	"10.200.161.252",
	"10.200.161.253",
	"10.200.167.251",
	"10.200.167.252",
	"10.200.167.253",
	"10.200.173.252",
	"10.200.173.253",
	"10.200.177.251",
	"10.200.177.252",
	"10.200.177.253",
	"10.200.183.252",
	"10.200.183.253",
	"10.200.187.252",
	"10.200.187.253",
	"10.200.191.253",
	"10.200.193.252",
	"10.200.193.253",
	"10.200.197.252",
	"10.200.197.253",
	"10.200.201.252",
	"10.200.201.253",
	"10.200.21.241",
	"10.200.21.242",
	"10.200.211.252",
	"10.200.211.253",
	"10.200.213.253",
	"10.200.215.251",
	"10.200.215.252",
	"10.200.215.253",
	"10.200.217.251",
	"10.200.217.252",
	"10.200.217.253",
	"10.200.219.251",
	"10.200.219.252",
	"10.200.219.253",
	"10.200.22.241",
	"10.200.221.252",
	"10.200.221.253",
	"10.200.223.253",
	"10.200.225.251",
	"10.200.225.252",
	"10.200.225.253",
	"10.200.229.253",
	"10.200.23.241",
	"10.200.23.242",
	"10.200.23.243",
	"10.200.230.253",
	"10.200.233.252",
	"10.200.233.253",
	"10.200.235.252",
	"10.200.235.253",
	"10.200.236.253",
	"10.200.237.251",
	"10.200.237.252",
	"10.200.237.253",
	"10.200.24.241",
	"10.200.240.239",
	"10.200.240.240",
	"10.200.240.241",
	"10.200.240.242",
	"10.200.240.243",
	"10.200.240.244",
	"10.200.240.245",
	"10.200.240.246",
	"10.200.240.247",
	"10.200.240.248",
	"10.200.240.249",
	"10.200.240.250",
	"10.200.240.251",
	"10.200.240.252",
	"10.200.240.253",
	"10.200.241.252",
	"10.200.241.253",
	"10.200.242.251",
	"10.200.242.252",
	"10.200.242.253",
	"10.200.250.240",
	"10.200.250.251",
	"10.200.250.252",
	"10.200.250.253",
	"10.200.254.253",
	"10.200.30.253",
	"10.201.116.252",
	"10.201.116.253",
	"10.228.255.248",
	"10.228.255.249",
	"10.228.255.250",
	"10.228.255.251",
	"10.228.255.252",
	"10.228.255.253",
	"10.252.237.253",
	"10.252.238.251",
	"10.252.238.252",
	"10.252.238.253",
	"10.252.241.253",
	"10.252.242.253",
	"10.252.243.252",
	"10.252.243.253",
	"10.252.244.252",
	"10.252.244.253",
	"10.252.248.253",
	"10.252.250.249",
	"10.252.250.250",
	"10.252.250.251",
	"10.252.250.252",
	"10.252.250.253",
	"10.252.251.252",
	"10.252.251.253",
	"10.252.252.253",
	"10.252.253.253",
	"10.252.254.253",
	"10.252.255.252",
	"10.252.255.253",
	"118.228.196.251",
	"118.228.196.252",
	"118.228.196.253",
	"118.228.197.251",
	"118.228.197.252",
	"118.228.197.253",
	"118.228.198.251",
	"118.228.198.252",
	"118.228.198.253",
	"118.228.199.250",
	"118.228.199.251",
	"118.228.199.252",
	"118.228.199.253",
	"118.228.200.252",
	"118.228.200.253",
	"118.228.201.251",
	"118.228.201.252",
	"118.228.201.253",
	"118.228.213.253",
	"118.228.214.251",
	"118.228.214.252",
	"118.228.214.253",
	"118.228.215.251",
	"118.228.215.252",
	"118.228.215.253",
	"118.228.216.251",
	"118.228.216.252",
	"118.228.216.253",
	"118.228.217.252",
	"118.228.217.253",
	"118.228.218.252",
	"118.228.218.253",
	"118.228.219.252",
	"118.228.219.253",
	"118.228.220.251",
	"118.228.220.252",
	"118.228.220.253",
	"118.228.221.251",
	"118.228.221.252",
	"118.228.221.253",
	"118.228.222.251",
	"118.228.222.252",
	"118.228.222.253",
	"118.228.223.252",
	"118.228.223.253",
	"202.204.194.251",
	"202.204.194.252",
	"202.204.195.242",
	"202.204.195.243",
	"202.204.195.244",
	"202.204.195.245",
	"202.204.195.246",
	"202.204.195.247",
	"202.204.195.248",
	"202.204.195.249",
	"202.204.195.250",
	"202.204.195.251",
	"202.204.195.253",
	"202.204.196.251",
	"202.204.196.252",
	"202.204.196.252",
	"202.204.196.253",
	"202.204.197.246",
	"202.204.197.247",
	"202.204.197.248",
	"202.204.197.249",
	"202.204.197.250",
	"202.204.197.251",
	"202.204.197.252",
	"202.204.197.253",
	"202.204.198.251",
	"202.204.198.252",
	"202.204.198.253",
	"202.204.199.253",
	"202.204.200.219",
	"202.204.201.253",
	"202.204.203.252",
	"202.204.203.253",
	"202.204.204.253",
	"202.204.205.253",
	"202.204.206.251"};


#include "msSQL.h"

int main(int argc, char* argv[])
{
	CUserInfo info;
	vector<user_info>&&v = info.GetUserInfo("润杰1-501");
	vector<MacAddress>&& vec = info.GetValidUserMac(v);
	CSwitcher session("10.200.145.253");
	vector<MacAddress> &&stdMac= session.GetPortSecurityList(35);
	vector<MacAddress> newMac;
	for_each(v.begin(),v.end(),[&](user_info x)
	{
		//replace_if(x.macAddress.begin(),x.macAddress.end(),bind2nd(std::equal_to<char>(),'-'),':');
		newMac.push_back(MacAddress(x.macAddress.c_str()));//假设这里已经实现了==重载判断mac地址是否相等
	});
	auto && oldMacList = GetDelta(newMac,stdMac);//old to delete
	auto && newMacList = GetDelta(stdMac,newMac);//new to add
	printf("New List:\n");
	for_each(newMacList.begin(),newMacList.end(),[=](MacAddress x)
	{
		printf("%s\n",x.get_printable());
	});
	printf("Old List:\n");
	for_each(oldMacList.begin(),oldMacList.end(),[=](MacAddress x)
	{
		printf("%s\n",x.get_printable());
	});
	session.DeleteMacFromPort(35,oldMacList);
	session.AddMacToPort(35,newMacList);
// 	CmsSQL querySQL;
// 	vector<user_info> && v = querySQL.GetUserInfo(_T("接入地点='润杰1-501'"));
// // 	vector<user_info> v;
// // 	v.push_back(user_info(0,"","","",""));
// 	vector<vector<DWORD>>&& pay = querySQL.GetUserPayedInfo(v);
// 	CSwitcher session("10.200.145.253");
// 	vector<MacAddress> &&stdMac= session.GetPortSecurityList(35);
// // 	vector<MacAddress> stdMac;
// // 	stdMac.push_back(MacAddress("18:03:73:99:82:99"));
// // 	printf("%s\n",stdMac[0].get_printable());
// // 	stdMac.push_back(MacAddress("20-16-D8-F5-0F-FD"));
// // 	stdMac.push_back(MacAddress("60-FA-CD-18-07-1E"));
// // 	stdMac.push_back(MacAddress("7C-E9-D3-ED-60-44"));
// // 	stdMac.push_back(MacAddress("AC-72-89-E9-4A-D0"));
// // 	stdMac.push_back(MacAddress("E8-03-9A-87-FB-36"));
// 	vector<MacAddress> newMac;
// 	for_each(v.begin(),v.end(),[&](user_info x)
// 	{
// 		//replace_if(x.macAddress.begin(),x.macAddress.end(),bind2nd(std::equal_to<char>(),'-'),':');
// 		newMac.push_back(MacAddress(x.macAddress.c_str()));//假设这里已经实现了==重载判断mac地址是否相等
// 	});
// 	auto && oldMacList = GetDelta(newMac,stdMac);//old to delete
// 	auto && newMacList = GetDelta(stdMac,newMac);//new to add

// 	CBaseInfo bs("data.db");
// 	string str = bs.GetLocationInfo("10.200.145.253");
// 	vector<string> v = bs.IPImagination("10.200.145");
// 	v = bs.RoomImagination("2-");
// 	Snmp::socket_startup();
// 	session db("data.db");
// 	statement st(db);
// 	string ip;
// 	vector<string> v;
// 	st << "select ip from switcher_list",into(ip);
// 	while (st.exec())
// 	{
// 		v.push_back(ip);
// 	}
// 	db<< "BEGIN;";
// 	map<string,int> STypeList;
// 	for_each(v.begin(),v.end(),[&](string ip)
// 	{
// 		CSnmp session((LPSTR)ip.c_str());
// 		string type = session.Get(".1.3.6.1.2.1.1.1.0").get_printable_value();
// 		if (type.empty())
// 		{
// 			st<<"DELETE FROM switcher_list WHERE ip=:ip;",use(ip);
// 			st.exec();
// 		}
// 		else
// 		{
// 			STypeList.insert(pair<string,int>(type,STypeList.size()));
// 			st << "UPDATE switcher_list SET type=:type where ip=:ip;",use(STypeList.find(type)->second),use(ip);
// 			st.reset(true);
// 			st.exec();
// 		}
// 	});
// 	db << "create table switcher_type(type_id integer primary key, descr text)";
// 	for_each(STypeList.begin(),STypeList.end(),[&](pair<string,int>x)
// 	{
// 		st <<"insert into switcher_type values(:x , :descr)", use(x.second), use(x.first.c_str());
// 		st.reset(true);
// 		st.exec();
// 	});
// 	db << "COMMIT;";
// 	vector<string> v;
// 	for (int i=0;i<248;i++)
// 	{
// 		v.push_back(ipList[i]);
// 	}
// 	for_each(v.begin(),v.end(),[&](string ip)
// 	{
// 		CSnmp session((LPSTR)ip.c_str());
// 		unsigned long len;
// 		BYTE mac[7];char macStr[18];
// 		session.Get("1.3.6.1.2.1.17.1.1.0").get_value((unsigned char *)mac,len);
// 		sprintf(macStr,"%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",mac[0],mac[1],mac[2],
// 			mac[3],mac[4],mac[5]);
// 		cout<<ip.c_str()<<"\t"<<macStr<<endl;
// 	});
// 	CRouter session(NULL,"10.200.145.254");
// 	vector<string> macList;
// 	macList.push_back("dc:0e:a1:9b:51:56");
// 	map<string,string> resList = session.GetIPFromMac(macList);
// 	for_each(resList.begin(),resList.end(),[&](pair<string,string> x)
// 	{
// 		cout << x.first <<"  ";
// 		cout << x.second <<endl;
// 	});

//	CSnmp session("202.204.200.229");
//	cout << session.Get("1.3.6.1.2.1.1.1.0").get_printable_value();
//	map<string,Vb> && resList = session.WalkEntry("1.3.6.1.2.1.3.1.1.2.41",50);//华三的设备最多只能一次传50个
//	printf("Got %d.\n",resList.size());
//	CSwitcher session(argv[1]);
// 	CSwitcher session("10.200.145.253");
// 	multimap<int,string> &&macLst= session.GetPortSecurityList();
// 	for_each(macLst.begin(),macLst.end(),[&](pair<int,string> x)
// 	{
// 		cout<<"On Port "<<x.first<<" : "<<x.second.c_str()<<endl;
// 	});
// 	vector<string> &&macLst= session.GetPortSecurityList(7);
// 	for_each(macLst.begin(),macLst.end(),[&](string x)
// 	{
// 		cout<<"On Port 7: "<<x<<endl;
// 	});
//	vector<PortStatus> && port = session.GetPortStatus();
//	for_each(portStatus.begin(),portStatus.end(),[&](pair<string,BYTE> x)
//	multimap<int,string>&& resList = session.GetBridge();
// 	for_each(resList.begin(),resList.end(),[&](pair<int,string> x)
// 	{
// 		cout<<"Port:"<<x.first<<endl;
// 		cout<<x.second.c_str()<<endl;
// 	});
// 	for_each(port.begin(),port.end(),[&] (PortStatus st)
// 	{
// 		cout<<st.Name.c_str()<<endl;
// 		if (st.bStatus&PORT_ADMINSTATUS_ON)
// 		{
// 			cout<<"Port on!  ";
// 		}
// 		if (st.bStatus&PORT_OPERSTATUS_ON)
// 		{
// 			cout<<"Port online!   ";
// 		}
// 		else
// 			cout<<"Port offline!  ";
// 		if(st.bStatus&PORT_MACSECURITY_ON)
// 			cout<<"Port Security on!"<<endl;
// 		else
// 		{
// 			cout<<"Port security off!"<<endl;
// 		}
// 	});
//	Snmp::socket_cleanup();
	return 0;
}

